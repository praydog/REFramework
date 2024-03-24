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

public class Il2CppDump {
    class Field {

    };

    public class Method {
        public REFrameworkNET.Method Impl;

        public Method(REFrameworkNET.Method impl) {
            this.Impl = impl;
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
            foreach (var method in tMethods) { // parallel isn't necessary here because there arent many methods
                if (method == null) {
                    continue;
                }

                if (GetMethodExtension(method) != null) {
                    continue;
                }

                var parentMethod = parent.GetMethod(method.Name);

                if (parentMethod != null) {
                    methodExtensions.Add(method, new Method(method) {
                        Override = true
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
    static CompilationUnitSyntax compilationUnit = SyntaxFactory.CompilationUnit();

    static public NamespaceDeclarationSyntax? ExtractNamespaceFromType(REFrameworkNET.TypeDefinition t) {
        var ns = t.GetNamespace();

        if (ns != null) {
            if (!namespaces.TryGetValue(ns, out NamespaceDeclarationSyntax? value)) {
                //ns = Regex.Replace(ns, @"[^a-zA-Z0-9.]", "_");
                Console.WriteLine("Creating namespace " + ns);
                value = SyntaxTreeBuilder.CreateNamespace(ns);
                namespaces[ns] = value;
            }

            return value;
        }

        return null;
    }

    public static SortedSet<string> validTypes = [];
    public static SortedSet<string> generatedTypes = [];

    static void FillValidEntries(REFrameworkNET.TDB context) {
        if (validTypes.Count > 0) {
            return;
        }

        ConcurrentBag<string> threadSafeValidTypes = [];

        Parallel.For(0, context.GetNumTypes(), i => {
            var t = context.GetType((uint)i);
            var typeName = t.GetFullName();

            if (typeName.Length == 0) {
                Console.WriteLine("Bad type name");
                return;
            }

            if (typeName.Contains("WrappedArrayContainer")) {
                return;
            }

            if (typeName.Contains("[") || typeName.Contains("]") || typeName.Contains('<')) {
                return;
            }

            // Skip system types
            // TODO: Fix this
            if (typeName.StartsWith("System.")) {
                return;
            }

            threadSafeValidTypes.Add(typeName);
        });

        foreach (var typeName in threadSafeValidTypes) {
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

        var generatedNamespace = ExtractNamespaceFromType(t);

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

    public static List<REFrameworkNET.Compiler.DynamicAssemblyBytecode> Main(REFrameworkNET.API api) {
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

    public static List<REFrameworkNET.Compiler.DynamicAssemblyBytecode> MainImpl() {
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
        string compilationUnitHash = "";

        using (var sha1 = System.Security.Cryptography.SHA1.Create()) {
            compilationUnitHash = BitConverter.ToString(sha1.ComputeHash(compilationUnit.ToFullString().Select(c => (byte)c).ToArray())).Replace("-", "");
        }

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
                //var assembly = Assembly.Load(ms.ToArray());

                // dump to file
                //File.WriteAllBytes("DynamicAssembly.dll", ms.ToArray());

                REFrameworkNET.API.LogInfo("Successfully compiled DynamicAssembly.dll");

                return [
                    new REFrameworkNET.Compiler.DynamicAssemblyBytecode {
                        Bytecode = ms.ToArray(),
                        Hash = compilationUnitHash
                    }
                ];
            }
        }

        return [];
    }
};
}