using System;

using static reframework.API;
public class TDBWrapper {
    private readonly TDB _original;

    public TDBWrapper(TDB original) {
        _original = original;
    }

    public UInt32 GetNumTypes() {
        return _original.get_num_types();
    }

    public UInt32 GetNumMethods() {
        return _original.get_num_methods();
    }

    public UInt32 GetNumFields() {
        return _original.get_num_fields();
    }

    public UInt32 GetNumProperties() {
        return _original.get_num_properties();
    }

    public UInt32 GetStringsSize() {
        return _original.get_strings_size();
    }

    public UInt32 GetRawDataSize() {
        return _original.get_raw_data_size();
    }

    /*public SByte* GetStringDatabase() {
        return _original.get_string_database();
    }

    public Byte* GetRawDatabase() {
        return _original.get_raw_database();
    }*/

    public Span<byte> GetRawData() {
        return _original.get_raw_data();
    }

    public String GetString(UInt32 index) {
        return _original.get_string(index);
    }

    public TypeDefinitionWrapper GetType(UInt32 index) {
        return new TypeDefinitionWrapper(_original.get_type(index));
    }

    public TypeDefinitionWrapper FindType(String name) {
        return new TypeDefinitionWrapper(_original.find_type(name));
    }

    public TypeDefinitionWrapper FindTypeByFqn(UInt32 fqn) {
        return new TypeDefinitionWrapper(_original.find_type_by_fqn(fqn));
    }

    public Method GetMethod(UInt32 index) {
        return _original.get_method(index);
    }

    public Method FindMethod(String type_name, String name) {
        return _original.find_method(type_name, name);
    }

    public Field GetField(UInt32 index) {
        return _original.get_field(index);
    }

    public Field FindField(String type_name, String name) {
        return _original.find_field(type_name, name);
    }

    public Property GetProperty(UInt32 index) {
        return _original.get_property(index);
    }
}
