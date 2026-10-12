# Value storage pinning regression

On Windows x64 with Visual Studio C++/CLI support and the .NET 10 SDK, configure
the bridge normally and build the explicit regression target from the repository
root:

```powershell
cmake -S csharp-api -B build -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target check-value-pinning -- /restore
```

This builds the normal `csharp-api` dependency if needed, builds the separate
`ValueTypePinningNative` C++/CLI assembly, and runs its C# host. Neither test target
belongs to the default build. There is no special bridge configuration or test
code in `REFramework.NET.dll`.

The fixture imports the real bridge types and compiles the same
`ObjectAddress.inl` implementation as production. A friend-assembly declaration
allows access to the internal buffer accessor; no public test API is added to
the bridge. The fixture checks that its types are absent from the bridge and
that the imported `ValueType` belongs to a different assembly. This tests the
shared scope implementation; live probes additionally cover its call sites.

Once built, rerun without building any C++ projects:

```powershell
dotnet run --project csharp-api/test/ValueTypePinning -c Release
```

For another build directory, supply `-p:BridgeDirectory=C:/path/to/bin` and
`-p:TestDirectory=C:/path/to/tests/ValueTypePinning`. The CMake target supplies
these paths automatically.

The fixture fabricates only CLR value storage, with no native game metadata.
It checks address movement without a pin (without dereferencing the old pointer),
stable reads and writes across compacting GC, proxy unwrapping, overlapping pins,
and release of the pinned buffer after a managed exception.

Live validation must additionally cover reflection/typed field reads and writes,
nested value copies, value arguments, returned values and instance mutation
copy-back, under collection pressure. Raw pointer APIs such as `Ptr()`,
`GetAddress()` and `Field.GetDataBoxed(address, ...)` still require a caller-owned
pin; a returned integer or native pointer cannot root or pin CLR storage.
