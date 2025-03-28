#nullable enable

using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using System.Reflection;
using System.Collections.Generic;
using Microsoft.CodeAnalysis.Emit;
using System.IO;
using System.Dynamic;
using System.Security.Cryptography;
using System.Linq;
using Microsoft.CodeAnalysis.Operations;
using REFrameworkNET;

public class EnumGenerator {
    private string enumName;
    private REFrameworkNET.TypeDefinition t;
    private EnumDeclarationSyntax? enumDeclaration;

    public EnumDeclarationSyntax? EnumDeclaration { 
        get {
            return enumDeclaration;
        }
    }

    public EnumGenerator(string enumName, REFrameworkNET.TypeDefinition t) {
        this.enumName = REFrameworkNET.AssemblyGenerator.FixBadChars(enumName);
        this.t = t;

        enumDeclaration = Generate();
    }

    public void Update(EnumDeclarationSyntax? typeDeclaration) {
        this.enumDeclaration = typeDeclaration;
    }
    static readonly REFrameworkNET.ManagedObject s_FlagsAttribute = REFrameworkNET.TDB.Get().FindType("System.FlagsAttribute").GetRuntimeType();

    public EnumDeclarationSyntax? Generate() {
        var ogEnumName = new string(enumName);

        // Pull out the last part of the class name (split '.' till last)
        if (t.DeclaringType == null) {
            enumName = enumName.Split('.').Last();
        }

        enumDeclaration = SyntaxFactory.EnumDeclaration(enumName)
            .AddModifiers(SyntaxFactory.Token(SyntaxKind.PublicKeyword));

        // Check if we need to add the new keyword to this.
        if (AssemblyGenerator.NestedTypeExistsInParent(t)) {
            enumDeclaration = enumDeclaration.AddModifiers(SyntaxFactory.Token(SyntaxKind.NewKeyword));
        } else {
            var declaringType = t.DeclaringType;

            if (declaringType != null) {
                if (declaringType.FindField(t.Name) != null) {
                    enumDeclaration = enumDeclaration.AddModifiers(SyntaxFactory.Token(SyntaxKind.NewKeyword));
                }
            }
        }

        if (t.HasAttribute(s_FlagsAttribute, true)) {
            enumDeclaration = enumDeclaration.AddAttributeLists(SyntaxFactory.AttributeList().AddAttributes(SyntaxFactory.Attribute(SyntaxFactory.ParseName("System.FlagsAttribute"))));
        }

        foreach (REFrameworkNET.Field field in t.Fields) {
            if (!field.IsStatic() || !field.IsLiteral()) {
                continue;
            }

            if (field.GetDeclaringType() != t) {
                continue;
            }

            var underlyingType = field.Type.GetUnderlyingType();

            SyntaxToken literalToken;
            bool foundRightType = true;

            switch (underlyingType.FullName) {
                case "System.Byte":
                    enumDeclaration = enumDeclaration.AddBaseListTypes(SyntaxFactory.SimpleBaseType(SyntaxFactory.ParseTypeName("byte")));
                    literalToken = SyntaxFactory.Literal(field.GetDataT<byte>(0, false));
                    break;
                case "System.SByte":
                    enumDeclaration = enumDeclaration.AddBaseListTypes(SyntaxFactory.SimpleBaseType(SyntaxFactory.ParseTypeName("sbyte")));
                    literalToken = SyntaxFactory.Literal(field.GetDataT<sbyte>(0, false));
                    break;
                case "System.Int16":
                    enumDeclaration = enumDeclaration.AddBaseListTypes(SyntaxFactory.SimpleBaseType(SyntaxFactory.ParseTypeName("short")));
                    literalToken = SyntaxFactory.Literal(field.GetDataT<short>(0, false));
                    break;
                case "System.UInt16":
                    enumDeclaration = enumDeclaration.AddBaseListTypes(SyntaxFactory.SimpleBaseType(SyntaxFactory.ParseTypeName("ushort")));
                    literalToken = SyntaxFactory.Literal(field.GetDataT<ushort>(0, false));
                    break;
                case "System.Int32":
                    enumDeclaration = enumDeclaration.AddBaseListTypes(SyntaxFactory.SimpleBaseType(SyntaxFactory.ParseTypeName("int")));
                    literalToken = SyntaxFactory.Literal(field.GetDataT<int>(0, false));
                    break;
                case "System.UInt32":
                    enumDeclaration = enumDeclaration.AddBaseListTypes(SyntaxFactory.SimpleBaseType(SyntaxFactory.ParseTypeName("uint")));
                    literalToken = SyntaxFactory.Literal(field.GetDataT<uint>(0, false));
                    break;
                case "System.Int64":
                    enumDeclaration = enumDeclaration.AddBaseListTypes(SyntaxFactory.SimpleBaseType(SyntaxFactory.ParseTypeName("long")));
                    literalToken = SyntaxFactory.Literal(field.GetDataT<long>(0, false));
                    break;
                case "System.UInt64":
                    enumDeclaration = enumDeclaration.AddBaseListTypes(SyntaxFactory.SimpleBaseType(SyntaxFactory.ParseTypeName("ulong")));
                    literalToken = SyntaxFactory.Literal(field.GetDataT<ulong>(0, false));
                    break;
                default:
                    literalToken = SyntaxFactory.Literal(0);
                    foundRightType = false;
                    break;
            }

            // The uber fallback
            if (!foundRightType) {
                REFrameworkNET.API.LogWarning($"Enum {enumName} has an unknown underlying type {underlyingType.FullName}");

                switch (underlyingType.GetValueTypeSize()) {
                case 1:
                    enumDeclaration = enumDeclaration.AddBaseListTypes(SyntaxFactory.SimpleBaseType(SyntaxFactory.ParseTypeName("byte")));
                    literalToken = SyntaxFactory.Literal(field.GetDataT<byte>(0, false));
                    break;
                case 2:
                    enumDeclaration = enumDeclaration.AddBaseListTypes(SyntaxFactory.SimpleBaseType(SyntaxFactory.ParseTypeName("short")));
                    literalToken = SyntaxFactory.Literal(field.GetDataT<short>(0, false));
                    break;
                case 4:
                    enumDeclaration = enumDeclaration.AddBaseListTypes(SyntaxFactory.SimpleBaseType(SyntaxFactory.ParseTypeName("int")));
                    literalToken = SyntaxFactory.Literal(field.GetDataT<int>(0, false));
                    break;
                case 8:
                    enumDeclaration = enumDeclaration.AddBaseListTypes(SyntaxFactory.SimpleBaseType(SyntaxFactory.ParseTypeName("long")));
                    literalToken = SyntaxFactory.Literal(field.GetDataT<long>(0, false));
                    break;
                default:
                    literalToken = SyntaxFactory.Literal(0);
                    throw new System.Exception("Unknown enum underlying type size");
                }
            }

            var fieldDeclaration = SyntaxFactory.EnumMemberDeclaration(field.Name);
            var valueExpr = SyntaxFactory.LiteralExpression(SyntaxKind.NumericLiteralExpression, literalToken);
            fieldDeclaration = fieldDeclaration.WithEqualsValue(SyntaxFactory.EqualsValueClause(valueExpr));
            enumDeclaration = enumDeclaration.AddMembers(fieldDeclaration);
        }

        return enumDeclaration;
    }
}