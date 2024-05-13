#nullable enable

using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using System.Reflection;
using System.Collections.Generic;
using Microsoft.CodeAnalysis.Emit;
using System.IO;
using System.Text.Json;
using System.Text.RegularExpressions;
using System.Text.Json.Serialization;
using System;
using System.Linq;
using System.Threading.Tasks;
using System.Collections.Concurrent;
using System.Reflection.Metadata;

public class Il2CppDump {
    class Field {

    };

    public class Method {
        public REFrameworkNET.Method Impl;

        public Method(REFrameworkNET.Method impl) {
            this.Impl = impl;
        }
        
        public REFrameworkNET.TypeDefinition DeclaringType => Impl.GetDeclaringType();

        public bool? Override { get; set;} // Not from JSON
        public List<REFrameworkNET.Method> MatchingParentMethods = [];
    }

    public class Type {
        private REFrameworkNET.TypeDefinition impl;

        public Type(REFrameworkNET.TypeDefinition impl) {
            this.impl = impl;
        }

        public REFrameworkNET.ManagedObject RuntimeType => impl.GetRuntimeType();

        public REFrameworkNET.TypeInfo TypeInfo => impl.GetTypeInfo();

        public REFrameworkNET.TypeDefinition UnderlyingType => impl.GetUnderlyingType();

        public REFrameworkNET.TypeDefinition DeclaringType => impl.GetDeclaringType();

        public REFrameworkNET.TypeDefinition ParentType => impl.GetParentType();

        public List<REFrameworkNET.Property> Properties => impl.GetProperties();

        public List<REFrameworkNET.Field> Fields => impl.GetFields();

        public List<REFrameworkNET.Method> Methods => impl.GetMethods();

        public string Name => impl.GetName();
        public string FullName => impl.GetFullName();

        public string Namespace => impl.GetNamespace();

        public uint FQN => impl.GetFQN();

        public uint ValueTypeSize => impl.GetValueTypeSize();

        public uint Size => impl.GetSize();

        public uint Index => impl.GetIndex();


        // Custom stuff below
        public HashSet<REFrameworkNET.TypeDefinition> NestedTypes = [];
    };

    static private Dictionary<REFrameworkNET.TypeDefinition, Type> typeExtensions = [];
    static private Dictionary<REFrameworkNET.Method, Method> methodExtensions = [];
    static public Type? GetTypeExtension(REFrameworkNET.TypeDefinition type) {
        if (typeExtensions.TryGetValue(type, out Type? value)) {
            return value;
        }

        return null;
    }

    static public Type GetOrAddTypeExtension(REFrameworkNET.TypeDefinition type) {
        if (typeExtensions.TryGetValue(type, out Type? value)) {
            return value;
        }

        value = new Type(type);
        typeExtensions[type] = value;

        return value;
    }

    static public Method? GetMethodExtension(REFrameworkNET.Method method) {
        if (methodExtensions.TryGetValue(method, out Method? value)) {
            return value;
        }

        return null;
    }

    public static void FillTypeExtensions(REFrameworkNET.TDB context) {
        if (typeExtensions.Count > 0) {
            return;
        }

        context.GetType(0).GetFullName(); // initialize the types

        //Parallel.For(0, context.GetNumTypes(), i =>
        foreach (REFrameworkNET.TypeDefinition t in context.Types) {
            //var t = context.GetType((uint)i);
            if (t == null) {
                //Console.WriteLine("Failed to get type " + i);
                continue;
            }

            var tDeclaringType = t.DeclaringType;
            if (tDeclaringType != null) {
                var ext = GetOrAddTypeExtension(tDeclaringType);
                ext.NestedTypes.Add(t);
            }

            if (t.GetNumMethods() == 0 || t.ParentType == null) {
                continue;
            }

            // Look for methods with the same name and mark them as overrides
            // We dont go through all parents, because GetMethod does that for us
            // Going through all parents would exponentially increase the number of checks and they would be redundant
            var parent = t.ParentType;
            var tMethods = t.GetMethods();

            //foreach (var method in t.Methods) {
            //Parallel.ForEach(tMethods, method => {
            foreach (REFrameworkNET.Method method in tMethods) { // parallel isn't necessary here because there arent many methods
                if (method == null) {
                    continue;
                }

                if (GetMethodExtension(method) != null) {
                    continue;
                }

                if (method.DeclaringType != t) {
                    continue;
                }

                /*var parentMethod = parent.GetMethod(method.Name);

                if (parentMethod != null) {
                    methodExtensions.Add(method, new Method(method) {
                        Override = true
                    });
                }*/

                var matchingParentMethods = method.GetMatchingParentMethods();

                if (matchingParentMethods.Count > 0) {
                    methodExtensions.Add(method, new Method(method) {
                        Override = true,
                        MatchingParentMethods = matchingParentMethods
                    });
                }
            }
        }   
    }
}

namespace REFrameworkNET {
public class AssemblyGenerator {
    static Dictionary<string, NamespaceDeclarationSyntax> namespaces = [];

    // Start with an empty CompilationUnitSyntax (represents an empty file)
    //static CompilationUnitSyntax compilationUnit = SyntaxFactory.CompilationUnit();

    static readonly char[] invalidChars = [
        '<',
        '>',
        ',',
        '!',
        ' ',
    ];

    static readonly char[] invalidGenericChars = [
        '<',
        '>',
        ',',
        '!',
        ' ',
        '`',
    ];

    public static string FixBadChars(string name) {
        // Find the first <, and the last >, replace any dots in between with underscores
        /*int first = name.IndexOf('<');
        int last = name.LastIndexOf('>');

        if (first != -1 && last != -1) {
            name = name.Substring(0, first) + name.Substring(first, last - first).Replace('.', '_') + name.Substring(last);
        }

        // Replace any invalid characters with underscores
        foreach (var c in invalidGenericChars) {
            name = name.Replace(c, '_');
        }*/

        return name;
    }

    /*public static string FixBadCharsForGeneric(string name) {
        // Find the first <, and the last >, replace any dots in between with underscores
        int first = name.IndexOf('<');
        int last = name.LastIndexOf('>');

        if (first != -1 && last != -1) {
            name = name.Substring(0, first) + name.Substring(first, last - first).Replace('.', '_') + name.Substring(last);
        }

        // Replace any invalid characters with underscores
        foreach (var c in invalidGenericChars) {
            name = name.Replace(c, '_');
        }

        return name;
    }*/

    public static string CorrectTypeName(string fullName) {
        if (fullName.StartsWith("System.") || fullName.StartsWith("Internal.")) {
            return "_" + fullName;
        }

        return FixBadChars(fullName);
    }

    static public NamespaceDeclarationSyntax? ExtractNamespaceFromType(REFrameworkNET.TypeDefinition t) {
        var ns = t.GetNamespace();

        if (ns != null && ns.Length > 0) {
            if (ns.StartsWith("System.") || ns == "System" || ns.StartsWith("Internal.") || ns == "Internal") {
                ns = "_" + ns;
            }

            if (!namespaces.TryGetValue(ns, out NamespaceDeclarationSyntax? value)) {
                //ns = Regex.Replace(ns, @"[^a-zA-Z0-9.]", "_");
                Console.WriteLine("Creating namespace " + ns);
                value = SyntaxTreeBuilder.CreateNamespace(ns);
                namespaces[ns] = value;
            }

            return value;
        } 

        //Console.WriteLine("Failed to extract namespace from " + t.GetFullName());
        if (!namespaces.TryGetValue("_", out NamespaceDeclarationSyntax? value2)) {
            value2 = SyntaxTreeBuilder.CreateNamespace("_");
            namespaces["_"] = value2;
        }

        return value2;
    }

    public static SortedSet<string> validTypes = [];
    public static SortedSet<string> generatedTypes = [];

    // Array of System.Array derived types
    public static List<REFrameworkNET.TypeDefinition> arrayTypes = [];
    public static HashSet<REFrameworkNET.TypeDefinition> typesWithArrayTypes = [];
    private static Dictionary<REFrameworkNET.TypeDefinition, List<REFrameworkNET.TypeDefinition>> elementTypesToArrayTypes = [];

    public static Dictionary<REFrameworkNET.TypeDefinition, string> typeRenames = [];
    public static Dictionary<REFrameworkNET.TypeDefinition, string> typeFullRenames = [];
    public static readonly REFrameworkNET.TypeDefinition SystemArrayT = REFrameworkNET.API.GetTDB().GetType("System.Array");

    public static void ForEachArrayType(
        TypeDefinition elementType, Action<TypeDefinition> action, 
        HashSet<REFrameworkNET.TypeDefinition>? visited = null,
        HashSet<REFrameworkNET.TypeDefinition>? visitedArrayTypes = null
    ) 
    {
        if (visited == null) {
            visited = new HashSet<REFrameworkNET.TypeDefinition>();
        }

        if (visitedArrayTypes == null) {
            visitedArrayTypes = new HashSet<REFrameworkNET.TypeDefinition>();
        }

        if (visited.Contains(elementType)) {
            return;
        }

        visited.Add(elementType);

        if (!elementTypesToArrayTypes.TryGetValue(elementType, out List<REFrameworkNET.TypeDefinition>? arrayTypes)) {
            return;
        }

        foreach (var arrayType in arrayTypes) {
            if (visitedArrayTypes.Contains(arrayType)) {
                continue;
            }

            action(arrayType);
            visitedArrayTypes.Add(arrayType);

            // Array types can have array types themselves.
            ForEachArrayType(arrayType, action, visited);
        }
    }

    public static REFrameworkNET.TypeDefinition? GetEquivalentNestedTypeInParent(REFrameworkNET.TypeDefinition nestedT) {
        var isolatedNestedName = nestedT.FullName?.Split('.').Last();

        if (nestedT.DeclaringType == null && nestedT.IsDerivedFrom(SystemArrayT)) {
            // Types derived from System.Array do not have a proper declaring type
            // so we need to get the element type and find the declaring type of that
            TypeDefinition? elementType = nestedT.GetElementType();

            while (elementType != null && elementType.IsDerivedFrom(SystemArrayT)) {
                elementType = elementType.GetElementType();
            }

            if (elementType != null) {
                var equivalentElementType = GetEquivalentNestedTypeInParent(elementType);

                if (equivalentElementType != null) {
                    // Now go through all possible array types for that equivalent type
                    TypeDefinition? equivalentArray = null;
                    ForEachArrayType(equivalentElementType, (arrayType) => {
                        if (equivalentArray != null) {
                            return;
                        }

                        var isolatedArrayTypeName = arrayType.FullName?.Split('.').Last();

                        if (isolatedArrayTypeName == isolatedNestedName) {
                            System.Console.WriteLine("Found equivalent array type for " + nestedT.FullName);
                            equivalentArray = arrayType;
                            return;
                        }
                    });
                }
            }
        }

        var t = nestedT.DeclaringType;

        if (t == null) {
            return null;
        }

        // Add the "new" keyword if this nested type is anywhere in the hierarchy
        for (var parent = t.ParentType; parent != null; parent = parent.ParentType) {
            // TODO: Fix this
            if (!validTypes.Contains(parent.FullName)) {
                continue;
            }

            var parentNestedTypes = Il2CppDump.GetTypeExtension(parent)?.NestedTypes;

            // Look for same named nested types
            if (parentNestedTypes != null) {
                foreach (var parentNested in parentNestedTypes) {
                    var isolatedParentNestedName = parentNested.FullName?.Split('.').Last();
                    if (isolatedParentNestedName == isolatedNestedName) {
                        return parentNested;
                    }
                }
            }
        }

        return null;
    }

    public static bool NestedTypeExistsInParent(REFrameworkNET.TypeDefinition nestedT) {
        return GetEquivalentNestedTypeInParent(nestedT) != null;
    }

    private static string? GetOptionalPrefix(REFrameworkNET.TypeDefinition t) {
        if (t.Namespace == null || t.Namespace.Length == 0) {
            if (t.DeclaringType == null) {
                return "_.";
            } else {
                var lastDeclaringType = t.DeclaringType;

                while (lastDeclaringType.DeclaringType != null) {
                    lastDeclaringType = lastDeclaringType.DeclaringType;
                }

                if (lastDeclaringType.Namespace == null || lastDeclaringType.Namespace.Length == 0) {
                    return "_.";
                }
            }
        }

        return null;
    }

    private static string MakePrefixedTypeName(REFrameworkNET.TypeDefinition t) {
        var prefix = GetOptionalPrefix(t);

        if (prefix != null) {
            return prefix + t.GetFullName();
        }

        return t.GetFullName();
    }
    
    private static bool HandleArrayType(REFrameworkNET.TypeDefinition t) {
        var elementTypeDef = t.GetElementType();

        if (elementTypeDef == null || !t.IsDerivedFrom(SystemArrayT)) {
            return false;
        }

        string arrayDims = "1D";

        // Look for the last "[]" in the type name
        // however we can have stuff like "[,]" or "[,,]" etc
        var tFullName = t.GetFullName();
        var lastDims = tFullName.LastIndexOf('[');
        var lastDimsEnd = tFullName.LastIndexOf(']');

        if (lastDims != -1 && lastDimsEnd != -1) {
            // Count how many , there are
            var dimCount = 0;

            for (int i = lastDims+1; i < lastDimsEnd; i++) {
                if (tFullName[i] == ',') {
                    dimCount++;
                }
            }

            arrayDims = (dimCount + 1).ToString() + "D";
        }

        typesWithArrayTypes.Add(elementTypeDef);

        if (!elementTypesToArrayTypes.ContainsKey(elementTypeDef)) {
            elementTypesToArrayTypes[elementTypeDef] = [];
        }

        elementTypesToArrayTypes[elementTypeDef].Add(t);
        
        // Check if the element type is a System.Array derived type
        if (elementTypeDef.IsDerivedFrom(SystemArrayT)) {
            if (HandleArrayType(elementTypeDef)) {
                typeRenames[t] = typeRenames[elementTypeDef] + "_Array" + arrayDims;
            } else {
                typeRenames[t] = elementTypeDef.Name + "_Array" + arrayDims;
            }

            if (typeFullRenames.ContainsKey(elementTypeDef)) {
                typeFullRenames[t] = typeFullRenames[elementTypeDef] + "_Array" + arrayDims;
            }
        } else {
            typeRenames[t] = elementTypeDef.Name + "_Array" + arrayDims;
            typeFullRenames[t] = MakePrefixedTypeName(elementTypeDef) + "_Array" + arrayDims;

            if (typeFullRenames.ContainsKey(elementTypeDef)) {
                typeFullRenames[t] = typeFullRenames[elementTypeDef] + "_Array" + arrayDims;
            }
        }

        return true;
    }

    static void FillValidEntries(REFrameworkNET.TDB context) {
        if (validTypes.Count > 0) {
            return;
        }

        context.GetType(0).GetFullName(); // initialize the types

        foreach (REFrameworkNET.TypeDefinition t in context.Types) {
            //var t = context.GetType((uint)i);
            var typeName = t.GetFullName();

            if (typeName.Length == 0) {
                Console.WriteLine("Bad type name");
                continue;
            }

            // Generics and arrays not yet supported
            if (typeName.Contains("[[") /*|| typeName.Contains("]")*/ || typeName.Contains('!')) {
                continue;
            }

            if (typeName.Contains('<') && !t.IsGenericTypeDefinition()) {
                continue;
            }

            if (t.Namespace == null || t.Namespace.Length == 0) {
                if (typeName.Length == 0) {
                    continue;
                }

                var optionalPrefix = GetOptionalPrefix(t);

                if (optionalPrefix != null) {
                    typeFullRenames[t] = optionalPrefix + t.GetFullName();
                }
            }

            if (t.IsDerivedFrom(SystemArrayT)) {
                arrayTypes.Add(t);
                
                HandleArrayType(t);
            }

            /*if (t.IsGenericType() && !t.IsGenericTypeDefinition()) {
                continue;
            }*/

            validTypes.Add(typeName);
        }
    }

    static CompilationUnitSyntax MakeFromTypeEntry(REFrameworkNET.TDB context, string typeName, REFrameworkNET.TypeDefinition? t) {
        var compilationUnit = SyntaxFactory.CompilationUnit();

        if (!validTypes.Contains(typeName)) {
            return compilationUnit;
        }

        if (t == null) {
            Console.WriteLine("Failed to find type");
            return compilationUnit;
        }

        if (t.DeclaringType != null) {
            //MakeFromTypeEntry(context, t.DeclaringType.Name ?? "", t.DeclaringType);
            return compilationUnit; // We want to define it inside of its declaring type, not a second time
        }

        if (generatedTypes.Contains(typeName)) {
            //Console.WriteLine("Skipping already generated type " + typeName);
            return compilationUnit;
        }

        generatedTypes.Add(typeName);

        // do not generate array types directly, we do it manually per element type
        if (typeName.Contains("[]")) {
            Console.WriteLine("Skipping array type " + typeName);
            return compilationUnit;
        }

        if (typeFullRenames.TryGetValue(t, out string? renamedTypeName)) {
            typeName = renamedTypeName;
        }

        if (t.IsEnum()) {
            var generator = new EnumGenerator(typeName, t);

            if (generator.EnumDeclaration == null) {
                return compilationUnit;
            }

            var generatedNamespace = ExtractNamespaceFromType(t);

            if (generatedNamespace != null) {
                var myNamespace = SyntaxTreeBuilder.AddMembersToNamespace(generatedNamespace, generator.EnumDeclaration);
                compilationUnit = SyntaxTreeBuilder.AddMembersToCompilationUnit(compilationUnit, myNamespace);
            } else {
                Console.WriteLine("Failed to create namespace for " + typeName);
            }

            ForEachArrayType(t, (arrayType) => {
                var arrayTypeName = typeFullRenames[arrayType];

                var arrayClassGenerator = new ClassGenerator(
                    arrayTypeName,
                    arrayType
                );

                if (arrayClassGenerator.TypeDeclaration == null) {
                    return;
                }

                // We can re-use the namespace from the original type
                if (generatedNamespace != null) {
                    var myNamespace = SyntaxTreeBuilder.AddMembersToNamespace(generatedNamespace, arrayClassGenerator.TypeDeclaration);
                    compilationUnit = SyntaxTreeBuilder.AddMembersToCompilationUnit(compilationUnit, myNamespace);
                }
            });
        } else {
            // Generate starting from topmost parent first
            if (t.ParentType != null) {
                compilationUnit = MakeFromTypeEntry(context, t.ParentType.FullName ?? "", t.ParentType);
            }

            var generator = new ClassGenerator(
                typeName.Split('.').Last() == "file" ? typeName.Replace("file", "@file") : typeName,
                t
            );

            if (generator.TypeDeclaration == null) {
                return compilationUnit;
            }

            var generatedNamespace = ExtractNamespaceFromType(t);

            if (generatedNamespace != null) {
                var myNamespace = SyntaxTreeBuilder.AddMembersToNamespace(generatedNamespace, generator.TypeDeclaration);
                compilationUnit = SyntaxTreeBuilder.AddMembersToCompilationUnit(compilationUnit, myNamespace);
            } else {
                Console.WriteLine("Failed to create namespace for " + typeName);
            }

            ForEachArrayType(t, (arrayType) => {
                var arrayTypeName = typeFullRenames[arrayType];

                System.Console.WriteLine("Generating array type " + arrayTypeName + " from " + t.FullName);

                var arrayClassGenerator = new ClassGenerator(
                    arrayTypeName,
                    arrayType
                );

                if (arrayClassGenerator.TypeDeclaration == null) {
                   return;
                }

                // We can re-use the namespace from the original type
                if (generatedNamespace != null) {
                    var myNamespace = SyntaxTreeBuilder.AddMembersToNamespace(generatedNamespace, arrayClassGenerator.TypeDeclaration);
                    compilationUnit = SyntaxTreeBuilder.AddMembersToCompilationUnit(compilationUnit, myNamespace);
                }
            });
        }

        return compilationUnit;
    }

    [REFrameworkNET.Attributes.PluginEntryPoint]
    public static List<REFrameworkNET.Compiler.DynamicAssemblyBytecode> Main() {
        try {
            return MainImpl();
        } catch (Exception e) {
            Console.WriteLine("Exception: " + e);

            var ex = e;
            while (ex.InnerException != null) {
                ex = ex.InnerException;
                Console.WriteLine("Inner Exception: " + ex);
            }
        }

        return [];
    }

    public static REFrameworkNET.Compiler.DynamicAssemblyBytecode? GenerateForAssembly(dynamic assembly, List<REFrameworkNET.Compiler.DynamicAssemblyBytecode> previousCompilations) {
        var strippedAssemblyName = assembly.get_FullName().Split(',')[0];

        // Dont want to conflict with the real .NET System
        if (strippedAssemblyName == "System") {
            strippedAssemblyName = "_System";
        }

        if (strippedAssemblyName == "mscorlib") {
            strippedAssemblyName = "_mscorlib";
        }

        if (strippedAssemblyName == "System.Core") {
            strippedAssemblyName = "_System.Core";
        }

        REFrameworkNET.API.LogInfo("Generating assembly " + strippedAssemblyName);

        List<CompilationUnitSyntax> compilationUnits = [];
        var tdb = REFrameworkNET.API.GetTDB();

        List<dynamic> typeList = [];

        foreach (dynamic reEngineT in assembly.GetTypes()) {
            typeList.Add(reEngineT);
        }

        // Clean up all the local objects
        // Mainly because some of the older games don't play well with a ton of objects on the thread local heap
        REFrameworkNET.API.LocalFrameGC();

        // Is this parallelizable?
        foreach (dynamic reEngineT in typeList) {
            var th = reEngineT.get_TypeHandle();

            if (th == null) {
                Console.WriteLine("Failed to get type handle for " + reEngineT.get_FullName());
                continue;
            }

            var t = th as REFrameworkNET.TypeDefinition;

            if (t == null) {
                Console.WriteLine("Failed to convert type handle for " + reEngineT.get_FullName());
                continue;
            }

            var typeName = t.GetFullName();
            var compilationUnit = MakeFromTypeEntry(tdb, typeName, t);
            compilationUnits.Add(compilationUnit);

            // Clean up all the local objects
            // Mainly because some of the older games don't play well with a ton of objects on the thread local heap
            REFrameworkNET.API.LocalFrameGC();
        }

        System.Console.WriteLine("Compiling " + strippedAssemblyName + " with " + compilationUnits.Count + " compilation units...");

        List<SyntaxTree> syntaxTrees = new List<SyntaxTree>();

        var syntaxTreeParseOption = CSharpParseOptions.Default.WithLanguageVersion(LanguageVersion.CSharp12);
        
        foreach (var cu in compilationUnits) {
            syntaxTrees.Add(SyntaxFactory.SyntaxTree(cu/*.NormalizeWhitespace()*/, syntaxTreeParseOption));
        }

        string? assemblyPath = Path.GetDirectoryName(typeof(object).Assembly.Location);
        var references = REFrameworkNET.Compiler.GenerateExhaustiveMetadataReferences(typeof(REFrameworkNET.API).Assembly, new List<Assembly>());

        // Add the previous compilations as references
        foreach (var compilationbc in previousCompilations) {
            var ms = new MemoryStream(compilationbc.Bytecode);
            references.Add(MetadataReference.CreateFromStream(ms));
        }

        //compilationUnit = compilationUnit.AddUsings(SyntaxFactory.UsingDirective(SyntaxFactory.ParseName("System")));

        var csoptions = new CSharpCompilationOptions(OutputKind.DynamicallyLinkedLibrary, 
            optimizationLevel: OptimizationLevel.Release,
            assemblyIdentityComparer: DesktopAssemblyIdentityComparer.Default,
            platform: Platform.X64,
            allowUnsafe: true);
        // Create a compilation
        CSharpCompilation compilation = CSharpCompilation.Create(strippedAssemblyName)
            .WithOptions(csoptions)
            .AddReferences(references)
            .AddSyntaxTrees(syntaxTrees);

        // Emit the assembly to a stream (in-memory assembly)
        using (var ms = new MemoryStream())
        {
            var result = compilation.Emit(ms);

            if (!result.Success)
            {
                //var textLines = syntaxTrees.GetText().Lines;
                List<Diagnostic> sortedDiagnostics = result.Diagnostics.OrderBy(d => d.Location.SourceSpan.Start).ToList();
                sortedDiagnostics.Reverse();

                foreach (Diagnostic diagnostic in sortedDiagnostics)
                {
                    var textLines = diagnostic.Location.SourceTree?.GetText().Lines;
                    Console.WriteLine($"{diagnostic.Id}: {diagnostic.GetMessage()}");

                    var lineSpan = diagnostic.Location.GetLineSpan();
                    var errorLineNumber = lineSpan.StartLinePosition.Line;
                    var errorLineText = textLines?[errorLineNumber].ToString();
                    Console.WriteLine($"Error in line {errorLineNumber + 1}: {errorLineText}");
                    //Console.WriteLine(diagnostic.Location.SourceTree?.GetText());
                    //Console.WriteLine(
                        //$"Error in line {errorLineNumber + 1}: {lineSpan.StartLinePosition.Character + 1} - {lineSpan.EndLinePosition.Character + 1}");
                }

                REFrameworkNET.API.LogError("Failed to compile " + strippedAssemblyName);
            }
            else
            {
                // Load and use the compiled assembly
                ms.Seek(0, SeekOrigin.Begin);
                //var assembly = Assembly.Load(ms.ToArray());

                // dump to file
                //File.WriteAllBytes("DynamicAssembly.dll", ms.ToArray());

                REFrameworkNET.API.LogInfo("Successfully compiled " + strippedAssemblyName);

                return 
                    new REFrameworkNET.Compiler.DynamicAssemblyBytecode {
                        Bytecode = ms.ToArray(),
                        AssemblyName = strippedAssemblyName
                    };
            }
        }

        return null;
    }

    public static List<REFrameworkNET.Compiler.DynamicAssemblyBytecode> MainImpl() {
        var tdb = REFrameworkNET.API.GetTDB();
        Il2CppDump.FillTypeExtensions(tdb);
        FillValidEntries(tdb);

        dynamic appdomainT = tdb.GetType("System.AppDomain");
        dynamic appdomain = appdomainT.get_CurrentDomain();
        var assemblies = (appdomain.GetAssemblies() as ManagedObject)!;

        List<dynamic> assembliesList = [];

        // Pre-emptively add all assemblies to the list
        // because the assemblies list that was returned will be cleaned up by the call to LocalFrameGC
        foreach (ManagedObject assembly in assemblies) {
            if (assembly == null) {
                continue;
            }

            // The object will get destroyed by the LocalFrameGC call if we don't do this
            if (!assembly.IsGlobalized()) {
                assembly.Globalize();
            }

            assembliesList.Add(assembly);
        }

        List<REFrameworkNET.Compiler.DynamicAssemblyBytecode> bytecodes = [];

        foreach (dynamic assembly in assembliesList) {
            var strippedAssemblyName = assembly.get_FullName().Split(',')[0];
            REFrameworkNET.API.LogInfo("Assembly: " + (assembly.get_Location()?.ToString() ?? "NONE"));
            REFrameworkNET.API.LogInfo("Assembly (stripped): " + strippedAssemblyName);

            var bytecode = GenerateForAssembly(assembly, bytecodes);

            if (bytecode != null) {
                bytecodes.Add(bytecode);
            }

            // Clean up all the local objects
            // Mainly because some of the older games don't play well with a ton of objects on the thread local heap
            REFrameworkNET.API.LocalFrameGC();
        }

        return bytecodes;
    }
};
}