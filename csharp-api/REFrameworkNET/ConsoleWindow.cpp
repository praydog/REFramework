#include <Windows.h>

#include "ConsoleWindow.hpp"

using namespace System;

namespace REFrameworkNET {
void ConsoleWindow::SetVisible(bool visible) {
    HWND console = GetConsoleWindow();

    if (!visible) {
        if (console != nullptr) {
            ShowWindow(console, SW_HIDE);
        }

        return;
    }

    HWND prev = GetForegroundWindow();

    if (console == nullptr) {
        if (!AllocConsole()) {
            return;
        }

        // .NET caches stdout/stderr on first use; rebind them in case they were resolved while no console existed
        auto out = gcnew System::IO::StreamWriter(Console::OpenStandardOutput(), Console::OutputEncoding, 256, false);
        out->AutoFlush = true;
        Console::SetOut(out);

        auto err = gcnew System::IO::StreamWriter(Console::OpenStandardError(), Console::OutputEncoding, 256, false);
        err->AutoFlush = true;
        Console::SetError(err);

        console = GetConsoleWindow();
        ShowWindow(console, SW_HIDE);
    }

    ShowWindow(console, SW_SHOWNOACTIVATE);

    if (prev) {
        SetForegroundWindow(prev);
    }
}
}
