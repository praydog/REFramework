/* Minimal DLL for the package.loadlib regression test (tests/lua/rftest_regression.lua): the runner copies it to
   reframework/data/rftest_loadlib.dll, and package.loadlib("rftest_loadlib.dll", "*") must load it from there. */
__declspec(dllexport) int rftest_loadlib_marker(void) { return 42; }
