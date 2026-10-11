#pragma once

namespace REFrameworkNET {
// Kept separate so <Windows.h> stays out of translation units with managed code that its macros could clobber
private ref class ConsoleWindow {
internal:
    // Shows the console without stealing focus, allocating it first if needed; hiding never frees it
    static void SetVisible(bool visible);
};
}
