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
using System;
using System.ComponentModel.DataAnnotations;

using static Microsoft.CodeAnalysis.CSharp.SyntaxFactory;

public class ClassGenerator {
    public class PseudoProperty {
        public REFrameworkNET.Method? getter;
        public REFrameworkNET.Method? setter;
        public REFrameworkNET.TypeDefinition? type;
        public bool indexer = false;
        public REFrameworkNET.TypeDefinition? indexType;
    };

    private Dictionary<string, PseudoProperty> pseudoProperties = [];

    private REFrameworkNET.TypeDefinition t;
    private List<REFrameworkNET.Method> methods = [];
    private List<REFrameworkNET.Field> fields = [];
    private InterfaceDeclarationSyntax typeDeclaration;
    private bool addedNewKeyword = false;
    private bool generic = false;
    

    public TypeDeclarationSyntax? TypeDeclaration {
        get {
            return typeDeclaration;
        }
    }

    public bool AddedNewKeyword {
        get {
            return addedNewKeyword;
        }
    }

    public ClassGenerator(REFrameworkNET.TypeDefinition t_, bool? pGeneric = null) {
        t = t_;
        generic = pGeneric ?? t.IsGenericTypeDefinition();
        typeDeclaration = InterfaceDeclaration("type");


        foreach (var method in t_.Methods) {
            // Means we've entered the parent type
            if (method.DeclaringType != t_) {
                break;
            }
            
            if (method.Name == null) {
                continue;
            }

            if (method.ReturnType == null || method.ReturnType.FullName == null) {
                REFrameworkNET.API.LogError("Method " + method.Name + " has a null return type");
                continue;
            }

            if (method.Name.StartsWith("get_") && method.ReturnType.FullName != "System.Void") {
                if (method.Parameters.Count == 0) {
                    // Add the getter to the pseudo property (create if it doesn't exist)
                    var propertyName = method.Name[4..];
                    if (!pseudoProperties.ContainsKey(propertyName)) {
                        pseudoProperties[propertyName] = new PseudoProperty();
                    }

                    pseudoProperties[propertyName].getter = method;
                    pseudoProperties[propertyName].type = method.ReturnType;
                } else if (method.Parameters.Count == 1 && method.Name == "get_Item") {
                    // This is an indexer property
                    var propertyName = method.Name[4..];
                    if (!pseudoProperties.ContainsKey(propertyName)) {
                        pseudoProperties[propertyName] = new PseudoProperty();
                    }

                    pseudoProperties[propertyName].getter = method;
                    pseudoProperties[propertyName].type = method.ReturnType;
                    pseudoProperties[propertyName].indexer = true;
                    pseudoProperties[propertyName].indexType = method.Parameters[0].Type;
                }
            } else if (method.Name.StartsWith("set_")) {
                if (method.Parameters.Count == 1) {
                    // Add the setter to the pseudo property (create if it doesn't exist)
                    var propertyName = method.Name[4..];
                    if (!pseudoProperties.ContainsKey(propertyName)) {
                        pseudoProperties[propertyName] = new PseudoProperty();
                    }

                    pseudoProperties[propertyName].setter = method;
                    pseudoProperties[propertyName].type = method.Parameters[0].Type;
                } else if (method.Parameters.Count == 2 && method.Name == "set_Item") {
                    // This is an indexer property
                    var propertyName = method.Name[4..];
                    if (!pseudoProperties.ContainsKey(propertyName)) {
                        pseudoProperties[propertyName] = new PseudoProperty();
                    }

                    pseudoProperties[propertyName].setter = method;
                    pseudoProperties[propertyName].type = method.Parameters[1].Type;
                    pseudoProperties[propertyName].indexer = true;
                    pseudoProperties[propertyName].indexType = method.Parameters[0].Type;
                }
            } else {
                methods.Add(method);
            }
        }

        foreach (var field in t_.Fields) {
            // Means we've entered the parent type
            if (field.DeclaringType != t_) {
                break;
            }

            if (field.Name == null) {
                continue;
            }

            if (field.Type == null || field.Type.FullName == null) {
                REFrameworkNET.API.LogError("Field " + field.Name + " has a null field type");
                continue;
            }

            fields.Add(field);

            var fieldName = new string(field.Name);

            if (fieldName.StartsWith("<") && fieldName.EndsWith("k__BackingField")) {
                fieldName = fieldName[1..fieldName.IndexOf(">k__")];
            }

            // remove any methods that start with get/set_{field.Name}
            // because we're going to make them properties instead
            methods.RemoveAll(method => method.Name == "get_" + fieldName || method.Name == "set_" + fieldName);
            pseudoProperties.Remove(fieldName);
        }

        Generate();
    }

       
    static readonly SortedSet<string> invalidMethodNames = [
        "Finalize",
        //"MemberwiseClone",
        //"ToString",
        //"Equals",
        //"GetHashCode",
        //"GetType",
        ".ctor",
        ".cctor",
        /*"op_Implicit",
        "op_Explicit",
        "op_Addition",
        "op_Subtraction",
        "op_Multiply",
        "op_Division",
        "op_Modulus",
        "op_BitwiseAnd",
        "op_BitwiseOr",
        "op_ExclusiveOr",*/
        
    ];

    static string[] GenericNames = ["T", "U", "V", "W", "X", "Y", "Z", "P7", "P8", "P9"];
    private void Generate() {
        var (name, count) = TypeHandler.BaseTypeName(t.Name);
        typeDeclaration = InterfaceDeclaration(name).AddModifiers(Token(SyntaxKind.PublicKeyword));

        if (generic) {
            var arguments = t.GenericArguments ?? [];
            var parentGenericCount = Math.Max(0, arguments.Length - count);
            var argumentList = new List<TypeParameterSyntax>();
            for (int i = parentGenericCount; i < arguments.Length; ++i) {
                argumentList.Add(TypeParameter(GenericNames[i]));
            }
            typeDeclaration = typeDeclaration.AddTypeParameterListParameters([..argumentList]);
        }


        // Check if we need to add the new keyword to this.
        if (AssemblyGenerator.NestedTypeExistsInParent(t)) {
            typeDeclaration = typeDeclaration.AddModifiers(SyntaxFactory.Token(SyntaxKind.NewKeyword));
            addedNewKeyword = true;
        }

        // Set up base types
        BaseTypeSyntax[] baseTypes = TypeHandler.ParentTypes(t);
        typeDeclaration = typeDeclaration.AddBaseListTypes(baseTypes);


        // Add a static field that holds a NativeProxy to the class (for static methods)
        var refProxyVarDecl = VariableDeclaration(TypeHandler.MakeProperType(t))
            .AddVariables(
                VariableDeclarator("REFProxy")
                .WithInitializer(EqualsValueClause(ParseExpression("REFType.As<" + REFrameworkNET.AssemblyGenerator.CorrectTypeName(t.FullName) + ">()"))));

        var refProxyFieldDecl = SyntaxFactory.FieldDeclaration(refProxyVarDecl).AddModifiers(SyntaxFactory.Token(SyntaxKind.PrivateKeyword), SyntaxFactory.Token(SyntaxKind.StaticKeyword), SyntaxFactory.Token(SyntaxKind.ReadOnlyKeyword));


        // Add a static field to the class that holds the REFrameworkNET.TypeDefinition
        var refTypeFieldDecl = ParseMemberDeclaration(
            $"public static readonly global::REFrameworkNET.TypeDefinition REFType = global::REFrameworkNET.TDB.Get().FindType(\"{t.FullName}\");"
        )!;
        if (baseTypes.Length > 0) {
            refTypeFieldDecl = refTypeFieldDecl.AddModifiers(SyntaxFactory.Token(SyntaxKind.NewKeyword));
        }
        typeDeclaration = typeDeclaration.AddMembers(refTypeFieldDecl);
        //typeDeclaration = typeDeclaration.AddMembers(refProxyFieldDecl);

        GenerateMethods();
        GenerateFields();
        GenerateProperties();

        typeDeclaration = typeDeclaration.AddMembers(TypeHandler.GenerateNestedTypes(t));
        if (t.FullName == "System.Array") {
            var decl = GenericArrayType();
            if (decl is not null)
                typeDeclaration = typeDeclaration.AddMembers(decl);
        }

    }

    private StatementSyntax GenericStub(TypeSyntax? returnType, string[] paramNames, int index) {
        var ret = returnType ?? TypeHandler.VoidType();
        var argumentList = string.Join(",", paramNames);
        var stmt = ret switch {
            var t when t.IsEquivalentTo(TypeHandler.VoidType()) =>
                $@"(this as REFrameworkNET.IObject)
                    .GetTypeDefinition()
                    .GetMethods()[{index}]
                    .Invoke(this, [{argumentList}]);",
            _ =>
                $@"return ({ret.ToFullString()})
                    (this as REFrameworkNET.IObject)
                    .GetTypeDefinition()
                    .GetMethods()[{index}]
                    .InvokeBoxed(typeof({ret.ToFullString()}), this, [{argumentList}]);",
        };
        return ParseStatement(stmt);
    }

    private void GenerateProperties() {
        if (pseudoProperties.Count == 0) {
            return;
        }

        List<FieldDeclarationSyntax> internalFieldDeclarations = [];

        var matchingProperties = pseudoProperties
            .Select(property => {
                var propertyType = TypeHandler.MakeProperType(property.Value.type);
                var propertyName = new string(property.Key);

                BasePropertyDeclarationSyntax propertyDeclaration = SyntaxFactory.PropertyDeclaration(propertyType, propertyName)
                    .AddModifiers([SyntaxFactory.Token(SyntaxKind.PublicKeyword)]);

                if (property.Value.indexer) {
                    ParameterSyntax parameter = SyntaxFactory
                        .Parameter(SyntaxFactory.Identifier("index"))
                        .WithType(TypeHandler.MakeProperType(property.Value.indexType));

                    propertyDeclaration = SyntaxFactory.IndexerDeclaration(propertyType)
                        .AddModifiers([SyntaxFactory.Token(SyntaxKind.PublicKeyword)])
                        .AddParameterListParameters(parameter);
                }

                bool shouldAddNewKeyword = false;
                bool shouldAddStaticKeyword = false;

                if (property.Value.getter != null) {
                    var getter = SyntaxFactory.AccessorDeclaration(SyntaxKind.GetAccessorDeclaration)
                        .AddAttributeLists(SyntaxFactory.AttributeList().AddAttributes(SyntaxFactory.Attribute(
                            SyntaxFactory.ParseName("global::REFrameworkNET.Attributes.Method"),
                            SyntaxFactory.ParseAttributeArgumentList("(" + property.Value.getter.Index.ToString() + ", global::REFrameworkNET.FieldFacadeType.None)"))
                        ));

                    if (property.Value.getter.IsStatic()) {
                        shouldAddStaticKeyword = true;

                        // Now we must add a body to it that actually calls the method
                        // We have our REFType field, so we can lookup the method and call it
                        // Make a private static field to hold the REFrameworkNET.Method
                        var internalFieldName = "INTERNAL_" + propertyName + property.Value.getter.Index.ToString();
                        var methodVariableDeclaration = SyntaxFactory.VariableDeclaration(SyntaxFactory.ParseTypeName("global::REFrameworkNET.Method"))
                            .AddVariables(SyntaxFactory.VariableDeclarator(internalFieldName).WithInitializer(SyntaxFactory.EqualsValueClause(SyntaxFactory.ParseExpression("REFType.GetMethod(\"" + property.Value.getter.GetMethodSignature() + "\")"))));

                        var methodFieldDeclaration = SyntaxFactory.FieldDeclaration(methodVariableDeclaration).AddModifiers(SyntaxFactory.Token(SyntaxKind.PrivateKeyword), SyntaxFactory.Token(SyntaxKind.StaticKeyword), SyntaxFactory.Token(SyntaxKind.ReadOnlyKeyword));
                        internalFieldDeclarations.Add(methodFieldDeclaration);

                        List<StatementSyntax> bodyStatements = [];
                        bodyStatements.Add(SyntaxFactory.ParseStatement("return (" + propertyType.GetText().ToString() + ")" + internalFieldName + ".InvokeBoxed(typeof(" + propertyType.GetText().ToString() + "), null, null);"));

                        getter = getter.AddBodyStatements(bodyStatements.ToArray());
                    } else if (generic) { 
                        var index = t.Methods.IndexOf(property.Value.getter);
                        getter = getter
                            .AddBodyStatements(GenericStub(propertyType, [], index))
                            .WithAttributeLists([]);
                    } else {
                        getter = getter.WithSemicolonToken(SyntaxFactory.Token(SyntaxKind.SemicolonToken));
                    }

                    propertyDeclaration = propertyDeclaration.AddAccessorListAccessors(getter);
                    
                    var getterExtension = Il2CppDump.GetMethodExtension(property.Value.getter);
                    if (getterExtension?.MatchingParentMethods.Any() ?? false) {
                        shouldAddNewKeyword = true;
                    }

                }

                if (property.Value.setter != null) {
                    var setter = SyntaxFactory.AccessorDeclaration(SyntaxKind.SetAccessorDeclaration)
                        .AddAttributeLists(SyntaxFactory.AttributeList().AddAttributes(SyntaxFactory.Attribute(
                            SyntaxFactory.ParseName("global::REFrameworkNET.Attributes.Method"),
                            SyntaxFactory.ParseAttributeArgumentList("(" + property.Value.setter.Index.ToString() + ", global::REFrameworkNET.FieldFacadeType.None)"))
                        ));
                    
                    if (property.Value.setter.IsStatic()) {
                        shouldAddStaticKeyword = true;

                        // Now we must add a body to it that actually calls the method
                        // We have our REFType field, so we can lookup the method and call it
                        // Make a private static field to hold the REFrameworkNET.Method
                        var internalFieldName = "INTERNAL_" + propertyName + property.Value.setter.Index.ToString();
                        var methodVariableDeclaration = SyntaxFactory.VariableDeclaration(SyntaxFactory.ParseTypeName("global::REFrameworkNET.Method"))
                            .AddVariables(SyntaxFactory.VariableDeclarator(internalFieldName).WithInitializer(SyntaxFactory.EqualsValueClause(SyntaxFactory.ParseExpression("REFType.GetMethod(\"" + property.Value.setter.GetMethodSignature() + "\")"))));

                        var methodFieldDeclaration = SyntaxFactory.FieldDeclaration(methodVariableDeclaration).AddModifiers(SyntaxFactory.Token(SyntaxKind.PrivateKeyword), SyntaxFactory.Token(SyntaxKind.StaticKeyword), SyntaxFactory.Token(SyntaxKind.ReadOnlyKeyword));
                        internalFieldDeclarations.Add(methodFieldDeclaration);

                        List<StatementSyntax> bodyStatements = [];
                        bodyStatements.Add(SyntaxFactory.ParseStatement(internalFieldName + ".Invoke(null, new object[] {value});"));

                        setter = setter.AddBodyStatements(bodyStatements.ToArray());
                    } else if (t.IsGenericType()) { 
                        var index = t.Methods.IndexOf(property.Value.setter);
                        setter = setter
                            .AddBodyStatements(GenericStub(null, [], index))
                            .WithAttributeLists([]);
                    } else {
                        setter = setter.WithSemicolonToken(SyntaxFactory.Token(SyntaxKind.SemicolonToken));
                    }
                    
                    propertyDeclaration = propertyDeclaration.AddAccessorListAccessors(setter);

                    var setterExtension = Il2CppDump.GetMethodExtension(property.Value.setter);
                    if (setterExtension?.MatchingParentMethods.Any() ?? false) {
                        shouldAddNewKeyword = true;
                    }
                }

                if (shouldAddStaticKeyword) {
                    propertyDeclaration = propertyDeclaration.AddModifiers(SyntaxFactory.Token(SyntaxKind.StaticKeyword));
                }

                if (shouldAddNewKeyword) {
                    propertyDeclaration = propertyDeclaration.AddModifiers(SyntaxFactory.Token(SyntaxKind.NewKeyword));
                }

                return propertyDeclaration;
            })
            .ToArray();

        typeDeclaration = typeDeclaration
            .AddMembers([..internalFieldDeclarations])
            .AddMembers(matchingProperties);
    }

    private void GenerateFields() {
        if (fields.Count == 0) {
            return;
        }

        List<REFrameworkNET.Field> validFields = [];

        int totalFields = 0;

        foreach (var field in fields) {
            if (field == null) {
                continue;
            }

            if (field.Name == null) {
                continue;
            }

            if (field.Type == null) {
                continue;
            }

            if (field.Type.FullName == null) {
                continue;
            }

            if (field.Type.FullName.Contains('!')) {
                continue;
            }

            // Make sure field name only contains ASCII characters
            if (field.Name.Any(c => c > 127)) {
                System.Console.WriteLine("Skipping field with non-ASCII characters: " + field.Name + " " + field.Index);
                continue;
            }
            
            ++totalFields;

            validFields.Add(field);

            // Some kind of limitation in the runtime prevents too many methods in the class
            if (totalFields >= (ushort.MaxValue - 15) / 2) {
                System.Console.WriteLine("Skipping fields in " + t.FullName + " because it has too many fields (" + fields.Count + ")");
                break;
            }
        }
        List<FieldDeclarationSyntax> internalFieldDeclarations = [];
        var matchingFields = validFields
            .Select(field => {
                var fieldType = TypeHandler.MakeProperType(field.Type);
                var fieldName = new string(field.Name);

                // Replace the k backingfield crap
                if (fieldName.StartsWith("<") && fieldName.EndsWith("k__BackingField")) {
                    fieldName = fieldName[1..fieldName.IndexOf(">k__")];
                }

                // So this is actually going to be made a property with get/set instead of an actual field
                // 1. Because interfaces can't have fields
                // 2. Because we don't actually have a concrete reference to the field in our VM, so we'll be a facade for the field
                var fieldFacadeGetter = SyntaxFactory.AttributeList().AddAttributes(SyntaxFactory.Attribute(
                    SyntaxFactory.ParseName("global::REFrameworkNET.Attributes.Method"),
                    SyntaxFactory.ParseAttributeArgumentList("(" + field.Index.ToString() + ", global::REFrameworkNET.FieldFacadeType.Getter)"))
                );

                var fieldFacadeSetter = SyntaxFactory.AttributeList().AddAttributes(SyntaxFactory.Attribute(
                    SyntaxFactory.ParseName("global::REFrameworkNET.Attributes.Method"),
                    SyntaxFactory.ParseAttributeArgumentList("(" + field.Index.ToString() + ", global::REFrameworkNET.FieldFacadeType.Setter)"))
                );

                AccessorDeclarationSyntax getter = SyntaxFactory.AccessorDeclaration(SyntaxKind.GetAccessorDeclaration).AddAttributeLists(fieldFacadeGetter);
                AccessorDeclarationSyntax setter = SyntaxFactory.AccessorDeclaration(SyntaxKind.SetAccessorDeclaration).AddAttributeLists(fieldFacadeSetter);

                var propertyDeclaration = SyntaxFactory.PropertyDeclaration(fieldType, fieldName)
                    .AddModifiers([SyntaxFactory.Token(SyntaxKind.PublicKeyword)]);

                if (field.IsStatic()) {
                    propertyDeclaration = propertyDeclaration.AddModifiers(SyntaxFactory.Token(SyntaxKind.StaticKeyword));

                    // Now we must add a body to it that actually calls the method
                    // We have our REFType field, so we can lookup the method and call it
                    // Make a private static field to hold the REFrameworkNET.Method
                    var internalFieldName = "INTERNAL_" + fieldName + field.GetIndex().ToString();
                    var fieldVariableDeclaration = SyntaxFactory.VariableDeclaration(SyntaxFactory.ParseTypeName("global::REFrameworkNET.Field"))
                        .AddVariables(SyntaxFactory.VariableDeclarator(internalFieldName).WithInitializer(SyntaxFactory.EqualsValueClause(SyntaxFactory.ParseExpression("REFType.GetField(\"" + field.GetName() + "\")"))));

                    var fieldDeclaration = SyntaxFactory.FieldDeclaration(fieldVariableDeclaration).AddModifiers(SyntaxFactory.Token(SyntaxKind.PrivateKeyword), SyntaxFactory.Token(SyntaxKind.StaticKeyword), SyntaxFactory.Token(SyntaxKind.ReadOnlyKeyword));
                    internalFieldDeclarations.Add(fieldDeclaration);

                    List<StatementSyntax> bodyStatementsSetter = [];
                    List<StatementSyntax> bodyStatementsGetter = [];


                    bodyStatementsGetter.Add(SyntaxFactory.ParseStatement("return (" + fieldType.GetText().ToString() + ")" + internalFieldName + ".GetDataBoxed(typeof(" + fieldType.GetText().ToString() + "), 0, false);"));
                    bodyStatementsSetter.Add(SyntaxFactory.ParseStatement(internalFieldName + ".SetDataBoxed(0, new object[] {value}, false);"));

                    getter = getter.AddBodyStatements(bodyStatementsGetter.ToArray());
                    setter = setter.AddBodyStatements(bodyStatementsSetter.ToArray());
                } else if (t.IsGenericType())  {
                    getter = getter
                        .AddBodyStatements(ParseStatement("throw new System.NotImplementedException();"))
                        .WithAttributeLists([]);
                    setter = setter
                        .AddBodyStatements(ParseStatement("throw new System.NotImplementedException();"))
                        .WithAttributeLists([]);
                } else {
                    getter = getter.WithSemicolonToken(SyntaxFactory.Token(SyntaxKind.SemicolonToken));
                    setter = setter.WithSemicolonToken(SyntaxFactory.Token(SyntaxKind.SemicolonToken));
                }

                propertyDeclaration = propertyDeclaration.AddAccessorListAccessors(getter, setter);

                // Search for k__BackingField version and the corrected version
                if (this.t.ParentType != null) {
                    var matchingField = this.t.ParentType.FindField(fieldName);
                    matchingField ??= this.t.ParentType.FindField(field.Name);
                    var matchingMethod = this.t.ParentType.FindMethod("get_" + fieldName);
                    matchingMethod ??= this.t.ParentType.FindMethod("set_" + fieldName);
                    if (matchingMethod?.GetMatchingParentMethods().Any() ?? false) {
                        propertyDeclaration = propertyDeclaration.AddModifiers(SyntaxFactory.Token(SyntaxKind.NewKeyword));
                    }
                }
                return propertyDeclaration;
            })
            .ToArray();

        typeDeclaration = typeDeclaration
            .AddMembers([..internalFieldDeclarations])
            .AddMembers(matchingFields);
    }

    private static readonly Dictionary<string, SyntaxToken> operatorTokens = new() {
        ["op_Addition"] = SyntaxFactory.Token(SyntaxKind.PlusToken),
        ["op_UnaryPlus"] = SyntaxFactory.Token(SyntaxKind.PlusToken),
        ["op_Subtraction"] = SyntaxFactory.Token(SyntaxKind.MinusToken),
        ["op_UnaryNegation"] = SyntaxFactory.Token(SyntaxKind.MinusToken),
        ["op_Multiply"] = SyntaxFactory.Token(SyntaxKind.AsteriskToken),
        ["op_Division"] = SyntaxFactory.Token(SyntaxKind.SlashToken),
        ["op_Modulus"] = SyntaxFactory.Token(SyntaxKind.PercentToken),
        ["op_BitwiseAnd"] = SyntaxFactory.Token(SyntaxKind.AmpersandToken),
        ["op_BitwiseOr"] = SyntaxFactory.Token(SyntaxKind.BarToken),
        ["op_ExclusiveOr"] = SyntaxFactory.Token(SyntaxKind.CaretToken),
        ["op_LeftShift"] = SyntaxFactory.Token(SyntaxKind.LessThanLessThanToken),
        ["op_RightShift"] = SyntaxFactory.Token(SyntaxKind.GreaterThanGreaterThanToken),
        ["op_Equality"] = SyntaxFactory.Token(SyntaxKind.EqualsEqualsToken),
        ["op_Inequality"] = SyntaxFactory.Token(SyntaxKind.ExclamationEqualsToken),
        ["op_LessThan"] = SyntaxFactory.Token(SyntaxKind.LessThanToken),
        ["op_LessThanOrEqual"] = SyntaxFactory.Token(SyntaxKind.LessThanEqualsToken),
        ["op_GreaterThan"] = SyntaxFactory.Token(SyntaxKind.GreaterThanToken),
        ["op_GreaterThanOrEqual"] = SyntaxFactory.Token(SyntaxKind.GreaterThanEqualsToken),
        ["op_LogicalNot"] = SyntaxFactory.Token(SyntaxKind.ExclamationToken),
        ["op_OnesComplement"] = SyntaxFactory.Token(SyntaxKind.TildeToken),
        ["op_True"] = SyntaxFactory.Token(SyntaxKind.TrueKeyword),
        ["op_False"] = SyntaxFactory.Token(SyntaxKind.FalseKeyword),
        ["op_Implicit"] = SyntaxFactory.Token(SyntaxKind.ImplicitKeyword),
        ["op_Explicit"] = SyntaxFactory.Token(SyntaxKind.ExplicitKeyword),
    };

    private void GenerateMethods() {
        if (methods.Count == 0) return;

        HashSet<string> seenMethodSignatures = [];

        List<REFrameworkNET.Method> validMethods = [];
        List<FieldDeclarationSyntax> internalFieldDeclarations = [];

        try {
            foreach(REFrameworkNET.Method m in methods) {
                if (m == null) {
                    continue;
                }

                if (invalidMethodNames.Contains(m.Name)) {
                    continue;
                }

                if (m.Name.Contains('<')) {
                    continue;
                }

                validMethods.Add(m);
            }
        } catch (Exception e) {
            Console.WriteLine("ASDF Error: " + e.Message);
        }

        var matchingMethods = validMethods
            .Select(method => 
        {

            var returnType = TypeHandler.MakeProperType(method.ReturnType);
            

            //string simpleMethodSignature = returnType.GetText().ToString();
            string simpleMethodSignature = ""; // Return types are not part of the signature. Return types are not overloaded.

            var methodName = new string(method.Name);
            if (methodName.StartsWith("System."))
                methodName = "_" + methodName;
            var methodExtension = Il2CppDump.GetMethodExtension(method);

            // Hacky fix for MHR because parent classes have the same method names
            // while we support that, we don't support constructed generic arguments yet, they are just "object"
            if (methodName == "sortCountList") {
                Console.WriteLine("Skipping sortCountList");
                return null;
            }

            var methodDeclaration = MethodDeclaration(returnType, methodName ?? "UnknownMethod").AddModifiers(Token(SyntaxKind.PublicKeyword))
                /*.AddBodyStatements(SyntaxFactory.ParseStatement("throw new System.NotImplementedException();"))*/;

            if (operatorTokens.ContainsKey(methodName ?? "UnknownMethod")) {
                // Add SpecialName attribute to the method
                methodDeclaration = methodDeclaration.AddAttributeLists(
                    SyntaxFactory.AttributeList().AddAttributes(SyntaxFactory.Attribute(
                        SyntaxFactory.ParseName("global::System.Runtime.CompilerServices.SpecialName"))
                    )
                );
            }

            simpleMethodSignature += methodName;

            // Add full method name as a MethodName attribute to the method
            methodDeclaration = methodDeclaration.AddAttributeLists(
                SyntaxFactory.AttributeList().AddAttributes(SyntaxFactory.Attribute(
                    SyntaxFactory.ParseName("global::REFrameworkNET.Attributes.Method"),
                    SyntaxFactory.ParseAttributeArgumentList("(" + method.GetIndex().ToString() + ", global::REFrameworkNET.FieldFacadeType.None)")))
                );

            bool anyOutParams = false;
            System.Collections.Generic.List<string> paramNames = [];

            if (method.Parameters.Count > 0) {
                var runtimeMethod = method.GetRuntimeMethod();
                
                if (runtimeMethod == null) {
                    REFrameworkNET.API.LogWarning("Method " + method.DeclaringType.FullName + "." + method.Name + " has a null runtime method");
                    return null;
                }

                var runtimeParams = runtimeMethod.Call("GetParameters") as REFrameworkNET.ManagedObject;
                if (runtimeParams is null)  {
                    return null;
                }
                System.Collections.Generic.List<ParameterSyntax> parameters = [];

                bool anyUnsafeParams = false;


                var methodActualRetval = method.GetReturnType();
                UInt32 unknownArgCount = 0;

                foreach (dynamic param in runtimeParams) {
                    /*if (param.get_IsRetval() == true) {
                        continue;
                    }*/

                    var paramDef = (REFrameworkNET.TypeDefinition)param.GetTypeDefinition();
                    var paramName = param.get_Name();

                    if (paramName == null || paramName == "") {
                        //paramName = "UnknownParam";
                        paramName = "arg" + unknownArgCount.ToString();
                        ++unknownArgCount;
                    }

                    if (paramName == "object") {
                        paramName = "object_"; // object is a reserved keyword.
                    }

                    var paramType = param.get_ParameterType();

                    if (paramType == null) {
                        paramNames.Add(paramName);
                        parameters.Add(SyntaxFactory.Parameter(SyntaxFactory.Identifier(paramName)).WithType(SyntaxFactory.ParseTypeName("object")));
                        continue;
                    }

                    var parsedParamName = new string(paramName as string);

                    /*if (param.get_IsGenericParameter() == true) {
                        return null; // no generic parameters.
                    }*/

                    var isByRef = paramType.IsByRefImpl();
                    var isPointer = paramType.IsPointerImpl();
                    var isOut = paramDef != null && paramDef.FindMethod("get_IsOut") != null ? param.get_IsOut() : false;
                    var paramTypeDef = (REFrameworkNET.TypeDefinition)paramType.get_TypeHandle();

                    var paramTypeSyntax = TypeHandler.MakeProperType(paramTypeDef);

                    System.Collections.Generic.List<SyntaxToken> modifiers = [];

                    if (isOut == true) {
                        simpleMethodSignature += "out";
                        modifiers.Add(SyntaxFactory.Token(SyntaxKind.OutKeyword));
                        anyOutParams = true;
                    }

                    if (isByRef == true) {
                        // can only be either ref or out.
                        if (!isOut) {
                            simpleMethodSignature += "ref " + paramTypeSyntax.GetText().ToString();
                            modifiers.Add(SyntaxFactory.Token(SyntaxKind.RefKeyword));
                        }

                        parameters.Add(SyntaxFactory.Parameter(SyntaxFactory.Identifier(paramName)).WithType(SyntaxFactory.ParseTypeName(paramTypeSyntax.ToString())).AddModifiers(modifiers.ToArray()));
                    } else if (isPointer == true) {
                        simpleMethodSignature += "ptr " + paramTypeSyntax.GetText().ToString();
                        parameters.Add(SyntaxFactory.Parameter(SyntaxFactory.Identifier(paramName)).WithType(SyntaxFactory.ParseTypeName(paramTypeSyntax.ToString() + "*")).AddModifiers(modifiers.ToArray()));
                        anyUnsafeParams = true;

                        parsedParamName = "(global::System.IntPtr) " + parsedParamName;
                    } else {
                        simpleMethodSignature += paramTypeSyntax.GetText().ToString();
                        parameters.Add(SyntaxFactory.Parameter(SyntaxFactory.Identifier(paramName)).WithType(paramTypeSyntax).AddModifiers(modifiers.ToArray()));
                    }

                    paramNames.Add(parsedParamName);
                }

                methodDeclaration = methodDeclaration.AddParameterListParameters([.. parameters]);

                if (anyUnsafeParams) {
                    methodDeclaration = methodDeclaration.AddModifiers(SyntaxFactory.Token(SyntaxKind.UnsafeKeyword));
                }

            } else {
                simpleMethodSignature += "()";
            }

            if (method.IsStatic()) {
                // lets see what happens if we just make it static
                methodDeclaration = methodDeclaration.AddModifiers(SyntaxFactory.Token(SyntaxKind.StaticKeyword));

                // Now we must add a body to it that actually calls the method
                // We have our REFType field, so we can lookup the method and call it
                // Make a private static field to hold the REFrameworkNET.Method
                var internalFieldName = "INTERNAL_" + method.Name + method.GetIndex().ToString();
                var methodVariableDeclaration = SyntaxFactory.VariableDeclaration(SyntaxFactory.ParseTypeName("global::REFrameworkNET.Method"))
                    .AddVariables(SyntaxFactory.VariableDeclarator(internalFieldName).WithInitializer(SyntaxFactory.EqualsValueClause(SyntaxFactory.ParseExpression("REFType.GetMethod(\"" + method.GetMethodSignature() + "\")"))));

                var methodFieldDeclaration = SyntaxFactory.FieldDeclaration(methodVariableDeclaration).AddModifiers(SyntaxFactory.Token(SyntaxKind.PrivateKeyword), SyntaxFactory.Token(SyntaxKind.StaticKeyword), SyntaxFactory.Token(SyntaxKind.ReadOnlyKeyword));
                internalFieldDeclarations.Add(methodFieldDeclaration);

                List<StatementSyntax> bodyStatements = [];

                if (method.ReturnType.FullName == "System.Void") {
                    if (method.Parameters.Count == 0) {
                        bodyStatements.Add(SyntaxFactory.ParseStatement(internalFieldName + ".Invoke(null, null);"));
                    } else if (!anyOutParams) {
                        bodyStatements.Add(SyntaxFactory.ParseStatement(internalFieldName + ".Invoke(null, new object[] {" + string.Join(", ", paramNames) + "});"));
                    } else {
                        bodyStatements.Add(SyntaxFactory.ParseStatement("throw new System.NotImplementedException();")); // TODO: Implement this
                    }
                } else {
                    if (method.Parameters.Count == 0) {
                        bodyStatements.Add(SyntaxFactory.ParseStatement("return (" + returnType.GetText().ToString() + ")" + internalFieldName + ".InvokeBoxed(typeof(" + returnType.GetText().ToString() + "), null, null);"));
                    } else if (!anyOutParams) {
                        bodyStatements.Add(SyntaxFactory.ParseStatement("return (" + returnType.GetText().ToString() + ")" + internalFieldName + ".InvokeBoxed(typeof(" + returnType.GetText().ToString() + "), null, new object[] {" + string.Join(", ", paramNames) + "});"));
                    } else {
                        bodyStatements.Add(SyntaxFactory.ParseStatement("throw new System.NotImplementedException();")); // TODO: Implement this
                    }
                }

                methodDeclaration = methodDeclaration.AddBodyStatements(
                    [.. bodyStatements]
                );
            } else if (t.IsGenericType()) {
                var index = t.Methods.IndexOf(method);
                methodDeclaration = methodDeclaration
                    .AddBodyStatements(GenericStub(returnType, [.. paramNames], index))
                    .WithAttributeLists([]);
            } else {
                methodDeclaration = methodDeclaration.WithSemicolonToken(Token(SyntaxKind.SemicolonToken));
            }

            if (seenMethodSignatures.Contains(simpleMethodSignature)) {
                Console.WriteLine("Skipping duplicate method: " + methodDeclaration.NormalizeWhitespace().GetText().ToString());
                return null;
            }

            seenMethodSignatures.Add(simpleMethodSignature);

            if (methodExtension?.MatchingParentMethods.Any() ?? false) {
                methodDeclaration = methodDeclaration.AddModifiers(Token(SyntaxKind.NewKeyword));
            }


            return methodDeclaration;
        })
        .Where(method => method != null)
        .Select(method => method!)
        .ToArray();

        typeDeclaration = typeDeclaration
            .AddMembers([..internalFieldDeclarations])
            .AddMembers(matchingMethods);
    }

    private static InterfaceDeclarationSyntax? GenericArrayType() {
        var array_generic = TDB.Get().GetType("!0[]");
        if (array_generic is null) return null;

        var decl = new ClassGenerator(array_generic).typeDeclaration;
        return decl
            .WithIdentifier(Identifier("Impl"))
            .WithTypeParameterList(TypeParameterList(SingletonSeparatedList(TypeParameter("T"))));
    }


}

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
        if (baseName.Split('`').ToArray() is [var name, var count])
            return (name, int.Parse(count));
        return (baseName, 0);

    }

    public static BaseTypeSyntax[] ParentTypes(REFrameworkNET.TypeDefinition type) {
        List<BaseTypeSyntax> parents = new();
        var parentType = type.ParentType;
        while (parentType != null) {
            if (parentType.Name == "") break;
            if (parentType.FullName == "System.Object") {
                parents.Insert(0, SimpleBaseType(ParseTypeName("global::_System.Object")));
                break;
            }
            var baseType = SimpleBaseType(MakeProperType(parentType));
            parents.Insert(0, baseType);
            parentType = parentType.ParentType;
        }
        return parents.ToArray();
    }

    public static TypeSyntax MakeProperType(REFrameworkNET.TypeDefinition? targetType) {

        if (targetType is null) return VoidType();
        if (targetType.Name.StartsWith("<")) return ObjType();
        if (targetType.Name.StartsWith("!!")) return ObjType();

        if (Predefined.ContainsKey(targetType.FullName))
            return Predefined[targetType.FullName];
        if (Cache.ContainsKey(targetType.Index))
            return Cache[targetType.Index];

        if (targetType.GetElementType() is TypeDefinition elemType) {
            var elem = MakeProperType(elemType);
            var arraySyntax = QualifiedName(
                    ParseName("global::_System.Array"),
                    GenericName("Impl")
                        .AddTypeArgumentListArguments([elem])
            );
            Cache[targetType.Index] = arraySyntax;
            return arraySyntax;
        }
        Cache[targetType.Index] = ObjType();

        var typeList = new List<string>();
        {
            var type = targetType!;
            while (true) {
                typeList.Insert(0, type.Name ?? "UNKN");
                if (type.DeclaringType is null || type.DeclaringType == type)
                    break;
                type = type.DeclaringType;
            }
            if (type.Namespace is not null && type.Namespace.Any()) {
                typeList.Insert(0, type.Namespace);
            } else {
                typeList.Insert(0, "_");
            }
        }

        int genericIndex = 0;
        var generics = targetType.GenericArguments ?? [];
        var toParse = string.Join(".", typeList.Select(tName => {
            var (name, count) = BaseTypeName(tName);
            if (count == 0) return name;
            name += "<";
            for (int i = 0; i < count; ++i) {
                if (i > 0) name += ",";
                if (i + genericIndex >= generics.Length) {
                    name += "UNKN";
                    continue;
                }
                name += MakeProperType(generics[i + genericIndex]).ToFullString();
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

    public static MemberDeclarationSyntax? GenerateType(TypeDefinition t) {

        if (t.Name == "") return null;
        if (t.FullName.EndsWith("[]")) return null;
        if (t.Name.StartsWith("<")) return null;
        if (t.IsGenericType() && !t.IsGenericTypeDefinition()) return null;

        // Enum
        if (t.IsEnum()) {
            var (baseName, _) = TypeHandler.BaseTypeName(t.Name);
            var nestedEnumGenerator = new EnumGenerator(baseName, t);
            return nestedEnumGenerator.EnumDeclaration;
        }
        var nestedGenerator = new ClassGenerator(t);
        return nestedGenerator.TypeDeclaration;
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

