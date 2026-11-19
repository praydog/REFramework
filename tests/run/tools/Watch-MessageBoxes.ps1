<#
.SYNOPSIS
Dismisses REFramework "ScriptRunner Message" boxes and logs their text, so unattended script runs can't stall.

.DESCRIPTION
A Lua script that fails to load makes REFramework show a message box, which blocks the game until someone clicks it.
This watches the desktop for those boxes, appends each message to -Log and closes the box. It stops after -Seconds or
as soon as -StopFile exists. Must run on the user's desktop (Compare-ScriptErrors.ps1 starts it there).
#>
param(
    [Parameter(Mandatory)] [string] $Log,
    [int] $Seconds = 900,
    [string] $StopFile
)

Add-Type @"
using System;
using System.Text;
using System.Runtime.InteropServices;
public static class RFBoxWatch {
    public delegate bool EnumProc(IntPtr h, IntPtr l);
    [DllImport("user32.dll")] public static extern bool EnumWindows(EnumProc cb, IntPtr l);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)] public static extern int GetWindowText(IntPtr h, StringBuilder s, int n);
    [DllImport("user32.dll")] public static extern IntPtr GetDlgItem(IntPtr h, int id);
    [DllImport("user32.dll")] public static extern bool IsWindowVisible(IntPtr h);
    [DllImport("user32.dll")] public static extern bool PostMessage(IntPtr h, uint m, IntPtr w, IntPtr l);
    public static string Text(IntPtr h) { var sb = new StringBuilder(4096); GetWindowText(h, sb, sb.Capacity); return sb.ToString(); }
}
"@

$BM_CLICK = 0x00F5
$WM_CLOSE = 0x0010
$end = (Get-Date).AddSeconds($Seconds)
while ((Get-Date) -lt $end -and -not ($StopFile -and (Test-Path -LiteralPath $StopFile))) {
    $found = New-Object System.Collections.Generic.List[IntPtr]
    [void][RFBoxWatch]::EnumWindows({ param($h, $l)
        if ([RFBoxWatch]::IsWindowVisible($h) -and [RFBoxWatch]::Text($h) -eq 'ScriptRunner Message') { $found.Add($h) }
        $true }, [IntPtr]::Zero)
    foreach ($h in $found) {
        $msg = [RFBoxWatch]::Text([RFBoxWatch]::GetDlgItem($h, 0xFFFF))
        Add-Content -LiteralPath $Log -Value ($msg -replace "`r?`n", ' | ')
        $button = [RFBoxWatch]::GetDlgItem($h, 2)
        if ($button -eq [IntPtr]::Zero) { $button = [RFBoxWatch]::GetDlgItem($h, 1) }
        if ($button -ne [IntPtr]::Zero) { [void][RFBoxWatch]::PostMessage($button, $BM_CLICK, [IntPtr]::Zero, [IntPtr]::Zero) }
        [void][RFBoxWatch]::PostMessage($h, $WM_CLOSE, [IntPtr]::Zero, [IntPtr]::Zero)
        Start-Sleep -Milliseconds 300
    }
    Start-Sleep -Milliseconds 500
}
