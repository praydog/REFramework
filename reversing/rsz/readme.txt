emulation-dumper.py:
Takes an il2cpp_dump.json (framework generated) and emulates all of the deserializer chains to guess the RSZ structure layout for native types.

Outputs: 
native-layouts.json
dump.txt

non-native-dumper.py:
Takes an il2cpp_dump.json and native-layouts.json from the emulation-dumper.py to generate the RSZ structures for non-native types.

Outputs:
RSZ_dump_python.txt