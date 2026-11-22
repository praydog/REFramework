<#
.SYNOPSIS
Runs the in-game test suites (Run-RFTests.ps1) on every installed game, one after another, and writes one summary.

.DESCRIPTION
Games come from games.json; by default every one Steam has installed. Each game's results land in
<OutDir>\<alias>\ (see Run-RFTests.ps1), and the combined results in <OutDir>\summary.md and summary.json.
Exit code 0 means no suite failed and no game crashed (see Run-RFTests.ps1). Skips don't fail the run, but every one
is listed with its reason in summary.md: a skip is lost coverage, and a skip that wasn't there before is a regression
until shown otherwise.

.EXAMPLE
powershell -ExecutionPolicy Bypass -File tests\run\Run-AllGames.ps1 -Build C:\builds\dinput8.dll

.EXAMPLE
powershell -ExecutionPolicy Bypass -File tests\run\Run-AllGames.ps1 -Games re2,mhwilds -Suites api,regression,plugin,csharp
#>
[CmdletBinding()]
param(
    # Aliases or app ids from games.json. Default: every installed game.
    [string[]] $Games,
    [string[]] $Suites = @('api', 'regression', 'plugin'),
    [ValidateSet('Normal', 'MalformedConfig')] [string] $Scenario = 'Normal',
    # REFramework build (dinput8.dll) to test. Default: local.json "build", else what each game already has.
    [string] $Build,
    [string] $CSharpBin,
    [int] $TimeoutSeconds = 900,
    # Default: <local.json "outDir" or tests\out>\all-<time>.
    [string] $OutDir
)

$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'RFTest.psm1') -Force
$Suites = @($Suites | ForEach-Object { $_ -split ',' } | Where-Object { $_ })
$Games = @($Games | ForEach-Object { $_ -split ',' } | Where-Object { $_ })
if (-not $PSBoundParameters.ContainsKey('Build')) { $Build = Get-RFTestSetting 'build' }
if (-not $CSharpBin) { $CSharpBin = Get-RFTestSetting 'csharpBin' }
if (-not $OutDir) { $OutDir = Join-Path (Get-RFTestSetting 'outDir' (Join-Path (Get-RFTestsDir) 'out')) "all-$(Get-Date -Format yyyyMMdd-HHmmss)" }
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null

$targets = if ($Games) { $Games | ForEach-Object { Resolve-Game $_ } } else { Get-InstalledGames }
$runner = Join-Path $PSScriptRoot 'Run-RFTests.ps1'
$ps = Join-Path $env:SystemRoot 'System32\WindowsPowerShell\v1.0\powershell.exe'
$all = @()

foreach ($g in $targets) {
    Write-Host "=== $($g.Alias) ($($g.Name))"
    $gameOut = Join-Path $OutDir $g.Alias
    if (-not $g.GameDir) {
        Write-Warning "$($g.Alias) is not installed; skipped"
        $all += [pscustomobject]@{ game = $g.Alias; installed = $false; suites = @(); failed = @('not installed'); deviceResets = 0 }
        continue
    }
    # A separate process per game, so one game's failure (or exit) can't take down the whole run.
    $runArgs = @('-NoProfile', '-ExecutionPolicy', 'Bypass', '-File', $runner, '-Game', $g.Alias, '-Suites', ($Suites -join ','),
        '-Scenario', $Scenario, '-TimeoutSeconds', $TimeoutSeconds, '-OutDir', $gameOut)
    if ($Build) { $runArgs += @('-Build', $Build) }
    if ($CSharpBin) { $runArgs += @('-CSharpBin', $CSharpBin) }
    & $ps @runArgs
    $summary = Join-Path $gameOut 'summary.json'
    if (Test-Path -LiteralPath $summary) {
        $s = Get-Content -LiteralPath $summary -Raw | ConvertFrom-Json
        $all += [pscustomobject]@{ game = $g.Alias; installed = $true; suites = @($s.suites); failed = @($s.failed); skipped = @($s.skipped); crash = $s.crash; deviceResets = $s.deviceResets }
    } else {
        $all += [pscustomobject]@{ game = $g.Alias; installed = $true; suites = @(); failed = @('runner produced no summary'); deviceResets = 0 }
    }
}

# Combined summary: one row per game, one column per suite (pass/fail/skip).
$suiteNames = @($all | ForEach-Object { $_.suites } | ForEach-Object { $_.Suite } | Select-Object -Unique)
$md = @(
    ("| Game | " + ($suiteNames -join ' | ') + " | Resets | Crash |"),
    ("|---|" + (($suiteNames | ForEach-Object { '---' }) -join '|') + "|---|---|")
)
foreach ($r in $all) {
    $cells = foreach ($n in $suiteNames) {
        $s = $r.suites | Where-Object { $_.Suite -eq $n } | Select-Object -First 1
        if ($s) { "$($s.Pass)/$($s.Fail)/$($s.Skip)" } else { '-' }
    }
    $crashCell = if ($r.crash) { ($r.crash -split ':')[0] } else { '-' }
    $md += "| $($r.game) | " + ($cells -join ' | ') + " | $($r.deviceResets) | $crashCell |"
}
$md += ''
$md += 'Cells are pass/fail/skip. Thousands of device resets mean the game was stuck resizing (see tests/README.md).'
$md += 'Crash: REFramework wrote reframework_crash.dmp during the run (also while the game was closing); it fails the game.'
$failures = @($all | Where-Object { $_.failed.Count -gt 0 })
if ($failures) {
    $md += ''
    $md += '## Failures'
    foreach ($r in $failures) { foreach ($f in $r.failed) { $md += "- $($r.game): $f" } }
}
$skips = @($all | Where-Object { $_.skipped.Count -gt 0 })
if ($skips) {
    $md += ''
    $md += '## Skips'
    foreach ($r in $skips) { foreach ($f in $r.skipped) { $md += "- $($r.game): $f" } }
}
Set-Content -LiteralPath (Join-Path $OutDir 'summary.md') -Value $md
$all | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath (Join-Path $OutDir 'summary.json')
Write-Host ($md -join "`n")
Write-Host "results: $OutDir"
exit [int]($failures.Count -gt 0)
