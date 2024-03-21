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
        public class ReturnValueClass {
            [JsonPropertyName("type")]
            public string? Type { get; set;}
            [JsonPropertyName("name")]
            public string? Name { get; set;}
        };

        public class Parameter {
            [JsonPropertyName("name")]
            public string? Name { get; set;}
            [JsonPropertyName("type")]
            public string? Type { get; set;}
        };
    
        [JsonPropertyName("flags")]
        public string? Flags { get; set;}
        
        [JsonPropertyName("function")]
        public string? Function { get; set;}

        [JsonPropertyName("id")]
        public int? ID { get; set;}
        [JsonPropertyName("invoke_id")]
        public int? InvokeID { get; set;}
        [JsonPropertyName("impl_flags")]
        public string? ImplFlags { get; set;}
        [JsonPropertyName("returns")]
        public ReturnValueClass? Returns { get; set;}
        [JsonPropertyName("params")]
        public List<Parameter>? Params { get; set;}

        public string? Name { get; set;} // Not from JSON
        public string? FriendlyName { get; set;} // Not from JSON
        public bool? Override { get; set;} // Not from JSON
    }

    public class Type {
        [JsonPropertyName("address")]
        public string? Address { get; set;}
        [JsonPropertyName("fqn")]
        public string? FQN { get; set;}
        [JsonPropertyName("flags")]
        public string? Flags { get; set;}
        [JsonPropertyName("crc")]
        public string? CRC { get; set;}
        [JsonPropertyName("id")]
        public int? ID { get; set;}
        [JsonPropertyName("is_generic_type")]
        public bool? IsGenericType { get; set;}
        [JsonPropertyName("is_generic_type_definition")]
        public bool? IsGenericTypeDefinition { get; set;}

        [JsonPropertyName("parent")]
        public string? Parent { get; set;}
        [JsonPropertyName("declaring_type")]
        public string? DeclaringTypeName { get; set;}
        [JsonPropertyName("methods")]
        public Dictionary<string, Method>? Methods { get; set;}

        public Dictionary<string, Method>? StrippedMethods { get; set;} // Not from JSON

        [JsonPropertyName("name_hierarchy")]
        public List<string>? NameHierarchy { get; set;}

        // Post processed parent type
        public Il2CppDump.Type? ParentType { get; set;}
        public Il2CppDump.Type? DeclaringType { get; set;}
        public string? Name { get; set;} // Not from JSON
        public List<Il2CppDump.Type>? NestedTypes { get; set;}
    };

    public Dictionary<string, Type>? Types;

    public static string StripMethodName(Method method) {
        var methodName = method.Name ?? "";
        var methodID = method.ID ?? -1;

        var methodIdStr = methodID.ToString();
        var methodIdIndex = methodName.LastIndexOf(methodIdStr);
        if (methodIdIndex != -1) {
            return methodName[..methodIdIndex];
        }

        return methodName;
    }

    // For adding additional context to the types
    public static Dictionary<string, Type>? PostProcessTypes(Dictionary<string, Type>? types) {
        if (types == null) {
            return null;
        }

        foreach (var typePair in types) {
            var typeName = typePair.Key;
            var type = typePair.Value;

            if (type.Parent != null && types.TryGetValue(type.Parent, out var parentType)) {
                type.ParentType = parentType;
            }

            if (type.DeclaringTypeName != null && types.TryGetValue(type.DeclaringTypeName, out var declaringType)) {
                type.DeclaringType = declaringType;

                if (declaringType.NestedTypes == null) {
                    declaringType.NestedTypes = [];
                }

                declaringType.NestedTypes.Add(type);
            }

            type.Name = typeName;

            foreach (var methodPair in type.Methods ?? []) {
                if (methodPair.Value.Name == null) {
                    methodPair.Value.Name = methodPair.Key;
                }

                var methodName = StripMethodName(methodPair.Value);

                methodPair.Value.FriendlyName = methodName;

                if (typeName.Contains("ConvolutionReverbParameters") || typeName.Contains("via.audiorender.EffectParametersBase")) {
                    System.Console.WriteLine("Method: " + methodName);
                }

                if (type.StrippedMethods == null) {
                    type.StrippedMethods = [];
                }

                type.StrippedMethods[methodName] = methodPair.Value;
            }
        }

        foreach (var typePair in types) {
            var typeName = typePair.Key;
            var type = typePair.Value;

            foreach (var methodPair in type.Methods ?? []) {
                var methodName = methodPair.Key;
                var method = methodPair.Value;

                var flags = method.Flags?.Split(" | ");

                //if (flags != null && flags.Contains("Virtual")) {
                    for (var parent = type.ParentType; parent != null; parent = parent.ParentType) {
                        if (parent.StrippedMethods != null && parent.StrippedMethods.TryGetValue(method.FriendlyName?? "", out var parentMethod)) {
                            method.Override = true;
                            break;
                        }

                        if (method.Override?? false) {
                            break;
                        }
                    }
                //}
            }
        }

        return types;
    }
}

public class SnakeCaseToPascalCaseNamingPolicy : JsonNamingPolicy
{
    public override string ConvertName(string name)
    {
        // Convert snake_case to PascalCase
        return Regex.Replace(name, @"_(.)", match => match.Groups[1].Value.ToUpper());
    }
}

public class Program {
    static Dictionary<string, NamespaceDeclarationSyntax> namespaces = [];

    // Start with an empty CompilationUnitSyntax (represents an empty file)
    static CompilationUnitSyntax compilationUnit = SyntaxFactory.CompilationUnit();

    static public NamespaceDeclarationSyntax? ExtractNamespaceFromTypeName(Il2CppDump context, string typeName) {
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
            
            if (context.Types.TryGetValue(currentTypeName, out var value_)) {
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

    static void FillValidEntries(Il2CppDump context) {
        if (validTypes.Count > 0) {
            return;
        }

        foreach (var typePair in context.Types ?? []) {
            var typeName = typePair.Key;
            var t = typePair.Value;

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

    static CompilationUnitSyntax MakeFromTypeEntry(Il2CppDump context, string typeName, Il2CppDump.Type? t) {
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
            MakeFromTypeEntry(context, t.ParentType.Name ?? "", t.ParentType);
        }

        var methods = t.Methods;
        var fixedMethods = methods?
            .Select(methodPair => {
                var method = methodPair.Value;
                var methodName = Il2CppDump.StripMethodName(method);
                return (methodName, method);
            })
            .GroupBy(pair => pair.methodName)
            .Select(group => group.First()) // Selects the first method of each group
            .ToDictionary(pair => pair.methodName, pair => pair.method);

        foreach (var methodPair in fixedMethods ?? []) {
            methodPair.Value.Name = methodPair.Key;
        }

        var generator = new ClassGenerator(
            typeName.Split('.').Last() == "file" ? typeName.Replace("file", "@file") : typeName,
            t,
            fixedMethods != null ? [.. fixedMethods.Values] : []
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

    private static Il2CppDump? dump;
    public static Il2CppDump? Dump {
        get {
            return dump;
        }
    }

    static void Main(string[] args) {
        if (args.Length == 0) {
            Console.WriteLine("Usage: dotnet run <il2cpp_dump_json>");
            return;
        }

        var il2cpp_dump_json = args[0];

        if (il2cpp_dump_json == null) {
            Console.WriteLine("Usage: dotnet run <il2cpp_dump_json>");
            return;
        }

        Console.WriteLine(args[0]);

        List<CompilationUnitSyntax> compilationUnits = new List<CompilationUnitSyntax>();

        // Open a JSON file
        using (var jsonFile = File.OpenRead(il2cpp_dump_json))
        {
            var options = new JsonSerializerOptions
            {
                PropertyNamingPolicy = new SnakeCaseToPascalCaseNamingPolicy(),
            };

            dump = new Il2CppDump
            {
                Types = Il2CppDump.PostProcessTypes(JsonSerializer.Deserialize<Dictionary<string, Il2CppDump.Type>>(jsonFile, options))
            };

            if (dump != null && dump.Types != null) {
                // Get via.SceneManager type
                /*var sceneManager = dump.Types?["via.SceneManager"];

                MakeFromTypeEntry(sceneManager);*/

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
            }
            else
            {
                // Load and use the compiled assembly
                ms.Seek(0, SeekOrigin.Begin);
                var assembly = Assembly.Load(ms.ToArray());

                // dump to file
                File.WriteAllBytes("DynamicAssembly.dll", ms.ToArray());
            }
        }

    }
};