<#
.SYNOPSIS
Switches Steam on a dedicated test machine between offline mode (for test runs) and online mode (for game updates),
without letting test runs change the account's Steam Cloud saves.

.DESCRIPTION
Why: a test machine signed in to your everyday Steam account receives your games' cloud saves, including option files.
They can break test runs (e.g. a 4K resolution from another PC makes some games resize in a loop on a 1080p display),
and anything a test run writes would be uploaded back. Steam's cloud switches are stored in the account's roaming
settings, so they can't be turned off for one machine. Offline mode stops all syncing on this machine.

-Mode Offline:
  1. Closes Steam.
  2. Backs up the userdata folders (cloud saves + remotecache.vdf) of the games in games.json to -BackupDir.
  3. Removes the local cloud saves of the -ResetSaves games, so they start with default settings.
  4. Sets Steam's offline-mode flag for the last-logged-in account and starts Steam on the user's desktop.
  Refuses to run while Steam is already in offline mode, so the backup always holds the last synced state.

-Mode Online:
  1. Closes Steam.
  2. Restores the backed-up userdata folders, so nothing written while offline is uploaded.
  3. Clears the offline-mode flag and starts Steam.

Use only on a machine dedicated to testing. -WhatIf shows what would happen.

.EXAMPLE
powershell -ExecutionPolicy Bypass -File tests\run\Set-SteamOffline.ps1 -Mode Offline -ResetSaves re7,gngr

.EXAMPLE
powershell -ExecutionPolicy Bypass -File tests\run\Set-SteamOffline.ps1 -Mode Online
#>
[CmdletBinding(SupportsShouldProcess)]
param(
    [Parameter(Mandatory)] [ValidateSet('Offline', 'Online')] [string] $Mode,
    # Aliases whose local cloud saves are removed when going offline (they start with default settings).
    [string[]] $ResetSaves = @(),
    # Default: local.json "steamCloudBackupDir", else %LOCALAPPDATA%\rftest\steam-cloud-backup.
    [string] $BackupDir
)

$ErrorActionPreference = 'Stop'
Import-Module (Join-Path $PSScriptRoot 'RFTest.psm1') -Force
$ResetSaves = @($ResetSaves | ForEach-Object { $_ -split ',' } | Where-Object { $_ })
if (-not $BackupDir) { $BackupDir = Get-RFTestSetting 'steamCloudBackupDir' (Join-Path $env:LOCALAPPDATA 'rftest\steam-cloud-backup') }
$steam = Get-SteamRoot
$steamExe = Join-Path $steam 'steam.exe'
$users = Join-Path $steam 'config\loginusers.vdf'
$userdata = Get-SteamUserDataDir
$appids = @(Get-GameCatalog | ForEach-Object { "$($_.appid)" })

function Test-OfflineFlag { (Get-SteamLogin ([IO.File]::ReadAllText($users))).Value -match '"WantsOfflineMode"\s+"1"' }

# Sets the offline flags of the login whose userdata folder this script uses.
function Set-OfflineFlag([bool] $Offline) {
    $value = if ($Offline) { '1' } else { '0' }
    $text = [IO.File]::ReadAllText($users)
    $login = Get-SteamLogin $text
    $block = $login.Value
    foreach ($key in @('WantsOfflineMode') + @(if ($Offline) { 'SkipOfflineModeWarning' })) {
        if ($block -match "`"$key`"\s+`"\d`"") { $block = $block -replace "(`"$key`"\s+)`"\d`"", "`$1`"$value`"" }
        else { $block = $block -replace '(\r?\n)(\s*)\}$', "`$1`$2`t`"$key`"`t`t`"$value`"`$1`$2}" }
    }
    [IO.File]::WriteAllText($users, $text.Substring(0, $login.Index) + $block + $text.Substring($login.Index + $login.Length))
}

function Stop-Steam {
    if (-not (Get-Process steam -ErrorAction SilentlyContinue)) { return }
    Invoke-InDesktopSession -FilePath $steamExe -Arguments '-shutdown'
    for ($i = 0; $i -lt 60 -and (Get-Process steam -ErrorAction SilentlyContinue); $i++) { Start-Sleep 1 }
    if (Get-Process steam -ErrorAction SilentlyContinue) { throw 'Steam did not shut down within 60 s' }
}

if ($Mode -eq 'Offline') {
    if (Test-OfflineFlag) { throw 'Steam is already in offline mode; run -Mode Online first (that restores the backup)' }
    if ($PSCmdlet.ShouldProcess($steam, 'close Steam')) { Stop-Steam }
    if ($PSCmdlet.ShouldProcess($BackupDir, "back up the userdata folders of $($appids.Count) games")) {
        if (Test-Path -LiteralPath $BackupDir) { Remove-Item -Recurse -Force $BackupDir }
        New-Item -ItemType Directory -Force -Path $BackupDir | Out-Null
        foreach ($id in $appids) {
            $src = Join-Path $userdata $id
            if (Test-Path -LiteralPath $src) { Copy-Item -LiteralPath $src -Destination (Join-Path $BackupDir $id) -Recurse }
        }
    }
    foreach ($alias in $ResetSaves) {
        $remote = Join-Path $userdata "$((Resolve-Game $alias).AppId)\remote"
        if ((Test-Path -LiteralPath $remote) -and $PSCmdlet.ShouldProcess($remote, "remove the local cloud saves of $alias")) {
            Remove-Item -Recurse -Force $remote
        }
    }
    if ($PSCmdlet.ShouldProcess($users, 'set offline mode')) { Set-OfflineFlag $true }
    if ($PSCmdlet.ShouldProcess($steamExe, 'start Steam')) { Invoke-InDesktopSession -FilePath $steamExe -Arguments '-silent' }
    Write-Host "Steam is offline; backup in $BackupDir"
} else {
    if (-not (Test-Path -LiteralPath $BackupDir)) { throw "no backup in $BackupDir; Set-SteamOffline.ps1 -Mode Offline makes it" }
    if ($PSCmdlet.ShouldProcess($steam, 'close Steam')) { Stop-Steam }
    foreach ($saved in Get-ChildItem -LiteralPath $BackupDir -Directory) {
        $target = Join-Path $userdata $saved.Name
        if ($PSCmdlet.ShouldProcess($target, 'restore from backup')) {
            if (Test-Path -LiteralPath $target) { Remove-Item -Recurse -Force $target }
            Copy-Item -LiteralPath $saved.FullName -Destination $target -Recurse
        }
    }
    if ($PSCmdlet.ShouldProcess($users, 'clear offline mode')) { Set-OfflineFlag $false }
    if ($PSCmdlet.ShouldProcess($steamExe, 'start Steam')) { Invoke-InDesktopSession -FilePath $steamExe -Arguments '-silent' }
    Write-Host 'Steam is online'
}
