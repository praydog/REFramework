<#
.SYNOPSIS
Starts a game repeatedly until REFramework hangs during startup, then takes a full memory dump of the hung game.

.DESCRIPTION
For intermittent startup hangs. Each run starts the game and watches the REFramework log:
- "Lua state initialized" appears: a good start; the game is closed and the next run begins.
- the log stops growing for -IdleSeconds before that: a hang. The game is dumped with procdump (full dump) into
  -DumpDir together with the log, and the loop stops.
- the game exits on its own: recorded, and the loop continues.

Needs Sysinternals procdump: -ProcDump, local.json "procdump", or procdump64.exe on PATH.

.EXAMPLE
powershell -ExecutionPolicy Bypass -File tests\run\Find-StartupHang.ps1 -Game dd2 -Runs 30 -Build C:\builds\dinput8.dll -DumpDir D:\dumps
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory)] [string] $Game,
    [int] $Runs = 10,
    # REFramework build (dinput8.dll) to test. Default: local.json "build", else what the game already has.
    [string] $Build,
    [int] $IdleSeconds = 40,
    [int] $TimeoutSeconds = 300,
    [string] $DumpDir,
    [string] $ProcDump
)

$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'RFTest.psm1') -Force
$g = Resolve-Game $Game
if (-not $g.GameDir) { throw "$($g.Name) is not installed" }
if (-not $PSBoundParameters.ContainsKey('Build')) { $Build = Get-RFTestSetting 'build' }
if (-not $ProcDump) { $ProcDump = Get-RFTestSetting 'procdump' }
if (-not $ProcDump) { $ProcDump = (Get-Command procdump64.exe, procdump.exe -ErrorAction SilentlyContinue | Select-Object -First 1).Source }
if (-not $ProcDump -or -not (Test-Path -LiteralPath $ProcDump)) { throw 'procdump not found: pass -ProcDump, set "procdump" in local.json or put procdump64.exe on PATH' }
if (-not $DumpDir) { $DumpDir = Join-Path (Get-RFTestSetting 'outDir' (Join-Path (Get-RFTestsDir) 'out')) "hang-$($g.Alias)" }
New-Item -ItemType Directory -Force -Path $DumpDir | Out-Null
$dir = $g.GameDir
$log = Get-RFLogPath $dir
if (Get-GameProcess $dir) { throw 'the game is already running; close it first' }

if ($Build) { Install-RFBuild -GameDir $dir -Build $Build }
$results = @()
try {
    for ($run = 1; $run -le $Runs; $run++) {
        $t0 = Get-Date
        $proc = Start-RFGame -GameDir $dir -AppId $g.AppId -Exe $g.Exe
        $verdict = 'timeout'
        while (((Get-Date) - $t0).TotalSeconds -lt $TimeoutSeconds) {
            Start-Sleep 5
            $p = Get-GameProcess $dir | Select-Object -First 1
            if (-not $p) { $verdict = 'exited'; break }
            $item = Get-Item -LiteralPath $log -ErrorAction SilentlyContinue
            if ($item -and $item.LastWriteTime -gt $t0 -and (Select-String -LiteralPath $log -Pattern 'Lua state initialized' -Quiet)) { $verdict = 'ok'; break }
            $since = if ($item -and $item.LastWriteTime -gt $t0) { $item.LastWriteTime } else { $t0 }
            $idle = ((Get-Date) - $since).TotalSeconds
            if ($idle -gt $IdleSeconds) {
                $stamp = "$($g.Alias)-run$run-$(Get-Date -Format yyyyMMdd-HHmmss)"
                $dump = Join-Path $DumpDir "$stamp.dmp"
                & $ProcDump -accepteula -ma $p.Id $dump | Out-Null
                if (Test-Path -LiteralPath $log) { Copy-Item -LiteralPath $log -Destination (Join-Path $DumpDir "$stamp.log") }
                $verdict = "hang (log idle $([int]$idle) s), dump $dump"
                break
            }
        }
        Write-Host "run ${run}: $verdict"
        $results += [pscustomobject]@{ run = $run; result = $verdict }
        Stop-RFGame -GameDir $dir
        if ($verdict -like 'hang*') { break }
    }
} finally {
    Stop-RFGame -GameDir $dir
    if ($Build) { Restore-RFBuild -GameDir $dir }
}
$results | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $DumpDir 'runs.json')
$hangs = @($results | Where-Object { $_.result -like 'hang*' })
Write-Host "$($results.Count) runs, $($hangs.Count) hang(s); results in $DumpDir"
exit [int]($hangs.Count -gt 0)
