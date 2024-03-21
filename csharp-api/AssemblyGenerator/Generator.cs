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

public class Il2CppDump {
    class Field {

    };

    public class Method {
        private REFrameworkNET.Method impl;

        public Method(REFrameworkNET.Method impl) {
            this.impl = impl;
        }
        
        public bool? Override { get; set;} // Not from JSON
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
        public HashSet<REFrameworkNET.TypeDefinition>? NestedTypes { get; set;}
    };

    static private Dictionary<REFrameworkNET.TypeDefinition, Type> typeExtensions = [];
    static private Dictionary<REFrameworkNET.Method, Method> methodExtensions = [];
    static public Type? GetTypeExtension(REFrameworkNET.TypeDefinition type) {
        if (typeExtensions.TryGetValue(type, out Type? value)) {
            return value;
        }

        return null;
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

        // Look for types that have a declaring type and add them to the declaring type's nested types
        foreach (REFrameworkNET.TypeDefinition t in context.Types) {
            if (t.DeclaringType != null) {
                if (!typeExtensions.TryGetValue(t.DeclaringType, out Type? value)) {
                    value = new Type(t.DeclaringType);
                    typeExtensions[t.DeclaringType] = value;
                }

                //value.NestedTypes ??= [];
                if (value.NestedTypes == null) {
                    value.NestedTypes = new HashSet<REFrameworkNET.TypeDefinition>();
                }
                value.NestedTypes.Add(t);

                //System.Console.WriteLine("Adding nested type " + t.GetFullName() + " to " + t.DeclaringType.GetFullName());
            }

            // Look for methods with the same name and mark them as overrides
            for (var parent = t.ParentType; parent != null; parent = parent.ParentType) {
                if (parent.Methods.Count == 0 || t.Methods.Count == 0) {
                    continue;
                }

                foreach (var method in t.Methods) {
                    var parentMethod = parent.GetMethod(method.Name);

                    if (parentMethod != null) {
                        if (!methodExtensions.TryGetValue(method, out Method? value)) {
                            value = new Method(method);
                            methodExtensions[method] = value;
                        }

                        value.Override = true;
                    }
                }
            }
        }
    }
}

public class AssemblyGenerator {
    static Dictionary<string, NamespaceDeclarationSyntax> namespaces = [];

    // Start with an empty CompilationUnitSyntax (represents an empty file)
    static CompilationUnitSyntax compilationUnit = SyntaxFactory.CompilationUnit();

    static public NamespaceDeclarationSyntax? ExtractNamespaceFromTypeName(REFrameworkNET.TDB context, string typeName) {
        if (context == null || context.Types == null) {
            return null;
        }

        var parts = typeName.Split('.');
        var currentTypeName = "";

        NamespaceDeclarationSyntax? currentNamespaceDecl = null;
        string currentNamespaceName = "";

        for (var i = 0; i < parts.Length; i++) {
            var part = parts[i];
            currentTypeName += part;
            
            if (context.GetType(currentTypeName) != null) {
                // Return a blank namespace
                if (currentNamespaceDecl == null) {
                    System.Console.WriteLine("Creating blank namespace for " + currentTypeName);
                    currentNamespaceDecl = SyntaxTreeBuilder.CreateNamespace("");
                }

                return currentNamespaceDecl;
            }

            currentNamespaceName += part;

            // Create via namespace in list of namespaces if not exist
            if (!namespaces.TryGetValue(currentTypeName, out NamespaceDeclarationSyntax? value)) {
                // Clean up the namespace name, remove any non-compliant characters other than "." and alphanumerics
                currentNamespaceName = Regex.Replace(currentTypeName, @"[^a-zA-Z0-9.]", "_");

                Console.WriteLine("Creating namespace " + currentNamespaceName);
                value = SyntaxTreeBuilder.CreateNamespace(currentNamespaceName);
                namespaces[currentNamespaceName] = value;
                currentNamespaceDecl = value;
            } else {
                currentNamespaceDecl = value;
            }

            currentTypeName += ".";
        }

        return currentNamespaceDecl;
    }

    public static SortedSet<string> validTypes = [];
    public static SortedSet<string> generatedTypes = [];

    static void FillValidEntries(REFrameworkNET.TDB context) {
        if (validTypes.Count > 0) {
            return;
        }

        // TDB only has GetType(index) and GetNumTypes()
        foreach (REFrameworkNET.TypeDefinition t in context.Types) {
            var typeName = t.GetFullName();

            if (typeName.Length == 0) {
                Console.WriteLine("Bad type name");
                continue;
            }

            if (typeName.Contains("WrappedArrayContainer")) {
                continue;
            }

            if (typeName.Contains("[") || typeName.Contains("]") || typeName.Contains('<')) {
                continue;
            }

            // Skip system types
            if (typeName.StartsWith("System.")) {
                continue;
            }

            validTypes.Add(typeName);
        }
    }

    static CompilationUnitSyntax MakeFromTypeEntry(REFrameworkNET.TDB context, string typeName, REFrameworkNET.TypeDefinition? t) {
        FillValidEntries(context);

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

        // Generate starting from topmost parent first
        if (t.ParentType != null) {
            MakeFromTypeEntry(context, t.ParentType.FullName ?? "", t.ParentType);
        }

        /*var methods = t.Methods;
        var fixedMethods = methods?
            .Select(methodPair => {
                var method = methodPair.Value;
                var methodName = Il2CppDump.StripMethodName(method);
                return (methodName, method);
            })
            .GroupBy(pair => pair.methodName)
            .Select(group => group.First()) // Selects the first method of each group
            .ToDictionary(pair => pair.methodName, pair => pair.method);*/

        // Make methods a SortedSet of method names
        HashSet<REFrameworkNET.Method> methods = [];

        foreach (var method in t.Methods) {
            //methods.Add(method);
            if (!methods.Select(m => m.Name).Contains(method.Name)) {
                methods.Add(method);
            }
        }

        var generator = new ClassGenerator(
            typeName.Split('.').Last() == "file" ? typeName.Replace("file", "@file") : typeName,
            t,
            [.. methods]
        );

        if (generator.TypeDeclaration == null) {
            return compilationUnit;
        }

        var generatedNamespace = ExtractNamespaceFromTypeName(context, typeName);

        if (generatedNamespace != null) {
            // Split the using types by their namespace
            /*foreach(var ut in usingTypes) {
                var ns = ExtractNamespaceFromTypeName(context, ut.Name ?? "");

                if (ns != null) {
                    generatedNamespace = generatedNamespace.AddUsings(SyntaxFactory.UsingDirective(SyntaxFactory.ParseName(ut.Name ?? "")));
                }
            }*/

            var myNamespace = SyntaxTreeBuilder.AddMembersToNamespace(generatedNamespace, generator.TypeDeclaration);

            /*compilationUnit = compilationUnit.AddUsings(usingTypes.Select(
                type =>  {
                    var ret = SyntaxFactory.UsingDirective (SyntaxFactory.ParseName(type.Name ?? ""));
                    System.Console.WriteLine(ret.GetText());

                    return ret;
                }
            ).ToArray());*/

            compilationUnit = SyntaxTreeBuilder.AddMembersToCompilationUnit(compilationUnit, myNamespace);
        } else {
            Console.WriteLine("Failed to create namespace for " + typeName);
        }

        return compilationUnit;
    }

    public static void Main(REFrameworkNET.API api) {
        try {
            MainImpl();
        } catch (Exception e) {
            Console.WriteLine("Exception: " + e);

            var ex = e;
            while (ex.InnerException != null) {
                ex = ex.InnerException;
                Console.WriteLine("Inner Exception: " + ex);
            }
        }
    }

    public static void MainImpl() {
        Il2CppDump.FillTypeExtensions(REFrameworkNET.API.GetTDB());

        List<CompilationUnitSyntax> compilationUnits = new List<CompilationUnitSyntax>();

        // Open a JSON file
        /*using (var jsonFile = File.OpenRead(il2cpp_dump_json))
        {
            dump = new Il2CppDump
            {
                Types = Il2CppDump.PostProcessTypes(JsonSerializer.Deserialize<Dictionary<string, Il2CppDump.Type>>(jsonFile, options))
            };

            if (dump != null && dump.Types != null) {
                // Look for any types that start with via.*
                foreach (var typePair in dump.Types ?? []) {
                    var typeName = typePair.Key;
                    var type = typePair.Value;

                    if (typeName.StartsWith("via.")) {
                        var compilationUnit = MakeFromTypeEntry(dump, typeName, type);
                        compilationUnits.Add(compilationUnit);
                    }
                }
            } else {
                Console.WriteLine("Failed to parse JSON");
            }
        }*/

        var tdb = REFrameworkNET.API.GetTDB();

        foreach (REFrameworkNET.TypeDefinition t in tdb.Types) {
            var typeName = t.GetFullName();

            if (typeName.StartsWith("via.")) {
                var compilationUnit = MakeFromTypeEntry(REFrameworkNET.API.GetTDB(), typeName, t);
                compilationUnits.Add(compilationUnit);
            }
        }

        System.Console.WriteLine(compilationUnits[0].NormalizeWhitespace().ToFullString());


        /*List<SyntaxTree> syntaxTrees = new List<SyntaxTree>();
        
        foreach (var cu in compilationUnits) {
            syntaxTrees.Add(SyntaxFactory.SyntaxTree(cu));
        }*/

        var normalized = compilationUnit.NormalizeWhitespace();

        // Dump to DynamicAssembly.cs
        File.WriteAllText("DynamicAssembly.cs", normalized.ToFullString());

        var syntaxTrees = SyntaxFactory.SyntaxTree(normalized, CSharpParseOptions.Default.WithLanguageVersion(LanguageVersion.CSharp12));

        string? assemblyPath = Path.GetDirectoryName(typeof(object).Assembly.Location);

        // get all DLLs in that directory
        var dlls = assemblyPath != null? Directory.GetFiles(assemblyPath, "*.dll") : [];

        var systemRuntimePath = dlls.FirstOrDefault(dll => dll.ToLower().EndsWith("system.runtime.dll"));

        var references = new List<MetadataReference> {
            MetadataReference.CreateFromFile(typeof(object).Assembly.Location),
            MetadataReference.CreateFromFile(typeof(void).Assembly.Location),
            MetadataReference.CreateFromFile(typeof(System.NotImplementedException).Assembly.Location),
            MetadataReference.CreateFromFile(typeof(System.Linq.Enumerable).Assembly.Location),
            MetadataReference.CreateFromFile(typeof(System.Runtime.AssemblyTargetedPatchBandAttribute).Assembly.Location),
            MetadataReference.CreateFromFile(typeof(Microsoft.CSharp.RuntimeBinder.CSharpArgumentInfo).Assembly.Location),
        };

        if (systemRuntimePath != null) {
            System.Console.WriteLine("Adding System.Runtime from " + systemRuntimePath);
            references.Add(MetadataReference.CreateFromFile(systemRuntimePath));
        }

        compilationUnit = compilationUnit.AddUsings(SyntaxFactory.UsingDirective(SyntaxFactory.ParseName("System")));

        var csoptions = new CSharpCompilationOptions(OutputKind.DynamicallyLinkedLibrary, 
            optimizationLevel: OptimizationLevel.Release,
            assemblyIdentityComparer: DesktopAssemblyIdentityComparer.Default,
            platform: Platform.X64);
        // Create a compilation
        var compilation = CSharpCompilation.Create("DynamicAssembly")
            .WithOptions(csoptions)
            .AddReferences(references)
            .AddSyntaxTrees(syntaxTrees);

        // Emit the assembly to a stream (in-memory assembly)
        using (var ms = new MemoryStream())
        {
            var result = compilation.Emit(ms);

            if (!result.Success)
            {
                var textLines = syntaxTrees.GetText().Lines;
                List<Diagnostic> sortedDiagnostics = result.Diagnostics.OrderBy(d => d.Location.SourceSpan.Start).ToList();
                sortedDiagnostics.Reverse();

                foreach (Diagnostic diagnostic in sortedDiagnostics)
                {
                    Console.WriteLine($"{diagnostic.Id}: {diagnostic.GetMessage()}");

                    var lineSpan = diagnostic.Location.GetLineSpan();
                    var errorLineNumber = lineSpan.StartLinePosition.Line;
                    var errorLineText = textLines[errorLineNumber].ToString();
                    Console.WriteLine($"Error in line {errorLineNumber + 1}: {errorLineText}");
                }

                REFrameworkNET.API.LogError("Failed to compile DynamicAssembly.dll");
            }
            else
            {
                // Load and use the compiled assembly
                ms.Seek(0, SeekOrigin.Begin);
                var assembly = Assembly.Load(ms.ToArray());

                // dump to file
                File.WriteAllBytes("DynamicAssembly.dll", ms.ToArray());

                REFrameworkNET.API.LogInfo("Successfully compiled DynamicAssembly.dll");
            }
        }

    }
};