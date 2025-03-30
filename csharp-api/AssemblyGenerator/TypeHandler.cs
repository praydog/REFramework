#nullable enable

using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using System.Collections.Generic;
using System.Linq;
using REFrameworkNET;

using static Microsoft.CodeAnalysis.CSharp.SyntaxFactory;
using System;

class TypeHandler {
    public static TypeSyntax VoidType() => PredefinedType(Token(SyntaxKind.VoidKeyword));
    public static TypeSyntax ObjType() => PredefinedType(Token(SyntaxKind.ObjectKeyword));
    public static Dictionary<string, TypeSyntax> Predefined = new() {
        ["System.Single"] = ParseTypeName("float"),
        ["System.Double"] = ParseTypeName("double"),
        ["System.Int32"] = ParseTypeName("int"),
        ["System.UInt32"] = ParseTypeName("uint"),
        ["System.Int16"] = ParseTypeName("short"),
        ["System.UInt16"] = ParseTypeName("ushort"),
        ["System.Byte"] = ParseTypeName("byte"),
        ["System.SByte"] = ParseTypeName("sbyte"),
        ["System.Char"] = ParseTypeName("char"),
        ["System.Int64"] = ParseTypeName("long"),
        ["System.IntPtr"] = ParseTypeName("long"),
        ["System.UInt64"] = ParseTypeName("ulong"),
        ["System.UIntPtr"] = ParseTypeName("ulong"),
        ["System.Boolean"] = ParseTypeName("bool"),
        ["System.String"] = ParseTypeName("string"),
        ["via.clr.ManagedObject"] = ObjType(),
        ["System.Object"] = ObjType(),
        ["System.Void"] = VoidType(),
        ["!0"] = ParseTypeName("T"),
        ["!1"] = ParseTypeName("U"),
        ["!2"] = ParseTypeName("V"),
        ["!3"] = ParseTypeName("W"),
        ["!4"] = ParseTypeName("X"),
        ["!5"] = ParseTypeName("Y"),
        ["!6"] = ParseTypeName("Z"),
        ["!7"] = ParseTypeName("P7"),
        ["!8"] = ParseTypeName("P8"),
        ["!9"] = ParseTypeName("P9"),
    };

    public static Dictionary<uint, TypeSyntax> Cache = new();

    public static (string, int) BaseTypeName(string baseName) {
        if (baseName == "file") {
            return ("_file", 0);
        }
        if (baseName.Split('`').ToArray() is [var name, var count])
            return (name, int.Parse(count));

        return (baseName, 0);

    }
    public static bool SkipType(TypeDefinition type) {
        if (type.Name == "") return true;
        if (type.Name.StartsWith("<")) return true;
        if (type.Name.Contains("!!")) return true;

       if (!type.IsGenericType()) {
            if (NameHierarchy(type).Any(name => name.Contains('`'))) return true;
        }
        return false;
    }

    public static BaseTypeSyntax[] ParentTypes(TypeDefinition type) {
        List<BaseTypeSyntax> parents = new();
        var parentType = type.ParentType;
        while (parentType != null) {
            if (parentType.Name == "") break;
            if (parentType.FullName == "System.Object") {
                parents.Insert(0, SimpleBaseType(ParseTypeName("global::_System.Object")));
                break;
            }

            var properType = ProperType(parentType);
            if (!properType.IsEquivalentTo(ObjType())) {
                parents.Insert(0, SimpleBaseType(properType));
            }
            parentType = parentType.ParentType;
        }
        return parents.ToArray();
    }


    public static TypeSyntax ProperType(TypeDefinition type) {
        if (Predefined.ContainsKey(type.FullName))
            return Predefined[type.FullName];
        if (Cache.ContainsKey(type.Index))
            return Cache[type.Index];
        return ObjType();
    }

    public static string[] NameHierarchy(TypeDefinition type) {
        var typeList = new List<string>();
        while (true) {
            typeList.Insert(0, type.Name);
            if (type.DeclaringType is null || type.DeclaringType == type)
                break;
            type = type.DeclaringType;
        }
        if (type.Namespace is not null && type.Namespace.Any()) {
            typeList.Insert(0, type.Namespace);
        } else {
            typeList.Insert(0, "_");
        }
        return [.. typeList];
    }

    static TypeSyntax BuildProperType(REFrameworkNET.TypeDefinition? targetType) {
        if (targetType is null) return VoidType();
        if (Predefined.ContainsKey(targetType.FullName))
            return Predefined[targetType.FullName];
        if (Cache.ContainsKey(targetType.Index))
            return Cache[targetType.Index];

        Cache[targetType.Index] = ObjType();
        if (targetType.GetElementType() is TypeDefinition elemType) {
            Cache[targetType.Index] = ObjType();
            var elem = BuildProperType(elemType);
            var arraySyntax = QualifiedName(
                    ParseName("global::_System.Array"),
                    GenericName("Impl")
                        .AddTypeArgumentListArguments([elem])
            );
            Cache[targetType.Index] = arraySyntax;
            return arraySyntax;
        }
        if (SkipType(targetType)) return ObjType();

        var typeList = NameHierarchy(targetType);
        int genericIndex = 0;
        var generics = targetType.GenericArguments ?? [];
        var toParse = string.Join(".", typeList.Select(tName => {
            var (name, count) = BaseTypeName(tName);
            if (count == 0) return name;
            name += "<";
            for (int i = 0; i < count; ++i) {
                if (i > 0) name += ",";
                if (i + genericIndex >= generics.Count()) {
                    name += "UNKN";
                    continue;
                }
                var generic = generics[i + genericIndex];
                if (generic is null)
                    name += "object";
                else
                    name += ProperType(generics[i + genericIndex]).ToFullString();
            }
            name += ">";
            genericIndex += count;
            return name;
        }
        ));
        if (toParse.StartsWith("System"))
            toParse = "_" + toParse;
        var parsed = ParseTypeName($"global::{toParse}");
        Cache[targetType.Index] = parsed;
        return parsed;
    }

    public static void BuildProperTypes() {
        foreach (TypeDefinition type in API.GetTDB().Types) {
            BuildProperType(type);
            // Special case delegates again
            if (!(type.FullName.StartsWith("System.Action") || type.FullName.StartsWith("System.Func"))) {
                if (type.IsGenericType()) BuildProperType(type.GetGenericTypeDefinition());
            }
        }
        API.LogInfo($"Built {Cache.Count} proper types");
    }

    public static MemberDeclarationSyntax? GenerateType(TypeDefinition t) {
        if (t.Name == "") return null;
        if (t.FullName.EndsWith("[]")) return null;
        if (SkipType(t)) return null;
        if (t.IsGenericType() && !t.IsGenericTypeDefinition()) return null;

        try {

            // Enum
            if (t.IsEnum()) {
                var (baseName, _) = TypeHandler.BaseTypeName(t.Name);
                var nestedEnumGenerator = new EnumGenerator(baseName, t);
                return nestedEnumGenerator.EnumDeclaration;
            }
            var nestedGenerator = new ClassGenerator(t);
            return nestedGenerator.TypeDeclaration;
        }
        catch (Exception e) {
            throw new Exception($"{e.Message}\nWhile handling {t.FullName}", e);

        }
    }

    public static MemberDeclarationSyntax[] GenerateNestedTypes(TypeDefinition t) {
        var nestedTypes = Il2CppDump.GetTypeExtension(t)?.NestedTypes;
        return nestedTypes?
            .Select(GenerateType)
            .Where(t => t is not null)
            .Select(t => t!)
            .ToArray() ?? [];
    }
}
