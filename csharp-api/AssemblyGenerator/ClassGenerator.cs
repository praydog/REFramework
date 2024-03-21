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


public class ClassGenerator {
    private string className;
    private Il2CppDump.Type t;
    private Il2CppDump.Method[] methods = [];
    public List<Il2CppDump.Type> usingTypes = [];
    private TypeDeclarationSyntax? typeDeclaration;
    
    public TypeDeclarationSyntax? TypeDeclaration {
        get {
            return typeDeclaration;
        }
    }

    public void Update(TypeDeclarationSyntax? typeDeclaration_) {
        typeDeclaration = typeDeclaration_;
    }

    public ClassGenerator(string className_, Il2CppDump.Type t_, Il2CppDump.Method[] methods_) {
        className = className_;
        t = t_;
        methods = methods_;

        typeDeclaration = Generate();
    }
    
    private TypeDeclarationSyntax? Generate() {
        usingTypes = [];

        SortedSet<string> invalidMethodNames = new SortedSet<string> {
            "Finalize",
            "MemberwiseClone",
            "ToString",
            "Equals",
            "GetHashCode",
            "GetType",
            ".ctor",
            ".cctor",
            "op_Implicit",
            "op_Explicit",
            /*"op_Addition",
            "op_Subtraction",
            "op_Multiply",
            "op_Division",
            "op_Modulus",
            "op_BitwiseAnd",
            "op_BitwiseOr",
            "op_ExclusiveOr",*/
            
        };

        var ogClassName = new string(className);

        // Pull out the last part of the class name (split '.' till last)
        if (t.DeclaringType == null) {
            className = className.Split('.').Last();
        }

        bool wantsAbstractInstead = false;

        for (var parent = t.ParentType; parent != null; parent = parent.ParentType) {
            if (parent.Name == null) {
                break;
            }

            if (parent.Name == "") {
                break;
            }

            if (parent.Name == "System.Attribute") {
                wantsAbstractInstead = true;
                break;
            }
        }

        if (wantsAbstractInstead) {
            typeDeclaration = SyntaxFactory
                        .ClassDeclaration(className)
                        .AddModifiers(new SyntaxToken[]{SyntaxFactory.Token(SyntaxKind.PublicKeyword), SyntaxFactory.Token(SyntaxKind.AbstractKeyword)});
        } else {
            typeDeclaration = SyntaxFactory
                .InterfaceDeclaration(className)
                .AddModifiers(new SyntaxToken[]{SyntaxFactory.Token(SyntaxKind.PublicKeyword)});
        }

        if (typeDeclaration == null) {
            return null;
        }

        typeDeclaration = typeDeclaration
            .AddMembers(methods.Where(method => method.Name != null && !invalidMethodNames.Contains(method.FriendlyName??method.Name) && !method.Name.Contains('<')).Select(method => {
                TypeSyntax? returnType = SyntaxFactory.PredefinedType(SyntaxFactory.Token(SyntaxKind.VoidKeyword));

                if (method.Returns != null && method.Returns.Type != "System.Void" && method.Returns.Type != "") {
                    // Check for easily convertible types like System.Single, System.Int32, etc.
                    switch (method.Returns.Type) {
                        case "System.Single":
                            returnType = SyntaxFactory.PredefinedType(SyntaxFactory.Token(SyntaxKind.FloatKeyword));
                            break;
                        case "System.Double":
                            returnType = SyntaxFactory.PredefinedType(SyntaxFactory.Token(SyntaxKind.DoubleKeyword));
                            break;
                        case "System.Int32":
                            returnType = SyntaxFactory.PredefinedType(SyntaxFactory.Token(SyntaxKind.IntKeyword));
                            break;
                        case "System.UInt32":
                            returnType = SyntaxFactory.PredefinedType(SyntaxFactory.Token(SyntaxKind.UIntKeyword));
                            break;
                        case "System.Int64":
                        case "System.IntPtr":
                            returnType = SyntaxFactory.PredefinedType(SyntaxFactory.Token(SyntaxKind.LongKeyword));
                            break;
                        case "System.UInt64":
                        case "System.UIntPtr":
                            returnType = SyntaxFactory.PredefinedType(SyntaxFactory.Token(SyntaxKind.ULongKeyword));
                            break;
                        case "System.Boolean":
                            returnType = SyntaxFactory.PredefinedType(SyntaxFactory.Token(SyntaxKind.BoolKeyword));
                            break;
                        case "System.String":
                            returnType = SyntaxFactory.PredefinedType(SyntaxFactory.Token(SyntaxKind.StringKeyword));
                            break;
                        case "via.clr.ManagedObject":
                        case "System.Object":
                            returnType = SyntaxFactory.PredefinedType(SyntaxFactory.Token(SyntaxKind.ObjectKeyword));
                            break;
                        default:
                            if (method.Returns != null && method.Returns.Type != null && method.Returns.Type != "") {
                                if (!Program.validTypes.Contains(method.Returns.Type)) {
                                    returnType = SyntaxFactory.PredefinedType(SyntaxFactory.Token(SyntaxKind.ObjectKeyword));
                                    break;
                                }

                                if (method.Returns.Type.Contains('<') || method.Returns.Type.Contains('[')) {
                                    returnType = SyntaxFactory.PredefinedType(SyntaxFactory.Token(SyntaxKind.ObjectKeyword));
                                    break;
                                }

                                if (method.Returns.Type.StartsWith("System.") || !method.Returns.Type.StartsWith("via.")) {
                                    returnType = SyntaxFactory.PredefinedType(SyntaxFactory.Token(SyntaxKind.ObjectKeyword));
                                    break;
                                }

                                if (Program.Dump == null) {
                                    returnType = SyntaxFactory.PredefinedType(SyntaxFactory.Token(SyntaxKind.ObjectKeyword));
                                    break;
                                }

                                var returnName = method.Returns.Type;
                                /*var returnTypeNamespace = Program.ExtractNamespaceFromTypeName(Program.Dump, returnName);
                                var typeNamespace = Program.ExtractNamespaceFromTypeName(Program.Dump, ogClassName);

                                if (returnTypeNamespace == typeNamespace) {
                                    returnName = returnName.Split('.').Last();
                                }*/
                                
                                returnType = SyntaxFactory.ParseTypeName(returnName);
                                break;
                            }

                            returnType = SyntaxFactory.PredefinedType(SyntaxFactory.Token(SyntaxKind.ObjectKeyword));
                            break;
                    }
                }

                var flags = method.Flags?.Split(" | ");
                var methodName = new string(method.FriendlyName);

                if (method.Override != null && method.Override == true) {
                    methodName += "_" + className.Replace('.', '_');
                }

                var methodDeclaration = SyntaxFactory.MethodDeclaration(returnType, methodName ?? "UnknownMethod")
                    .AddModifiers(new SyntaxToken[]{SyntaxFactory.Token(SyntaxKind.PublicKeyword)})
                    /*.AddBodyStatements(SyntaxFactory.ParseStatement("throw new System.NotImplementedException();"))*/;

                if (wantsAbstractInstead) {
                    methodDeclaration = methodDeclaration.AddModifiers(SyntaxFactory.Token(SyntaxKind.AbstractKeyword));
                }

                if (method.Params != null) {
                    methodDeclaration = methodDeclaration.AddParameterListParameters(method.Params.Where(param => param != null && param.Type != null && param.Name != null).Select(param => {
                        return SyntaxFactory.Parameter(SyntaxFactory.Identifier(param.Name ?? "UnknownParam"))
                            .WithType(SyntaxFactory.ParseTypeName(/*param.Type ??*/ "object"));
                    }).ToArray());
                }

                // find "Virtual" flag
                //if (flags != null && flags.Contains("Virtual")) {
                    //methodDeclaration = methodDeclaration.AddModifiers(SyntaxFactory.Token(SyntaxKind.VirtualKeyword));

                    if (method.Override != null && method.Override == true) {
                        //methodDeclaration = methodDeclaration.AddModifiers(SyntaxFactory.Token(SyntaxKind.OverrideKeyword));
                        
                    }
                //}

                return methodDeclaration;
            }).ToArray());

        if (!className.Contains('[') && !className.Contains('<')) {
            List<SimpleBaseTypeSyntax> baseTypes = new List<SimpleBaseTypeSyntax>();

            SortedSet<string> badBaseTypes = new SortedSet<string> {
                "System.Object",
                "System.ValueType",
                "System.Enum",
                "System.Delegate",
                "System.MulticastDelegate"
            };

            for (var parent = t.ParentType; parent != null; parent = parent.ParentType) {
                if (parent.Name == null) {
                    break;
                }

                if (parent.Name == "") {
                    break;
                }

                if (parent.Name.Contains('[') || parent.Name.Contains('<')) {
                    break;
                }

                if (badBaseTypes.Contains(parent.Name)) {
                    break;
                }

                baseTypes.Add(SyntaxFactory.SimpleBaseType(SyntaxFactory.ParseTypeName(parent.Name)));
                usingTypes.Add(parent);
                break;
            }

            if (baseTypes.Count > 0 && typeDeclaration != null) {
                if (wantsAbstractInstead && typeDeclaration is ClassDeclarationSyntax) {
                    typeDeclaration = (typeDeclaration as ClassDeclarationSyntax)?.AddBaseListTypes(baseTypes.ToArray());
                } else if (typeDeclaration is InterfaceDeclarationSyntax) {
                    typeDeclaration = (typeDeclaration as InterfaceDeclarationSyntax)?.AddBaseListTypes(baseTypes.ToArray());
                }
            }
        }

        return GenerateNestedTypes();
    }

    private TypeDeclarationSyntax? GenerateNestedTypes() {
        if (this.typeDeclaration == null) {
            return null;
        }

        foreach (var nestedT in t.NestedTypes ?? []) {
            var nestedTypeName = nestedT.Name ?? "";

            if (nestedTypeName == "") {
                continue;
            }

            if (nestedTypeName.Contains("[") || nestedTypeName.Contains("]") || nestedTypeName.Contains('<')) {
                continue;
            }

            if (nestedTypeName.Contains("WrappedArrayContainer")) {
                continue;
            }

            if (nestedTypeName.Split('.').Last() == "file") {
                nestedTypeName = nestedTypeName.Replace("file", "@file");
            }

            var fixedNestedMethods = nestedT.Methods?
                .Select(methodPair => {
                    var method = methodPair.Value;
                    var methodName = Il2CppDump.StripMethodName(method);
                    return (methodName, method);
                })
                .GroupBy(pair => pair.methodName)
                .Select(group => group.First()) // Selects the first method of each group
                .ToDictionary(pair => pair.methodName, pair => pair.method);

            var nestedGenerator = new ClassGenerator(nestedTypeName.Split('.').Last(),
                nestedT,
                fixedNestedMethods != null ? [.. fixedNestedMethods.Values] : []
            );

            if (nestedGenerator.TypeDeclaration == null) {
                continue;
            }

            var isolatedNestedName = nestedTypeName.Split('.').Last();

            bool addedNew = false;
            // Add the "new" keyword if this nested type is anywhere in the hierarchy
            for (var parent = t.ParentType; parent != null; parent = parent.ParentType) {
                if (addedNew) {
                    break;
                }

                // Look for same named nested types
                if (parent.NestedTypes != null) {
                    foreach (var parentNested in parent.NestedTypes) {
                        var isolatedParentNestedName = parentNested.Name?.Split('.').Last();
                        if (isolatedParentNestedName == isolatedNestedName) {
                            nestedGenerator.Update(nestedGenerator.TypeDeclaration.AddModifiers(SyntaxFactory.Token(SyntaxKind.NewKeyword)));
                            addedNew = true;
                            break;
                        }
                    }
                }
            }

            if (nestedGenerator.TypeDeclaration != null) {
                this.Update(this.typeDeclaration.AddMembers(nestedGenerator.TypeDeclaration));
            }
        }

        return typeDeclaration;
    }
}