# Shared helpers for the in-game test tools in tests/run: locating Steam and games, starting and closing games in the
# desktop session (also when the tools run over SSH), installing a REFramework build for a run, and local settings.
# Windows PowerShell 5.1 compatible.

$script:TestsDir = Split-Path -Parent $PSScriptRoot
$script:SteamIdBase = 76561197960265728

function Get-RFTestsDir { $script:TestsDir }

# Machine-specific settings: tests/run/local.json (gitignored; see local.example.json). Missing file = no settings.
function Get-RFTestLocalConfig {
    $path = Join-Path $PSScriptRoot 'local.json'
    if (-not (Test-Path -LiteralPath $path)) { return [pscustomobject]@{} }
    Get-Content -LiteralPath $path -Raw | ConvertFrom-Json
}

function Get-RFTestSetting([string] $Name, $Default = $null) {
    $cfg = Get-RFTestLocalConfig
    if ($cfg.PSObject.Properties.Name -contains $Name -and $cfg.$Name) { return $cfg.$Name }
    $Default
}

function Get-SteamRoot {
    $reg = Get-ItemProperty -Path 'HKCU:\Software\Valve\Steam' -Name SteamPath -ErrorAction SilentlyContinue
    if ($reg -and $reg.SteamPath -and (Test-Path -LiteralPath $reg.SteamPath)) { return (Resolve-Path -LiteralPath $reg.SteamPath).Path }
    foreach ($p in "${env:ProgramFiles(x86)}\Steam", "$env:ProgramFiles\Steam") {
        if ($p -and (Test-Path -LiteralPath (Join-Path $p 'steam.exe'))) { return $p }
    }
    throw 'Steam installation not found'
}

# Every Steam library folder (steamapps\libraryfolders.vdf), the Steam root first.
function Get-SteamLibraries {
    $root = Get-SteamRoot
    $libs = @($root)
    $vdf = Join-Path $root 'steamapps\libraryfolders.vdf'
    if (Test-Path -LiteralPath $vdf) {
        foreach ($m in [regex]::Matches((Get-Content -LiteralPath $vdf -Raw), '"path"\s+"([^"]+)"')) {
            $libs += $m.Groups[1].Value -replace '\\\\', '\'
        }
    }
    $libs | Where-Object { Test-Path -LiteralPath $_ } | Select-Object -Unique
}

function Get-GameCatalog { (Get-Content -LiteralPath (Join-Path $PSScriptRoot 'games.json') -Raw | ConvertFrom-Json).games }

# A game by alias or Steam app id, with its install directory if Steam has it installed (GameDir = $null otherwise).
function Resolve-Game([Parameter(Mandatory)] [string] $Name) {
    $game = Get-GameCatalog | Where-Object { $_.alias -eq $Name -or "$($_.appid)" -eq $Name } | Select-Object -First 1
    if (-not $game) { throw "unknown game '$Name' (see tests/run/games.json)" }
    $dir = $null
    foreach ($lib in Get-SteamLibraries) {
        $manifest = Join-Path $lib "steamapps\appmanifest_$($game.appid).acf"
        if (-not (Test-Path -LiteralPath $manifest)) { continue }
        $installdir = [regex]::Match((Get-Content -LiteralPath $manifest -Raw), '"installdir"\s+"([^"]+)"').Groups[1].Value
        $candidate = Join-Path $lib "steamapps\common\$installdir"
        if (Test-Path -LiteralPath $candidate) { $dir = $candidate; break }
    }
    [pscustomobject]@{ Alias = $game.alias; AppId = "$($game.appid)"; Name = $game.name; Exe = $game.exe; GameDir = $dir }
}

function Get-InstalledGames { Get-GameCatalog | ForEach-Object { Resolve-Game $_.alias } | Where-Object { $_.GameDir } }

# The loginusers.vdf entry of the account Steam logs in with: the one marked MostRecent, else the latest timestamp
# (Steam omits MostRecent when only one account ever logged in). Returns the regex match of the whole entry; Groups[1]
# is the SteamID64.
function Get-SteamLogin([Parameter(Mandatory)] [string] $LoginUsersText) {
    $entries = @([regex]::Matches($LoginUsersText, '"(\d{17})"\s*\{[^}]*\}'))
    if (-not $entries) { throw 'no Steam login found in loginusers.vdf' }
    $recent = $entries | Where-Object { $_.Value -match '"MostRecent"\s+"1"' } | Select-Object -First 1
    if ($recent) { return $recent }
    $entries | Sort-Object { if ($_.Value -match '"timestamp"\s+"(\d+)"') { [long]$Matches[1] } else { 0 } } -Descending | Select-Object -First 1
}

# The Steam account folder (userdata\<account id>) of that login.
function Get-SteamUserDataDir {
    $root = Get-SteamRoot
    $login = Get-SteamLogin (Get-Content -LiteralPath (Join-Path $root 'config\loginusers.vdf') -Raw)
    Join-Path $root "userdata\$([decimal]$login.Groups[1].Value - $script:SteamIdBase)"
}

# True when this process can show windows on the user's desktop. SSH sessions run in session 0 and cannot.
function Test-DesktopSession { [System.Diagnostics.Process]::GetCurrentProcess().SessionId -ne 0 }

# Runs a program on the logged-in user's desktop. From a desktop session it is started directly; from session 0
# (e.g. over SSH) through a one-off scheduled task with an interactive principal, which needs that user to be logged in.
# -Console: a console program that must not open a window. With a helper's console window (or Windows Terminal tab)
# on the desktop, Ghosts 'n Goblins Resurrection stays windowed and resizes in a loop instead of starting (9 of 9
# launches; 0 of 5 without such windows).
function Invoke-InDesktopSession([Parameter(Mandatory)] [string] $FilePath, [string] $Arguments = '', [switch] $Console, [switch] $Wait, [int] $TimeoutSeconds = 600) {
    if (Test-DesktopSession) {
        $start = @{ FilePath = $FilePath; PassThru = $true }
        if ($Arguments) { $start.ArgumentList = $Arguments }
        if ($Console) { $start.WindowStyle = 'Hidden' }
        $p = Start-Process @start
        if ($Wait) { [void]$p.WaitForExit($TimeoutSeconds * 1000) }
        return
    }
    if ($Console) {
        # conhost --headless hosts the program without any window; -WindowStyle Hidden doesn't stop Windows Terminal
        # (the default console host on Windows 11) from showing one.
        $Arguments = "--headless `"$FilePath`" $Arguments"
        $FilePath = Join-Path $env:SystemRoot 'System32\conhost.exe'
    }
    $task = "rftest-$([guid]::NewGuid().ToString('N').Substring(0, 8))"
    $act = if ($Arguments) { New-ScheduledTaskAction -Execute $FilePath -Argument $Arguments } else { New-ScheduledTaskAction -Execute $FilePath }
    $pr = New-ScheduledTaskPrincipal -UserId $env:USERNAME -LogonType Interactive
    $settings = New-ScheduledTaskSettingsSet -AllowStartIfOnBatteries -DontStopIfGoingOnBatteries -ExecutionTimeLimit (New-TimeSpan -Seconds 0)
    Register-ScheduledTask -TaskName $task -Action $act -Principal $pr -Settings $settings -Force | Out-Null
    try {
        Start-ScheduledTask -TaskName $task
        $t0 = Get-Date
        # Give the task time to start before checking its state.
        do { Start-Sleep -Milliseconds 500 } while ((Get-ScheduledTask -TaskName $task).State -eq 'Queued' -and ((Get-Date) - $t0).TotalSeconds -lt 10)
        if ($Wait) {
            while ((Get-ScheduledTask -TaskName $task).State -eq 'Running' -and ((Get-Date) - $t0).TotalSeconds -lt $TimeoutSeconds) { Start-Sleep 1 }
        } else {
            Start-Sleep 2
        }
    } finally {
        Unregister-ScheduledTask -TaskName $task -Confirm:$false -ErrorAction SilentlyContinue
    }
}

# Runs a PowerShell script file on the user's desktop, without a window (see Invoke-InDesktopSession).
function Invoke-ScriptInDesktopSession([Parameter(Mandatory)] [string] $Script, [string] $Arguments = '', [switch] $Wait, [int] $TimeoutSeconds = 600) {
    $ps = Join-Path $env:SystemRoot 'System32\WindowsPowerShell\v1.0\powershell.exe'
    Invoke-InDesktopSession -FilePath $ps -Arguments "-NoProfile -ExecutionPolicy Bypass -File `"$Script`" $Arguments" -Console -Wait:$Wait -TimeoutSeconds $TimeoutSeconds
}

function Get-GameProcess([Parameter(Mandatory)] [string] $GameDir) {
    $prefix = $GameDir.TrimEnd('\') + '\'
    Get-Process -ErrorAction SilentlyContinue | Where-Object {
        try { $_.Path -and $_.Path.StartsWith($prefix, [StringComparison]::OrdinalIgnoreCase) } catch { $false }
    }
}

# Starts a game through Steam (by app id) or its exe, on the user's desktop, and waits for its process.
function Start-RFGame([Parameter(Mandatory)] [string] $GameDir, [string] $AppId, [string] $Exe, [int] $TimeoutSeconds = 300) {
    if ($AppId) {
        Invoke-InDesktopSession -FilePath (Join-Path (Get-SteamRoot) 'steam.exe') -Arguments "-applaunch $AppId"
    } else {
        $exePath = if ($Exe) { Join-Path $GameDir $Exe } else {
            (Get-ChildItem -LiteralPath $GameDir -Filter '*.exe' -File | Sort-Object Length -Descending | Select-Object -First 1).FullName
        }
        Invoke-InDesktopSession -FilePath $exePath
    }
    $t0 = Get-Date
    while (-not (Get-GameProcess $GameDir)) {
        # Steam can take minutes to start a game (updates, cloud sync, first-run scripts).
        if (((Get-Date) - $t0).TotalSeconds -gt $TimeoutSeconds) { throw "game process did not start within $TimeoutSeconds s" }
        Start-Sleep 2
    }
    Get-GameProcess $GameDir | Select-Object -First 1
}

# Asks the game to close (WM_CLOSE from the user's desktop), then kills whatever is left after $GraceSeconds.
# A game that crashed and is still writing REFramework's crash dump (newer than $DumpSince) is not killed: it gets up
# to $DumpWaitSeconds more to exit, since a full dump can take minutes and a killed one is cut short.
function Stop-RFGame([Parameter(Mandatory)] [string] $GameDir, [int] $GraceSeconds = 30, [datetime] $DumpSince = (Get-Date),
    [int] $DumpWaitSeconds = 600) {
    $procs = @(Get-GameProcess $GameDir)
    if (-not $procs) { return }
    if (Test-DesktopSession) {
        foreach ($p in $procs) { [void]$p.CloseMainWindow() }
    } else {
        $taskkill = Join-Path $env:SystemRoot 'System32\taskkill.exe'
        Invoke-InDesktopSession -FilePath $taskkill -Arguments (($procs | ForEach-Object { "/pid $($_.Id)" }) -join ' ') -Console -Wait -TimeoutSeconds 30
    }
    $t0 = Get-Date
    while ((Get-GameProcess $GameDir) -and ((Get-Date) - $t0).TotalSeconds -lt $GraceSeconds) { Start-Sleep 2 }
    while ((Get-GameProcess $GameDir) -and (Get-RFCrashDump -GameDir $GameDir -Since $DumpSince) -and
        ((Get-Date) - $t0).TotalSeconds -lt $GraceSeconds + $DumpWaitSeconds) { Start-Sleep 2 }
    $left = @(Get-GameProcess $GameDir)
    if ($left) {
        Write-Host "game did not close within $([int]((Get-Date) - $t0).TotalSeconds) s; killing it"
        $left | Stop-Process -Force -ErrorAction SilentlyContinue
        Start-Sleep 3
    }
}

function Get-RFCrashDumpPath([Parameter(Mandatory)] [string] $GameDir) { Join-Path $GameDir 'reframework_crash.dmp' }

# The crash dump REFramework's exception handler writes into the game folder, if it was written after $Since (a dump
# left from an earlier session is older); $null otherwise. The file exists from the moment the handler starts writing.
function Get-RFCrashDump([Parameter(Mandatory)] [string] $GameDir, [Parameter(Mandatory)] [datetime] $Since) {
    $dump = Get-Item -LiteralPath (Get-RFCrashDumpPath $GameDir) -ErrorAction SilentlyContinue
    if ($dump -and $dump.LastWriteTime -gt $Since) { $dump }
}

# Puts a REFramework build (dinput8.dll) into the game for a run. An existing dinput8.dll (file or symlink) is moved
# aside and comes back with Restore-RFBuild; if there was none, a marker makes Restore-RFBuild delete the build.
function Install-RFBuild([Parameter(Mandatory)] [string] $GameDir, [Parameter(Mandatory)] [string] $Build) {
    if (-not (Test-Path -LiteralPath $Build)) { throw "build not found: $Build" }
    $dll = Join-Path $GameDir 'dinput8.dll'
    $backup = Join-Path $GameDir 'dinput8.dll.rftest-backup'
    $none = Join-Path $GameDir 'dinput8.dll.rftest-none'
    if (-not (Test-Path -LiteralPath $backup) -and -not (Test-Path -LiteralPath $none)) {
        if (Test-Path -LiteralPath $dll) { Move-Item -LiteralPath $dll -Destination $backup }
        else { Set-Content -LiteralPath $none -Value 'the game had no dinput8.dll before Run-RFTests.ps1 installed a build' }
    }
    if (Test-Path -LiteralPath $dll) { Remove-Item -LiteralPath $dll -Force }
    Copy-Item -LiteralPath $Build -Destination $dll
}

function Restore-RFBuild([Parameter(Mandatory)] [string] $GameDir) {
    $dll = Join-Path $GameDir 'dinput8.dll'
    $backup = Join-Path $GameDir 'dinput8.dll.rftest-backup'
    $none = Join-Path $GameDir 'dinput8.dll.rftest-none'
    if (Test-Path -LiteralPath $backup) {
        if (Test-Path -LiteralPath $dll) { Remove-Item -LiteralPath $dll -Force }
        Move-Item -LiteralPath $backup -Destination $dll
    } elseif (Test-Path -LiteralPath $none) {
        if (Test-Path -LiteralPath $dll) { Remove-Item -LiteralPath $dll -Force }
        Remove-Item -LiteralPath $none -Force
    }
}

function Get-RFLogPath([Parameter(Mandatory)] [string] $GameDir) { Join-Path $GameDir 're2_framework_log.txt' }

# Count of swap chain resets in the REFramework log. Thousands mean the game is stuck resizing (see tests/README.md).
function Get-RFResetCount([Parameter(Mandatory)] [string] $GameDir) {
    $log = Get-RFLogPath $GameDir
    if (-not (Test-Path -LiteralPath $log)) { return 0 }
    @(Select-String -LiteralPath $log -Pattern '\] Reset!$').Count
}

Export-ModuleMember -Function *
