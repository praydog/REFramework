<#
.SYNOPSIS
Runs a set of Lua scripts (mods) in a game with two REFramework builds and compares the script errors.

.DESCRIPTION
A compatibility check for changes to the Lua API: point it at real scripts and two builds (e.g. master and a branch).
For each build it installs the scripts, starts the game, waits for Lua to initialize plus -Seconds, closes the game,
and collects the error lines from the REFramework log and the text of any "ScriptRunner Message" boxes (which it
dismisses). Then it compares the distinct messages of both runs. Counts are ignored: per-frame errors repeat for as
long as the game ran.

-Scripts is a folder with either a reframework\ subfolder (copied as is: autorun, data, fonts) or Lua files (copied
into reframework\autorun). Both runs start from the same copy of the scripts' data folder. The game's own autorun,
data and fonts folders, its dinput8.dll and the files REFramework writes next to it (config, logs, ref_ui.ini) are
put back afterwards.

Exit code 0 means both builds produced the same distinct messages.

.EXAMPLE
powershell -ExecutionPolicy Bypass -File tests\run\Compare-ScriptErrors.ps1 -Game re2 -Scripts C:\mods\re2-autorun -BuildA C:\builds\master\dinput8.dll -BuildB C:\builds\branch\dinput8.dll
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory)] [string] $Game,
    [Parameter(Mandatory)] [string] $Scripts,
    [Parameter(Mandatory)] [string] $BuildA,
    [Parameter(Mandatory)] [string] $BuildB,
    # How long the game runs after Lua initializes.
    [int] $Seconds = 120,
    [string] $OutDir
)

$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'RFTest.psm1') -Force
$g = Resolve-Game $Game
if (-not $g.GameDir) { throw "$($g.Name) is not installed" }
$dir = $g.GameDir
$rf = Join-Path $dir 'reframework'
if (-not $OutDir) { $OutDir = Join-Path (Get-RFTestSetting 'outDir' (Join-Path (Get-RFTestsDir) 'out')) "compare-$($g.Alias)-$(Get-Date -Format yyyyMMdd-HHmmss)" }
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
# Scratch space next to the game, so moving the game's folders aside stays on one volume.
$work = Join-Path $dir 'rftest-compare'
if (Test-Path -LiteralPath $work) { throw "$work exists: an earlier run did not finish; put its game-* folders and files back by hand first" }
$watcher = Join-Path $PSScriptRoot 'tools\Watch-MessageBoxes.ps1'
$subdirs = 'autorun', 'data', 'fonts'
$config = Join-Path $dir 're2_fw_config.txt'
# Files REFramework writes next to the game; saved and put back like the folders above.
$rootFiles = 're2_fw_config.txt', 're2_framework_log.txt', 're2_framework_log.prev.txt', 'ref_ui.ini',
    'reframework_accessed_files.txt', 'reframework_loose_files.txt', 'reframework_faulty_files.txt', 'reframework_crash.dmp'

function Set-ConfigValue([string] $Key, [string] $Value) {
    $lines = @(if (Test-Path -LiteralPath $config) { Get-Content -LiteralPath $config })
    if ($lines -match "^$Key=") { $lines = $lines | ForEach-Object { if ($_ -like "$Key=*") { "$Key=$Value" } else { $_ } } }
    else { $lines += "$Key=$Value" }
    Set-Content -LiteralPath $config -Value $lines
}

function Install-Scripts {
    New-Item -ItemType Directory -Force -Path $rf | Out-Null
    foreach ($s in $subdirs) { if (Test-Path -LiteralPath (Join-Path $rf $s)) { Remove-Item -LiteralPath (Join-Path $rf $s) -Recurse -Force } }
    $nested = Join-Path $Scripts 'reframework'
    if (Test-Path -LiteralPath $nested) {
        foreach ($s in $subdirs) { if (Test-Path -LiteralPath (Join-Path $nested $s)) { Copy-Item -LiteralPath (Join-Path $nested $s) -Destination $rf -Recurse } }
    } else {
        New-Item -ItemType Directory -Force -Path (Join-Path $rf 'autorun') | Out-Null
        Get-ChildItem -LiteralPath $Scripts -Force | Copy-Item -Destination (Join-Path $rf 'autorun') -Recurse
    }
    New-Item -ItemType Directory -Force -Path (Join-Path $rf 'data') | Out-Null
}

function Invoke-Run([string] $Tag, [string] $Build) {
    Write-Host "== $Tag ($Build)"
    # Same starting data for both builds.
    Remove-Item -LiteralPath (Join-Path $rf 'data') -Recurse -Force
    Copy-Item -LiteralPath (Join-Path $work 'pristine-data') -Destination (Join-Path $rf 'data') -Recurse
    Install-RFBuild -GameDir $dir -Build $Build
    $boxes = Join-Path $OutDir "$Tag-messageboxes.txt"
    $stop = Join-Path $work "$Tag.stop"
    Remove-Item -LiteralPath $boxes, $stop -Force -ErrorAction SilentlyContinue
    Invoke-ScriptInDesktopSession -Script $watcher -Arguments "-Log `"$boxes`" -StopFile `"$stop`" -Seconds 1800"
    $log = Get-RFLogPath $dir
    $t0 = Get-Date
    try {
        [void](Start-RFGame -GameDir $dir -AppId $g.AppId -Exe $g.Exe)
        while (((Get-Date) - $t0).TotalSeconds -lt 300) {
            if ((Test-Path -LiteralPath $log) -and (Get-Item -LiteralPath $log).LastWriteTime -gt $t0 -and
                (Select-String -LiteralPath $log -Pattern 'Lua state initialized' -Quiet)) { break }
            if (-not (Get-GameProcess $dir)) { Write-Warning 'game exited before Lua initialized'; break }
            Start-Sleep 5
        }
        Start-Sleep $Seconds
    } finally {
        Stop-RFGame -GameDir $dir
        Set-Content -LiteralPath $stop -Value 'stop'
        Restore-RFBuild -GameDir $dir
    }
    # Normalize: drop timestamps and addresses so the two runs compare.
    $lines = @()
    if (Test-Path -LiteralPath $log) {
        $lines = Select-String -LiteralPath $log -Pattern '\[error\]' | ForEach-Object {
            ($_.Line -replace '^\[[^\]]*\] ', '') -replace '0x[0-9A-Fa-f]{6,}', '0x?' -replace '\b[0-9A-Fa-f]{12,}\b', '<hex>'
        }
    }
    Start-Sleep 2
    $box = @(if (Test-Path -LiteralPath $boxes) { Get-Content -LiteralPath $boxes | ForEach-Object { 'MSGBOX ' + ($_ -replace '\b[0-9A-Fa-f]{12,}\b', '<hex>') } })
    $grouped = (@($lines) + $box) | Group-Object | Sort-Object Name
    $grouped | ForEach-Object { '{0,6} {1}' -f $_.Count, $_.Name } | Set-Content -LiteralPath (Join-Path $OutDir "$Tag-errors.txt")
    Write-Host "$($grouped.Count) distinct messages"
    @($grouped | ForEach-Object { $_.Name })
}

if (Get-GameProcess $dir) { throw 'the game is already running; close it first' }

# Set the game's own files aside.
New-Item -ItemType Directory -Path $work | Out-Null
$hadRf = Test-Path -LiteralPath $rf
foreach ($s in $subdirs) {
    $p = Join-Path $rf $s
    if (Test-Path -LiteralPath $p) { Move-Item -LiteralPath $p -Destination (Join-Path $work "game-$s") }
}
foreach ($f in $rootFiles) {
    $p = Join-Path $dir $f
    if (Test-Path -LiteralPath $p) { Copy-Item -LiteralPath $p -Destination (Join-Path $work "game-$f") }
}

try {
    Install-Scripts
    Copy-Item -LiteralPath (Join-Path $rf 'data') -Destination (Join-Path $work 'pristine-data') -Recurse
    # Script errors also go to the log file, not only to the REFramework UI.
    Set-ConfigValue 'ScriptRunner_LogToDisk' 'true'
    $a = Invoke-Run 'A' $BuildA
    $b = Invoke-Run 'B' $BuildB
} finally {
    foreach ($s in $subdirs) {
        $p = Join-Path $rf $s
        if (Test-Path -LiteralPath $p) { Remove-Item -LiteralPath $p -Recurse -Force }
        $saved = Join-Path $work "game-$s"
        if (Test-Path -LiteralPath $saved) { Move-Item -LiteralPath $saved -Destination $p }
    }
    if (-not $hadRf -and (Test-Path -LiteralPath $rf) -and -not (Get-ChildItem -LiteralPath $rf -Force)) { Remove-Item -LiteralPath $rf -Force }
    foreach ($f in $rootFiles) {
        $saved = Join-Path $work "game-$f"
        $p = Join-Path $dir $f
        if (Test-Path -LiteralPath $saved) { Copy-Item -LiteralPath $saved -Destination $p -Force }
        elseif (Test-Path -LiteralPath $p) { Remove-Item -LiteralPath $p -Force }
    }
    # Only reached when everything above was put back.
    Remove-Item -LiteralPath $work -Recurse -Force
}

$onlyA = @($a | Where-Object { $b -notcontains $_ })
$onlyB = @($b | Where-Object { $a -notcontains $_ })
$report = @("A: $BuildA ($($a.Count) distinct messages)", "B: $BuildB ($($b.Count) distinct messages)", '')
if (-not $onlyA -and -not $onlyB) { $report += 'Same distinct messages in both runs.' }
else {
    $report += "Only with A ($($onlyA.Count)):"; $report += $onlyA | ForEach-Object { "  $_" }
    $report += "Only with B ($($onlyB.Count)):"; $report += $onlyB | ForEach-Object { "  $_" }
}
Set-Content -LiteralPath (Join-Path $OutDir 'report.txt') -Value $report
Write-Host ($report -join "`n")
Write-Host "results: $OutDir"
exit [int](($onlyA.Count + $onlyB.Count) -gt 0)
