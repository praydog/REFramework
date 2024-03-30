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
        
        public REFrameworkNET.TypeDefinition DeclaringType => Impl.GetDeclaringType();

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
    //static CompilationUnitSyntax compilationUnit = SyntaxFactory.CompilationUnit();

    public static string CorrectTypeName(string fullName) {
        if (fullName.StartsWith("System.") || fullName.StartsWith("Internal.")) {
            return "_" + fullName;
        }

        return fullName;
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
        } else {
            Console.WriteLine("Failed to extract namespace from " + t.GetFullName());
        }

        return null;
    }

    public static SortedSet<string> validTypes = [];
    public static SortedSet<string> generatedTypes = [];

    static void FillValidEntries(REFrameworkNET.TDB context) {
        if (validTypes.Count > 0) {
            return;
        }

        foreach (REFrameworkNET.TypeDefinition t in context.Types) {
            var asdf = t.GetFullName();
        }

        ConcurrentBag<string> threadSafeValidTypes = [];

        context.GetType(0).GetFullName(); // initialize the types

        Parallel.For(0, context.GetNumTypes(), i => {
            var t = context.GetType((uint)i);
            var typeName = t.GetFullName();

            if (typeName.Length == 0) {
                Console.WriteLine("Bad type name @ " + i);
                return;
            }

            if (typeName.Contains("WrappedArrayContainer")) {
                return;
            }

            // Generics and arrays not yet supported
            if (typeName.Contains("[") || typeName.Contains("]") || typeName.Contains('<') || typeName.Contains('!')) {
                return;
            }

            // Dont worry about global namespace types for now...
            if (t.Namespace == null || t.Namespace.Length == 0) {
                return;
            }

            // Skip system types
            // TODO: Fix this
            /*if (typeName.StartsWith("System.")) {
                //return;
                threadSafeValidTypes.Add("_" + typeName);
            } else {*/
                threadSafeValidTypes.Add(typeName);
            //}
        });

        foreach (var typeName in threadSafeValidTypes) {
            validTypes.Add(typeName);
        }
    }

    static CompilationUnitSyntax MakeFromTypeEntry(REFrameworkNET.TDB context, string typeName, REFrameworkNET.TypeDefinition? t) {
        var compilationUnit = SyntaxFactory.CompilationUnit();
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
            compilationUnit = MakeFromTypeEntry(context, t.ParentType.FullName ?? "", t.ParentType);
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
                if (method.DeclaringType == t) { // really important
                    methods.Add(method);
                }
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

    public static REFrameworkNET.Compiler.DynamicAssemblyBytecode? GenerateForAssembly(dynamic assembly, List<REFrameworkNET.Compiler.DynamicAssemblyBytecode> previousCompilations) {
        var strippedAssemblyName = assembly.get_FullName().Split(',')[0];

        // Dont want to conflict with the real .NET System
        if (strippedAssemblyName == "System") {
            strippedAssemblyName = "_System";
        }

        REFrameworkNET.API.LogInfo("Generating assembly " + strippedAssemblyName);

        List<CompilationUnitSyntax> compilationUnits = [];
        var tdb = REFrameworkNET.API.GetTDB();

        foreach (dynamic reEngineT in assembly.GetTypes()) {
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
        }

        List<SyntaxTree> syntaxTrees = new List<SyntaxTree>();

        var syntaxTreeParseOption = CSharpParseOptions.Default.WithLanguageVersion(LanguageVersion.CSharp12);
        
        foreach (var cu in compilationUnits) {
            syntaxTrees.Add(SyntaxFactory.SyntaxTree(cu, syntaxTreeParseOption));
        }

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

        // Add the previous compilations as references
        foreach (var compilationbc in previousCompilations) {
            var ms = new MemoryStream(compilationbc.Bytecode);
            references.Add(MetadataReference.CreateFromStream(ms));
        }

        //compilationUnit = compilationUnit.AddUsings(SyntaxFactory.UsingDirective(SyntaxFactory.ParseName("System")));

        var csoptions = new CSharpCompilationOptions(OutputKind.DynamicallyLinkedLibrary, 
            optimizationLevel: OptimizationLevel.Release,
            assemblyIdentityComparer: DesktopAssemblyIdentityComparer.Default,
            platform: Platform.X64);
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
        Il2CppDump.FillTypeExtensions(REFrameworkNET.API.GetTDB());

        var tdb = REFrameworkNET.API.GetTDB();

        dynamic appdomainT = tdb.GetType("System.AppDomain");
        dynamic appdomain = appdomainT.get_CurrentDomain();
        dynamic assemblies = appdomain.GetAssemblies();

        List<REFrameworkNET.Compiler.DynamicAssemblyBytecode> bytecodes = [];

        foreach (dynamic assembly in assemblies) {
            var strippedAssemblyName = assembly.get_FullName().Split(',')[0];
            REFrameworkNET.API.LogInfo("Assembly: " + (assembly.get_Location()?.ToString() ?? "NONE"));
            REFrameworkNET.API.LogInfo("Assembly (stripped): " + strippedAssemblyName);

            var bytecode = GenerateForAssembly(assembly, bytecodes);

            if (bytecode != null) {
                bytecodes.Add(bytecode);
            }
        }

        return bytecodes;
    }
};
}