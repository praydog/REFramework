<#
.SYNOPSIS
Runs REFramework's in-game test suites (tests/lua, tests/plugin, tests/csharp) against one installed game.

.DESCRIPTION
Installs the suites into <game>\reframework, starts the game, waits until every suite has written its results file
(reframework\data\rftest\results\<suite>.json), closes the game, prints a summary, copies the results to -OutDir and
removes everything it installed.

The game is picked with -Game (an alias or Steam app id from games.json; the install folder comes from Steam's own
manifests) or with -GameDir. REFramework comes from -Build (a dinput8.dll, installed for the run and removed
afterwards; the game's own dinput8.dll is put back), else from local.json's "build", else whatever the game already has.

The game is started on the logged-in user's desktop, so this also works over SSH (session 0): there the game is
started and closed through a one-off scheduled task, which needs that user logged in.

Actions (default Run):
  Run        Install, launch, wait, close, collect, uninstall.
  Install    Only install the suites (and -Build), for launching the game some other way.
  Wait       Wait for results from a game started elsewhere, then collect. Does not start or close the game.
  Collect    Summarize whatever results exist now.
  Uninstall  Remove the suites, restore the config and the game's own dinput8.dll.

Scenarios:
  Normal           The suites as they are.
  MalformedConfig  Also writes a non-numeric value for a numeric key (REFrameworkConfig_MenuKey_V2=abc) and a sentinel
                   (ScriptRunner_GarbageCollectionBudget=777) to the config first. REFramework must start anyway
                   (the suites produce results) and keep the sentinel (regression for kananlib Config::get).

.EXAMPLE
powershell -ExecutionPolicy Bypass -File tests\run\Run-RFTests.ps1 -Game re2 -Build C:\builds\dinput8.dll

.EXAMPLE
powershell -ExecutionPolicy Bypass -File tests\run\Run-RFTests.ps1 -Game mhwilds -Suites api,regression,plugin,csharp -CSharpBin C:\src\REFramework\csharp-api\build\bin

.EXAMPLE
powershell -ExecutionPolicy Bypass -File tests\run\Run-RFTests.ps1 -GameDir 'D:\Games\RE2' -AppId 883710
#>
[CmdletBinding()]
param(
    # Alias or Steam app id from tests/run/games.json.
    [string] $Game,
    # The game's install folder, instead of -Game.
    [string] $GameDir,
    [ValidateSet('Run', 'Install', 'Wait', 'Collect', 'Uninstall')] [string] $Action = 'Run',
    [ValidateSet('Normal', 'MalformedConfig')] [string] $Scenario = 'Normal',
    [string[]] $Suites = @('api', 'regression', 'plugin'),
    # REFramework build (dinput8.dll) to install for the run. Default: local.json "build", else the game's own.
    [string] $Build,
    # Steam app id: launches through Steam. Without it the game exe (-Exe, or the largest .exe in GameDir) is started
    # directly. Set automatically by -Game.
    [string] $AppId,
    [string] $Exe,
    # Defaults: tests\plugin\out\rftest_plugin.dll and rftest_loadlib.dll (built by tests\plugin\build.bat).
    [string] $PluginDll,
    [string] $LoadlibDll,
    # The C# API build output (csharp-api\build\bin) for the csharp suite. Its files are installed only if the game does
    # not already have REFramework.NET, and removed again afterwards. Default: local.json "csharpBin".
    [string] $CSharpBin,
    [int] $TimeoutSeconds = 900,
    # Where results are copied. Default: <local.json "outDir" or tests\out>\<game>-<time>.
    [string] $OutDir
)

$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'RFTest.psm1') -Force
# powershell -File passes "api,csharp" as one string.
$Suites = @($Suites | ForEach-Object { $_ -split ',' } | Where-Object { $_ })
$TestsDir = Get-RFTestsDir
if (-not $PluginDll) { $PluginDll = Join-Path $TestsDir 'plugin\out\rftest_plugin.dll' }
if (-not $LoadlibDll) { $LoadlibDll = Join-Path $TestsDir 'plugin\out\rftest_loadlib.dll' }
if (-not $CSharpBin) { $CSharpBin = Get-RFTestSetting 'csharpBin' }
if (-not $PSBoundParameters.ContainsKey('Build')) { $Build = Get-RFTestSetting 'build' }

if ($Game) {
    $g = Resolve-Game $Game
    if (-not $g.GameDir) { throw "$($g.Name) ($($g.AppId)) is not installed in any Steam library" }
    $GameDir = $g.GameDir
    if (-not $AppId) { $AppId = $g.AppId }
    if (-not $Exe) { $Exe = $g.Exe }
}
if (-not $GameDir) { throw 'pass -Game <alias or app id> or -GameDir <install folder>' }
$GameDir = (Resolve-Path -LiteralPath $GameDir).Path
$Rf = Join-Path $GameDir 'reframework'
$Autorun = Join-Path $Rf 'autorun'
$Data = Join-Path $Rf 'data'
$Plugins = Join-Path $Rf 'plugins'
$Results = Join-Path $Data 'rftest\results'
$LuaDir = Join-Path $TestsDir 'lua'
# What the game had before Install: the list of paths under reframework\ and copies of the files REFramework writes
# next to the game. Uninstall removes everything else under reframework\ and puts the files back.
$State = Join-Path $GameDir 'rftest-state'
$RootFiles = 're2_fw_config.txt', 're2_framework_log.txt', 're2_framework_log.prev.txt', 'ref_ui.ini',
    'reframework_accessed_files.txt', 'reframework_loose_files.txt', 'reframework_faulty_files.txt', 'reframework_crash.dmp'
$CSharpPluginFiles = 'REFramework.NET.dll', 'REFramework.NET.runtimeconfig.json', 'Ijwhost.dll'
$CSharpDependencyFiles = 'AssemblyGenerator.dll', 'REFCoreDeps.dll', 'Microsoft.CodeAnalysis.CSharp.dll', 'Microsoft.CodeAnalysis.dll',
    'Hexa.NET.ImGui.dll', 'HexaGen.Runtime.dll'

function Get-RootFiles { @($RootFiles) + (Split-Path -Leaf (Get-ConfigPath)) | Select-Object -Unique }

function Get-ConfigPath {
    $cfg = Get-ChildItem -LiteralPath $GameDir -Filter '*_fw_config.txt' -File -ErrorAction SilentlyContinue | Select-Object -First 1
    if ($cfg) { return $cfg.FullName }
    return Join-Path $GameDir 're2_fw_config.txt'
}

function Set-ConfigValues([hashtable] $Values) {
    $path = Get-ConfigPath
    $lines = @(if (Test-Path -LiteralPath $path) { Get-Content -LiteralPath $path })
    foreach ($k in $Values.Keys) {
        $found = $false
        $lines = @($lines | ForEach-Object { if ($_ -like "$k=*") { $found = $true; "$k=$($Values[$k])" } else { $_ } })
        if (-not $found) { $lines += "$k=$($Values[$k])" }
    }
    Set-Content -LiteralPath $path -Value $lines
}

function Get-ExpectedSuites {
    $names = @()
    if ($Suites -contains 'api') { $names += 'api' }
    if ($Suites -contains 'regression') { $names += 'regression' }
    if ($Suites -contains 'plugin') { $names += 'plugin_api', 'plugin_regression' }
    if ($Suites -contains 'csharp') { $names += 'csharp' }
    return $names
}

function Install-Suites {
    if (Test-Path -LiteralPath $State) { throw "$State exists: a previous run was not uninstalled; run -Action Uninstall first" }
    New-Item -ItemType Directory -Path $State | Out-Null
    @(if (Test-Path -LiteralPath $Rf) { $Rf; Get-ChildItem -LiteralPath $Rf -Recurse -Force | ForEach-Object { $_.FullName } }) |
        Set-Content -LiteralPath (Join-Path $State 'reframework-before.txt')
    foreach ($f in Get-RootFiles) {
        $p = Join-Path $GameDir $f
        if (Test-Path -LiteralPath $p) { Copy-Item -LiteralPath $p -Destination $State }
    }
    if ($Build) {
        Install-RFBuild -GameDir $GameDir -Build $Build
        Write-Host "installed build $Build"
    } elseif (-not (Test-Path -LiteralPath (Join-Path $GameDir 'dinput8.dll'))) {
        Write-Warning "no dinput8.dll in $GameDir and no -Build: REFramework will only load if the game has another loader"
    }
    New-Item -ItemType Directory -Force -Path $Autorun, $Data, $Plugins | Out-Null
    Remove-Item -LiteralPath (Join-Path $Data 'rftest') -Recurse -Force -ErrorAction SilentlyContinue

    Copy-Item -LiteralPath (Join-Path $LuaDir 'rftest') -Destination $Autorun -Recurse -Force
    foreach ($s in 'api', 'regression') {
        if ($Suites -contains $s) { Copy-Item -LiteralPath (Join-Path $LuaDir "rftest_$s.lua") -Destination $Autorun -Force }
    }
    if ($Suites -contains 'regression') {
        if (Test-Path -LiteralPath $LoadlibDll) { Copy-Item -LiteralPath $LoadlibDll -Destination (Join-Path $Data 'rftest_loadlib.dll') -Force }
        else { Write-Warning "$LoadlibDll not found (build tests\plugin\build.bat); the package.loadlib test will SKIP" }
    }
    if ($Suites -contains 'plugin') {
        if (-not (Test-Path -LiteralPath $PluginDll)) { throw "$PluginDll not found: build it with tests\plugin\build.bat" }
        Copy-Item -LiteralPath $PluginDll -Destination (Join-Path $Plugins 'rftest_plugin.dll') -Force
    }
    if ($Suites -contains 'csharp') {
        $source = Join-Path $Plugins 'source'
        New-Item -ItemType Directory -Force -Path $source | Out-Null
        if (-not (Test-Path -LiteralPath (Join-Path $Plugins 'REFramework.NET.dll'))) {
            if (-not $CSharpBin -or -not (Test-Path -LiteralPath (Join-Path $CSharpBin 'REFramework.NET.dll'))) {
                throw "the csharp suite needs REFramework.NET: pass -CSharpBin <csharp-api\build\bin> or install it into the game"
            }
            $deps = Join-Path $Plugins 'managed\dependencies'
            New-Item -ItemType Directory -Force -Path $deps | Out-Null
            foreach ($f in $CSharpPluginFiles) { Copy-Item -LiteralPath (Join-Path $CSharpBin $f) -Destination $Plugins -Force }
            foreach ($f in $CSharpDependencyFiles) { Copy-Item -LiteralPath (Join-Path $CSharpBin $f) -Destination $deps -Force }
        }
        Copy-Item -LiteralPath (Join-Path $TestsDir 'csharp\rftest_csharp.cs') -Destination $source -Force
    }

    if ($Scenario -eq 'MalformedConfig') {
        Set-ConfigValues @{ 'REFrameworkConfig_MenuKey_V2' = 'abc'; 'ScriptRunner_GarbageCollectionBudget' = '777.000000' }
    }
    Write-Host "installed suites ($($Suites -join ', '), scenario $Scenario) into $Rf"
}

function Uninstall-Suites {
    if (-not (Test-Path -LiteralPath $State)) { Write-Warning "nothing to uninstall ($State not found)"; return }
    # Everything under reframework\ that wasn't there before: the suites, REFramework.NET, generated assemblies, fonts...
    $before = @{}
    foreach ($p in @(Get-Content -LiteralPath (Join-Path $State 'reframework-before.txt'))) { if ($p) { $before[$p] = $true } }
    if (Test-Path -LiteralPath $Rf) {
        Get-ChildItem -LiteralPath $Rf -Recurse -Force | Sort-Object { $_.FullName.Length } -Descending | Where-Object { -not $before[$_.FullName] } |
            ForEach-Object { Remove-Item -LiteralPath $_.FullName -Recurse -Force -ErrorAction SilentlyContinue }
        if (-not $before[$Rf]) { Remove-Item -LiteralPath $Rf -Recurse -Force -ErrorAction SilentlyContinue }
    }
    foreach ($f in Get-RootFiles) {
        $saved = Join-Path $State $f
        $p = Join-Path $GameDir $f
        if (Test-Path -LiteralPath $saved) { Copy-Item -LiteralPath $saved -Destination $p -Force }
        elseif (Test-Path -LiteralPath $p) { Remove-Item -LiteralPath $p -Force }
    }
    # Builds without the rooted-path fix write here (see tests/lua/rftest_regression.lua).
    Remove-Item -LiteralPath ((Split-Path -Qualifier $GameDir) + '\rftest_escape') -Recurse -Force -ErrorAction SilentlyContinue
    Restore-RFBuild -GameDir $GameDir
    Remove-Item -LiteralPath $State -Recurse -Force
    Write-Host "removed suites from $Rf"
}

function Get-GameProcesses { Get-GameProcess -GameDir $GameDir }

function Start-Game {
    $p = Start-RFGame -GameDir $GameDir -AppId $AppId -Exe $Exe
    Write-Host "game started: $($p.Path)"
}

function Stop-Game { Stop-RFGame -GameDir $GameDir }

function Test-SuiteComplete([string] $Name) {
    $path = Join-Path $Results "$Name.json"
    if (-not (Test-Path -LiteralPath $path)) { return $false }
    try { $r = Get-Content -LiteralPath $path -Raw | ConvertFrom-Json } catch { return $false }
    # The plugin pre-records its deadlock tests as "pending" before running them.
    return -not ($r.tests | Where-Object { $_.detail -like 'pending:*' })
}

function Wait-Results {
    $expected = Get-ExpectedSuites
    $t0 = Get-Date
    $lastChange = Get-Date
    $seen = 0
    $gameSeen = $false
    while (((Get-Date) - $t0).TotalSeconds -lt $TimeoutSeconds) {
        $done = @($expected | Where-Object { Test-SuiteComplete $_ })
        if ($done.Count -eq $expected.Count) { return $true }
        if ($done.Count -ne $seen) { $seen = $done.Count; $lastChange = Get-Date; Write-Host "results: $($done -join ', ')" }
        if (Get-GameProcesses) { $gameSeen = $true }
        elseif ($gameSeen -or $Action -eq 'Run') { Write-Warning 'game exited before all results were written (crash?)'; return $false }
        # REFramework gave up initializing its mods (no Lua, no plugins): nothing will be written.
        $log = Get-Item -LiteralPath (Join-Path $GameDir 're2_framework_log.txt') -ErrorAction SilentlyContinue
        if ($gameSeen -and $log -and $log.LastWriteTime -gt $t0 -and
            (Select-String -LiteralPath $log.FullName -Pattern 'Initialization of mods failed' -Quiet)) {
            Write-Warning "REFramework initialization failed: $((Select-String -LiteralPath $log.FullName -Pattern 'Initialization of mods failed').Line)"
            return $false
        }
        # Every suite wrote results but one stays pending: a deadlock test hung the game.
        $present = @($expected | Where-Object { Test-Path -LiteralPath (Join-Path $Results "$_.json") })
        if ($present.Count -eq $expected.Count -and ((Get-Date) - $lastChange).TotalSeconds -gt 120) {
            Write-Warning 'a suite stayed pending for 120 s (game hung?)'
            return $false
        }
        Start-Sleep 5
    }
    Write-Warning "timed out after $TimeoutSeconds s"
    return $false
}

function Get-Summary {
    $rows = @()
    $failed = @()
    foreach ($name in Get-ExpectedSuites) {
        $path = Join-Path $Results "$name.json"
        if (-not (Test-Path -LiteralPath $path)) {
            $rows += [pscustomobject]@{ Suite = $name; Pass = 0; Fail = 1; Skip = 0; Note = 'no results (REFramework or the suite did not start)' }
            $failed += "${name}: no results file"
            continue
        }
        $r = Get-Content -LiteralPath $path -Raw | ConvertFrom-Json
        $fail = @($r.tests | Where-Object { $_.status -eq 'FAIL' })
        $rows += [pscustomobject]@{
            Suite = $name; Pass = @($r.tests | Where-Object { $_.status -eq 'PASS' }).Count; Fail = $fail.Count
            Skip = @($r.tests | Where-Object { $_.status -eq 'SKIP' }).Count; Note = $r.game
        }
        foreach ($t in $fail) { $failed += "${name}: $($t.name) -- $($t.detail)" }
    }

    if ($Scenario -eq 'MalformedConfig') {
        $cfg = Get-ConfigPath
        $budget = Select-String -LiteralPath $cfg -Pattern '^ScriptRunner_GarbageCollectionBudget=(.*)$' | Select-Object -First 1
        $ok = $budget -and ([double]$budget.Matches[0].Groups[1].Value -eq 777)
        $rows += [pscustomobject]@{ Suite = 'config'; Pass = [int]$ok; Fail = [int](-not $ok); Skip = 0; Note = "GarbageCollectionBudget: $($budget.Line)" }
        if (-not $ok) { $failed += "config: malformed REFrameworkConfig_MenuKey_V2 lost later settings ($($budget.Line))" }
    }
    return [pscustomobject]@{ Rows = $rows; Failed = $failed }
}

function Save-Results($Summary) {
    if (-not $OutDir) {
        $root = Get-RFTestSetting 'outDir' (Join-Path $TestsDir 'out')
        $name = if ($Game) { $Game } else { Split-Path -Leaf $GameDir }
        $script:OutDir = Join-Path $root "$name-$(Get-Date -Format yyyyMMdd-HHmmss)"
    }
    New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
    if (Test-Path -LiteralPath $Results) { Copy-Item -Path (Join-Path $Results '*.json') -Destination $OutDir -Force }
    $log = Get-RFLogPath $GameDir
    $resets = 0
    if (Test-Path -LiteralPath $log) {
        $lines = Select-String -LiteralPath $log -Pattern '\[rftest\]|\[error\]|\] Reset!$' | ForEach-Object {
            if ($_.Line.EndsWith('] Reset!')) { $resets++ } else { $_.Line }
        }
        # A swap chain reset every frame means the game is stuck resizing and may not be updating.
        @($lines) + "device resets: $resets" | Set-Content -LiteralPath (Join-Path $OutDir 'log-excerpt.txt')
    }
    $text = @($Summary.Rows | Format-Table -AutoSize | Out-String) + @($Summary.Failed | ForEach-Object { "FAIL $_" })
    Set-Content -LiteralPath (Join-Path $OutDir 'summary.txt') -Value $text
    # Machine-readable summary for Run-AllGames.ps1 and other tools.
    [pscustomobject]@{
        game = if ($Game) { $Game } else { Split-Path -Leaf $GameDir }; scenario = $Scenario; build = $Build
        suites = $Summary.Rows; failed = @($Summary.Failed); deviceResets = $resets
    } | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $OutDir 'summary.json')
    Write-Host ($text -join "`n")
    Write-Host "results: $OutDir"
}

switch ($Action) {
    'Install' { Install-Suites }
    'Uninstall' { Uninstall-Suites }
    'Collect' { $s = Get-Summary; Save-Results $s; exit [int]($s.Failed.Count -gt 0) }
    'Wait' { [void](Wait-Results); $s = Get-Summary; Save-Results $s; exit [int]($s.Failed.Count -gt 0) }
    'Run' {
        if (Get-GameProcesses) { throw 'the game is already running; close it first' }
        Install-Suites
        try {
            Start-Game
            [void](Wait-Results)
            Stop-Game
            $s = Get-Summary
            Save-Results $s
        } finally {
            Stop-Game
            Uninstall-Suites
        }
        exit [int]($s.Failed.Count -gt 0)
    }
}
