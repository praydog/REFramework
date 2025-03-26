using System;
using System.Collections.Generic;

namespace REFrameworkNET;

public static class TypeName {
    static Dictionary<System.Type, string> Predefined = new() {
        [typeof(float)] = "System.Single",
        [typeof(double)] = "System.Double",
        [typeof(int)] = "System.Int32",
        [typeof(uint)] = "System.UInt32",
        [typeof(short)] = "System.Int16",
        [typeof(ushort)] = "System.UInt16",
        [typeof(byte)] = "System.Byte",
        [typeof(sbyte)] = "System.SByte",
        [typeof(char)] = "System.Char",
        [typeof(long)] = "System.Int64",
        [typeof(long)] = "System.IntPtr",
        [typeof(ulong)] = "System.UInt64",
        [typeof(ulong)] = "System.UIntPtr",
        [typeof(bool)] = "System.Boolean",
        [typeof(string)] = "System.String",
        [typeof(object)] = "System.Object",
    };
    public static string Get<T>() {
        if (Predefined.ContainsKey(typeof(T))) {
            return Predefined[typeof(T)];
        }
        return typeof(T).GetField("REFTypeName")?.GetValue(null) as string ?? "";
    }
}