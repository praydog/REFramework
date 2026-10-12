# Value storage pinning regression

On Windows x64, configure the C# bridge with `-DREF_CSHARP_PINNING_TESTS=ON`,
build `csharp-api`, and run:

```powershell
dotnet run --project csharp-api/test/ValueTypePinning -c Release
```

Override `-p:BridgeDirectory=C:/path/to/bridge` if needed. The fixture is off by
default and must be disabled again for a production build. It compiles into the
test bridge to exercise the same native RAII scope used by production reads.

The fixture fabricates only CLR value storage, with no native game metadata.
It checks address movement without a pin (without dereferencing the old pointer),
stable reads and writes across compacting GC, proxy unwrapping, overlapping pins,
and release of the pinned buffer after a managed exception.

Live validation must additionally cover reflection/typed field reads and writes,
nested value copies, value arguments, returned values and instance mutation
copy-back, under collection pressure. Raw pointer APIs such as `Ptr()`,
`GetAddress()` and `Field.GetDataBoxed(address, ...)` still require a caller-owned
pin; a returned integer or native pointer cannot root or pin CLR storage.
