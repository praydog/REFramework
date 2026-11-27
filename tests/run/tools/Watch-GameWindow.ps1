<#
.SYNOPSIS
Logs a game window's position, size, monitor, style, DPI and focus every 100 ms (only changes are written).

.DESCRIPTION
A diagnostic for games that fight over their window, e.g. one stuck resizing (thousands of "Reset!" lines in the
REFramework log). Start it, then start the game. Window queries only work on the user's desktop; run over SSH, the
script restarts itself there through a scheduled task and waits for it.

.EXAMPLE
powershell -ExecutionPolicy Bypass -File tests\run\tools\Watch-GameWindow.ps1 -Game re7 -Seconds 300 -Log C:\temp\re7-window.log
#>
param(
    [string] $Game,
    # Process name without .exe, instead of -Game.
    [string] $ProcessName,
    [int] $Seconds = 300,
    [Parameter(Mandatory)] [string] $Log
)

$ErrorActionPreference = 'Stop'
Import-Module (Join-Path (Split-Path -Parent $PSScriptRoot) 'RFTest.psm1') -Force
if ($Game) { $ProcessName = [IO.Path]::GetFileNameWithoutExtension((Resolve-Game $Game).Exe) }
if (-not $ProcessName) { throw 'pass -Game or -ProcessName' }
$Log = [IO.Path]::GetFullPath($Log)

if (-not (Test-DesktopSession)) {
    Invoke-ScriptInDesktopSession -Script $PSCommandPath -Arguments "-ProcessName `"$ProcessName`" -Seconds $Seconds -Log `"$Log`"" -Wait -TimeoutSeconds ($Seconds + 60)
    if (Test-Path -LiteralPath $Log) { Get-Content -LiteralPath $Log }
    return
}

Add-Type @"
using System;
using System.Runtime.InteropServices;
public static class RFWindow {
    [StructLayout(LayoutKind.Sequential)] public struct RECT { public int L, T, R, B; }
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)] public struct MONITORINFOEX {
        public int cbSize; public RECT rcMonitor; public RECT rcWork; public uint dwFlags;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)] public string szDevice; }
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
    [DllImport("user32.dll")] public static extern bool IsIconic(IntPtr h);
    [DllImport("user32.dll")] public static extern IntPtr MonitorFromWindow(IntPtr h, uint f);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)] public static extern bool GetMonitorInfo(IntPtr m, ref MONITORINFOEX i);
    [DllImport("user32.dll")] public static extern int GetWindowLong(IntPtr h, int i);
    [DllImport("user32.dll")] public static extern uint GetDpiForWindow(IntPtr h);
}
"@

$end = (Get-Date).AddSeconds($Seconds)
$last = ''
"start $(Get-Date -Format o), process $ProcessName" | Set-Content -LiteralPath $Log
while ((Get-Date) -lt $end) {
    $p = Get-Process $ProcessName -ErrorAction SilentlyContinue | Where-Object { $_.MainWindowHandle -ne 0 } | Select-Object -First 1
    $line = 'no window'
    if ($p) {
        $h = $p.MainWindowHandle
        $r = New-Object RFWindow+RECT; $c = New-Object RFWindow+RECT
        [void][RFWindow]::GetWindowRect($h, [ref]$r); [void][RFWindow]::GetClientRect($h, [ref]$c)
        $mi = New-Object RFWindow+MONITORINFOEX; $mi.cbSize = [Runtime.InteropServices.Marshal]::SizeOf($mi)
        [void][RFWindow]::GetMonitorInfo([RFWindow]::MonitorFromWindow($h, 2), [ref]$mi)
        $line = 'window {0},{1} {2}x{3} client {4}x{5} monitor {6} {7}x{8} style {9:X8} dpi {10} foreground {11} minimized {12}' -f `
            $r.L, $r.T, ($r.R - $r.L), ($r.B - $r.T), $c.R, $c.B, $mi.szDevice, ($mi.rcMonitor.R - $mi.rcMonitor.L), `
            ($mi.rcMonitor.B - $mi.rcMonitor.T), [RFWindow]::GetWindowLong($h, -16), [RFWindow]::GetDpiForWindow($h), `
            ([RFWindow]::GetForegroundWindow() -eq $h), [RFWindow]::IsIconic($h)
    }
    if ($line -ne $last) { "$(Get-Date -Format HH:mm:ss.fff) $line" | Add-Content -LiteralPath $Log; $last = $line }
    Start-Sleep -Milliseconds 100
}
'end' | Add-Content -LiteralPath $Log
