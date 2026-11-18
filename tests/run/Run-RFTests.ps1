<#
.SYNOPSIS
Runs REFramework's in-game test suites (tests/lua, tests/plugin) against one installed game.

.DESCRIPTION
Installs the suites into <GameDir>\reframework, starts the game, waits until every suite has written its results file
(reframework\data\rftest\results\<suite>.json), closes the game, prints a summary, copies the results to -OutDir and
removes everything it installed. REFramework itself (dinput8.dll or the game's loader DLL) must already be installed.

Actions (default Run):
  Run        Install, launch, wait, close, collect, uninstall.
  Install    Only install the suites (for launching the game some other way, e.g. on a remote console session).
  Wait       Wait for results from a game started elsewhere, then collect. Does not start or close the game.
  Collect    Summarize whatever results exist now.
  Uninstall  Remove the suites and restore the config.

Scenarios:
  Normal           The suites as they are.
  MalformedConfig  Also writes a non-numeric value for a numeric key (REFrameworkConfig_MenuKey_V2=abc) and a sentinel
                   (ScriptRunner_GarbageCollectionBudget=777) to the config first. REFramework must start anyway
                   (the suites produce results) and keep the sentinel (regression for kananlib Config::get).

.EXAMPLE
pwsh tests\run\Run-RFTests.ps1 -GameDir 'D:\SteamLibrary\steamapps\common\RESIDENT EVIL 2  BIOHAZARD RE2' -AppId 883710

.EXAMPLE
# Several games, one after another (see tests/README.md for app ids)
'883710:D:\Games\RE2', '601150:D:\Games\Devil May Cry 5' | ForEach-Object {
    $id, $dir = $_ -split ':', 2; pwsh tests\run\Run-RFTests.ps1 -GameDir $dir -AppId $id }
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory)] [string] $GameDir,
    [ValidateSet('Run', 'Install', 'Wait', 'Collect', 'Uninstall')] [string] $Action = 'Run',
    [ValidateSet('Normal', 'MalformedConfig')] [string] $Scenario = 'Normal',
    [string[]] $Suites = @('api', 'regression', 'plugin'),
    # Steam app id: launches through steam://rungameid. Without it the game exe (-Exe, or the largest .exe in GameDir)
    # is started directly.
    [string] $AppId,
    [string] $Exe,
    # Defaults: tests\plugin\out\rftest_plugin.dll and rftest_loadlib.dll (built by tests\plugin\build.bat).
    [string] $PluginDll,
    [string] $LoadlibDll,
    [int] $TimeoutSeconds = 900,
    [string] $OutDir
)

$ErrorActionPreference = 'Stop'
# Windows PowerShell 5.1 leaves $PSScriptRoot empty while binding parameter defaults.
$TestsDir = Split-Path -Parent $PSScriptRoot
if (-not $PluginDll) { $PluginDll = Join-Path $TestsDir 'plugin\out\rftest_plugin.dll' }
if (-not $LoadlibDll) { $LoadlibDll = Join-Path $TestsDir 'plugin\out\rftest_loadlib.dll' }
$GameDir = (Resolve-Path -LiteralPath $GameDir).Path
$Rf = Join-Path $GameDir 'reframework'
$Autorun = Join-Path $Rf 'autorun'
$Data = Join-Path $Rf 'data'
$Plugins = Join-Path $Rf 'plugins'
$Results = Join-Path $Data 'rftest\results'
$LuaDir = Join-Path $TestsDir 'lua'
$Backup = Join-Path $GameDir 'rftest-config-backup.txt'

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
    return $names
}

function Install-Suites {
    if (-not (Test-Path -LiteralPath $Rf)) { throw "$Rf not found: install REFramework into the game first" }
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

    $cfg = Get-ConfigPath
    if (Test-Path -LiteralPath $cfg) { Copy-Item -LiteralPath $cfg -Destination $Backup -Force }
    if ($Scenario -eq 'MalformedConfig') {
        Set-ConfigValues @{ 'REFrameworkConfig_MenuKey_V2' = 'abc'; 'ScriptRunner_GarbageCollectionBudget' = '777.000000' }
    }
    Write-Host "installed suites ($($Suites -join ', '), scenario $Scenario) into $Rf"
}

function Uninstall-Suites {
    Remove-Item -LiteralPath (Join-Path $Autorun 'rftest') -Recurse -Force -ErrorAction SilentlyContinue
    Remove-Item -Path (Join-Path $Autorun 'rftest_*.lua') -Force -ErrorAction SilentlyContinue
    Remove-Item -LiteralPath (Join-Path $Plugins 'rftest_plugin.dll') -Force -ErrorAction SilentlyContinue
    Remove-Item -LiteralPath (Join-Path $Data 'rftest_loadlib.dll') -Force -ErrorAction SilentlyContinue
    Remove-Item -LiteralPath (Join-Path $Data 'rftest') -Recurse -Force -ErrorAction SilentlyContinue
    # Builds without the rooted-path fix write here (see tests/lua/rftest_regression.lua).
    Remove-Item -LiteralPath ((Split-Path -Qualifier $GameDir) + '\rftest_escape') -Recurse -Force -ErrorAction SilentlyContinue
    if (Test-Path -LiteralPath $Backup) {
        Copy-Item -LiteralPath $Backup -Destination (Get-ConfigPath) -Force
        Remove-Item -LiteralPath $Backup -Force
    }
    Write-Host "removed suites from $Rf"
}

function Get-GameProcess {
    Get-Process -ErrorAction SilentlyContinue | Where-Object {
        try { $_.Path -and $_.Path.StartsWith($GameDir, [StringComparison]::OrdinalIgnoreCase) } catch { $false }
    }
}

function Start-Game {
    if ($AppId) {
        Start-Process "steam://rungameid/$AppId"
    } else {
        $exePath = if ($Exe) { Join-Path $GameDir $Exe } else {
            (Get-ChildItem -LiteralPath $GameDir -Filter '*.exe' -File | Sort-Object Length -Descending | Select-Object -First 1).FullName
        }
        Start-Process -FilePath $exePath -WorkingDirectory $GameDir
    }
    $t0 = Get-Date
    while (-not (Get-GameProcess)) {
        if (((Get-Date) - $t0).TotalSeconds -gt 120) { throw 'game process did not start within 120 s' }
        Start-Sleep 2
    }
    Write-Host "game started: $((Get-GameProcess | Select-Object -First 1).Path)"
}

function Stop-Game {
    $procs = @(Get-GameProcess)
    foreach ($p in $procs) { [void]$p.CloseMainWindow() }
    $t0 = Get-Date
    while ((Get-GameProcess) -and ((Get-Date) - $t0).TotalSeconds -lt 30) { Start-Sleep 2 }
    Get-GameProcess | Stop-Process -Force -ErrorAction SilentlyContinue
}

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
        if (Get-GameProcess) { $gameSeen = $true }
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
    if (-not $OutDir) { $OutDir = Join-Path $TestsDir "out\$(Split-Path -Leaf $GameDir)-$(Get-Date -Format yyyyMMdd-HHmmss)" }
    New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
    if (Test-Path -LiteralPath $Results) { Copy-Item -Path (Join-Path $Results '*.json') -Destination $OutDir -Force }
    $log = Join-Path $GameDir 're2_framework_log.txt'
    if (Test-Path -LiteralPath $log) {
        $resets = 0
        $lines = Select-String -LiteralPath $log -Pattern '\[rftest\]|\[error\]|\] Reset!$' | ForEach-Object {
            if ($_.Line.EndsWith('] Reset!')) { $resets++ } else { $_.Line }
        }
        # A swap chain reset every frame means the game is stuck resizing and may not be updating.
        @($lines) + "device resets: $resets" | Set-Content -LiteralPath (Join-Path $OutDir 'log-excerpt.txt')
    }
    $text = @($Summary.Rows | Format-Table -AutoSize | Out-String) + @($Summary.Failed | ForEach-Object { "FAIL $_" })
    Set-Content -LiteralPath (Join-Path $OutDir 'summary.txt') -Value $text
    Write-Host ($text -join "`n")
    Write-Host "results: $OutDir"
}

switch ($Action) {
    'Install' { Install-Suites }
    'Uninstall' { Uninstall-Suites }
    'Collect' { $s = Get-Summary; Save-Results $s; exit [int]($s.Failed.Count -gt 0) }
    'Wait' { [void](Wait-Results); $s = Get-Summary; Save-Results $s; exit [int]($s.Failed.Count -gt 0) }
    'Run' {
        if (Get-GameProcess) { throw 'the game is already running; close it first' }
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
