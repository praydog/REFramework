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
  2. Backs up the userdata folders (cloud saves + remotecache.vdf) of the games in games.json to -BackupDir. Refuses a
     non-empty -BackupDir that isn't one of its own backups.
  3. Removes the local cloud saves of the -ResetSaves games, so they start with default settings.
  4. Sets Steam's offline-mode flag for the last-logged-in account and starts Steam on the user's desktop.
  Refuses to run while Steam is already in offline mode, so the backup always holds the last synced state.

-Mode Online:
  1. Closes Steam. Refuses unless Steam is in offline mode and -BackupDir holds one of its backups.
  2. Restores the backed-up userdata folders and removes the ones created while offline, so nothing written while
     offline is uploaded.
  3. Clears the offline-mode flag, renames the backup to <BackupDir>.restored-<time> so it can't be restored twice,
     and starts Steam.

Go online only with -Mode Online: going online from Steam's own menu can upload whatever changed while offline,
including the removal of saves by -ResetSaves.

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

# Marks a folder as this script's backup, so it never deletes a folder it didn't create.
$marker = Join-Path $BackupDir 'rftest-steam-backup.txt'

if ($Mode -eq 'Offline') {
    if (Test-OfflineFlag) { throw 'Steam is already in offline mode; run -Mode Online first (that restores the backup)' }
    if ((Test-Path -LiteralPath $BackupDir) -and (Get-ChildItem -LiteralPath $BackupDir -Force) -and -not (Test-Path -LiteralPath $marker)) {
        throw "$BackupDir is not empty and is not a backup made by this script; pass an empty or new -BackupDir"
    }
    if ($PSCmdlet.ShouldProcess($steam, 'close Steam')) { Stop-Steam }
    if ($PSCmdlet.ShouldProcess($BackupDir, "back up the userdata folders of $($appids.Count) games")) {
        # An old backup from an earlier offline period is stale once Steam has been online since.
        if (Test-Path -LiteralPath $BackupDir) { Remove-Item -LiteralPath $BackupDir -Recurse -Force }
        New-Item -ItemType Directory -Force -Path $BackupDir | Out-Null
        foreach ($id in $appids) {
            $src = Join-Path $userdata $id
            if (Test-Path -LiteralPath $src) { Copy-Item -LiteralPath $src -Destination (Join-Path $BackupDir $id) -Recurse }
        }
        Set-Content -LiteralPath $marker -Value "Steam userdata backup made by Set-SteamOffline.ps1 on $(Get-Date -Format o) from $userdata"
    }
    foreach ($alias in $ResetSaves) {
        $remote = Join-Path $userdata "$((Resolve-Game $alias).AppId)\remote"
        if ((Test-Path -LiteralPath $remote) -and $PSCmdlet.ShouldProcess($remote, "remove the local cloud saves of $alias")) {
            Remove-Item -LiteralPath $remote -Recurse -Force
        }
    }
    if ($PSCmdlet.ShouldProcess($users, 'set offline mode')) { Set-OfflineFlag $true }
    if ($PSCmdlet.ShouldProcess($steamExe, 'start Steam')) { Invoke-InDesktopSession -FilePath $steamExe -Arguments '-silent' }
    Write-Host "Steam is offline; backup in $BackupDir"
} else {
    if (-not (Test-Path -LiteralPath $marker)) { throw "no backup made by this script in $BackupDir (Set-SteamOffline.ps1 -Mode Offline makes it)" }
    if (-not (Test-OfflineFlag)) { throw 'Steam is not in offline mode, so the backup may be older than the saves; nothing restored' }
    if ($PSCmdlet.ShouldProcess($steam, 'close Steam')) { Stop-Steam }
    foreach ($id in $appids) {
        $saved = Join-Path $BackupDir $id
        $target = Join-Path $userdata $id
        if (Test-Path -LiteralPath $saved) {
            if ($PSCmdlet.ShouldProcess($target, 'restore from backup')) {
                if (Test-Path -LiteralPath $target) { Remove-Item -LiteralPath $target -Recurse -Force }
                Copy-Item -LiteralPath $saved -Destination $target -Recurse
            }
        } elseif ((Test-Path -LiteralPath $target) -and $PSCmdlet.ShouldProcess($target, 'remove (created while offline)')) {
            Remove-Item -LiteralPath $target -Recurse -Force
        }
    }
    if ($PSCmdlet.ShouldProcess($users, 'clear offline mode')) { Set-OfflineFlag $false }
    # Retire the backup so a later -Mode Online can't roll the saves back to it.
    $retired = "$($BackupDir.TrimEnd('\')).restored-$(Get-Date -Format yyyyMMdd-HHmmss)"
    if ($PSCmdlet.ShouldProcess($BackupDir, "rename to $retired")) { Rename-Item -LiteralPath $BackupDir -NewName (Split-Path -Leaf $retired) }
    if ($PSCmdlet.ShouldProcess($steamExe, 'start Steam')) { Invoke-InDesktopSession -FilePath $steamExe -Arguments '-silent' }
    Write-Host 'Steam is online'
}
