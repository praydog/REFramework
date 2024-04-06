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


public class ClassGenerator {
    private string className;
    private REFrameworkNET.TypeDefinition t;
    private REFrameworkNET.Method[] methods = [];
    public List<REFrameworkNET.TypeDefinition> usingTypes = [];
    private TypeDeclarationSyntax? typeDeclaration;
    
    public TypeDeclarationSyntax? TypeDeclaration {
        get {
            return typeDeclaration;
        }
    }

    public void Update(TypeDeclarationSyntax? typeDeclaration_) {
        typeDeclaration = typeDeclaration_;
    }

    public ClassGenerator(string className_, REFrameworkNET.TypeDefinition t_, REFrameworkNET.Method[] methods_) {
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

        /*for (var parent = t.ParentType; parent != null; parent = parent.ParentType) {
            var parentName = parent.FullName ?? "";
            if (parentName == null) {
                break;
            }

            if (parentName == "") {
                break;
            }

            if (parentName == "System.Attribute") {
                wantsAbstractInstead = true;
                break;
            }
        }*/

        if (wantsAbstractInstead) {
            typeDeclaration = SyntaxFactory
                        .ClassDeclaration(REFrameworkNET.AssemblyGenerator.CorrectTypeName(className))
                        .AddModifiers(new SyntaxToken[]{SyntaxFactory.Token(SyntaxKind.PublicKeyword), SyntaxFactory.Token(SyntaxKind.AbstractKeyword)});
        } else {
            typeDeclaration = SyntaxFactory
                .InterfaceDeclaration(REFrameworkNET.AssemblyGenerator.CorrectTypeName(className))
                .AddModifiers(new SyntaxToken[]{SyntaxFactory.Token(SyntaxKind.PublicKeyword)});
        }

        if (typeDeclaration == null) {
            return null;
        }

        typeDeclaration = typeDeclaration
            .AddMembers(methods.Where(method => !invalidMethodNames.Contains(method.Name) && !method.Name.Contains('<')).Select(method => {
                TypeSyntax? returnType = SyntaxFactory.PredefinedType(SyntaxFactory.Token(SyntaxKind.VoidKeyword));

                var methodReturnT = method.ReturnType;
                string methodReturnName = methodReturnT != null ? methodReturnT.GetFullName() : "";

                if (methodReturnT != null && methodReturnName != "System.Void" && methodReturnName != "") {
                    // Check for easily convertible types like System.Single, System.Int32, etc.
                    switch (methodReturnName) { 
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
                            if (methodReturnT != null && methodReturnName != "") {
                                if (!REFrameworkNET.AssemblyGenerator.validTypes.Contains(methodReturnName)) {
                                    returnType = SyntaxFactory.PredefinedType(SyntaxFactory.Token(SyntaxKind.ObjectKeyword));
                                    break;
                                }

                                if (methodReturnName.Contains('<') || methodReturnName.Contains('[')) {
                                    returnType = SyntaxFactory.PredefinedType(SyntaxFactory.Token(SyntaxKind.ObjectKeyword));
                                    break;
                                }

                                /*if (methodReturnName.StartsWith("System.") || !methodReturnName.StartsWith("via.")) {
                                    returnType = SyntaxFactory.PredefinedType(SyntaxFactory.Token(SyntaxKind.ObjectKeyword));
                                    break;
                                }*/

                                // Stuff in System should NOT be referencing via
                                // how is this even compiling for them?
                                if (ogClassName.StartsWith("System") && methodReturnName.StartsWith("via")) {
                                    REFrameworkNET.API.LogWarning("Method " + ogClassName + "." + method.Name + " is referencing via class " + methodReturnName);
                                    returnType = SyntaxFactory.PredefinedType(SyntaxFactory.Token(SyntaxKind.ObjectKeyword));
                                    break;
                                }

                                if (ogClassName.StartsWith("System") && methodReturnName.StartsWith("app.")) {
                                    REFrameworkNET.API.LogWarning("Method " + ogClassName + "." + method.Name + " is referencing app class " + methodReturnName);
                                    returnType = SyntaxFactory.PredefinedType(SyntaxFactory.Token(SyntaxKind.ObjectKeyword));
                                    break;
                                }


                                methodReturnName = "global::" + REFrameworkNET.AssemblyGenerator.CorrectTypeName(methodReturnName);

                                returnType = SyntaxFactory.ParseTypeName(methodReturnName);
                                break;
                            }

                            returnType = SyntaxFactory.PredefinedType(SyntaxFactory.Token(SyntaxKind.ObjectKeyword));
                            break;
                    }
                }

                var methodName = new string(method.Name);
                var methodExtension = Il2CppDump.GetMethodExtension(method);

                if (methodExtension != null && methodExtension.Override != null && methodExtension.Override == true) {
                    methodName += "_" + className.Replace('.', '_');
                }

                var methodDeclaration = SyntaxFactory.MethodDeclaration(returnType, methodName ?? "UnknownMethod")
                    .AddModifiers(new SyntaxToken[]{SyntaxFactory.Token(SyntaxKind.PublicKeyword)})
                    /*.AddBodyStatements(SyntaxFactory.ParseStatement("throw new System.NotImplementedException();"))*/;

                if (wantsAbstractInstead) {
                    methodDeclaration = methodDeclaration.AddModifiers(SyntaxFactory.Token(SyntaxKind.AbstractKeyword));
                }

                if (method.Parameters.Count > 0) {
                    methodDeclaration = methodDeclaration.AddParameterListParameters(method.Parameters.Where(param => param != null && param.Type != null && param.Name != null).Select(param => {
                        return SyntaxFactory.Parameter(SyntaxFactory.Identifier(param.Name ?? "UnknownParam"))
                            .WithType(SyntaxFactory.ParseTypeName(/*param.Type ??*/ "object"));
                    }).ToArray());
                }

                return methodDeclaration;
            }).ToArray());

        if (!className.Contains('[') && !className.Contains('<')) {
            List<SimpleBaseTypeSyntax> baseTypes = new List<SimpleBaseTypeSyntax>();

            SortedSet<string> badBaseTypes = new SortedSet<string> {
                /*"System.Object",
                "System.ValueType",
                "System.Enum",
                "System.Delegate",
                "System.MulticastDelegate"*/
            };

            for (var parent = t.ParentType; parent != null; parent = parent.ParentType) {
                // TODO: Fix this
                if (!REFrameworkNET.AssemblyGenerator.validTypes.Contains(parent.FullName)) {
                    continue;
                }

                var parentName = REFrameworkNET.AssemblyGenerator.CorrectTypeName(parent.FullName ?? "");
                if (parentName == null) {
                    break;
                }

                if (parentName == "") {
                    break;
                }

                if (parentName.Contains('[') || parentName.Contains('<')) {
                    break;
                }

                if (badBaseTypes.Contains(parentName)) {
                    break;
                }

                // Forces compiler to start at the global namespace
                parentName = "global::" + parentName;

                baseTypes.Add(SyntaxFactory.SimpleBaseType(SyntaxFactory.ParseTypeName(parentName)));
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

        HashSet<REFrameworkNET.TypeDefinition>? nestedTypes = Il2CppDump.GetTypeExtension(t)?.NestedTypes;

        foreach (var nestedT in nestedTypes ?? []) {
            var nestedTypeName = nestedT.FullName ?? "";

            //System.Console.WriteLine("Nested type: " + nestedTypeName);

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

            if (nestedT.IsEnum()) {
                var nestedEnumGenerator = new EnumGenerator(nestedTypeName.Split('.').Last(), nestedT);
                
                if (nestedEnumGenerator.EnumDeclaration != null) {
                    this.Update(this.typeDeclaration.AddMembers(nestedEnumGenerator.EnumDeclaration));
                }

                continue;
            }

            HashSet<REFrameworkNET.Method> nestedMethods = [];

            foreach (var method in nestedT.Methods) {
                if (!nestedMethods.Select(nestedMethod => nestedMethod.Name).Contains(method.Name)) {
                    if (method.DeclaringType == nestedT) {
                        nestedMethods.Add(method);   
                    }
                }
            }

            var nestedGenerator = new ClassGenerator(nestedTypeName.Split('.').Last(),
                nestedT,
                [.. nestedMethods]
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

                // TODO: Fix this
                if (!REFrameworkNET.AssemblyGenerator.validTypes.Contains(parent.FullName)) {
                    continue;
                }

                var parentNestedTypes = Il2CppDump.GetTypeExtension(parent)?.NestedTypes;

                // Look for same named nested types
                if (parentNestedTypes != null) {
                    foreach (var parentNested in parentNestedTypes) {
                        var isolatedParentNestedName = parentNested.FullName?.Split('.').Last();
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