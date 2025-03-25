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
using REFrameworkNET.Attributes;
using REFrameworkNET;
using REFrameworkNET.Callbacks;
using System.Threading;


public class Il2CppDump {
    public class Field {
        public REFrameworkNET.Field Impl;
        public Field(REFrameworkNET.Field impl) {
            this.Impl = impl;
        }

        public List<REFrameworkNET.Field> MatchingParentFields = [];
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
    static private Dictionary<REFrameworkNET.Field, Field> fieldExtensions = [];
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

    static public Field? GetFieldExtension(REFrameworkNET.Field field) {
        if (fieldExtensions.TryGetValue(field, out Field? value)) {
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

            if (t.GetNumMethods() != 0 && t.ParentType != null) {
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

            if (t.GetNumFields() != 0 && t.ParentType != null) {
                var tFields = t.GetFields();

                foreach (REFrameworkNET.Field field in tFields) {
                    if (field == null) {
                        continue;
                    }

                    if (GetFieldExtension(field) != null) {
                        continue;
                    }

                    List<REFrameworkNET.Field> matchingParentFields = [];
                    for (var parent = t.ParentType; parent != null; parent = parent.ParentType) {
                        var parentFields = parent.GetFields();

                        foreach (var parentField in parentFields) {
                            if (parentField.Name == field.Name) {
                                matchingParentFields.Add(parentField);
                            }
                        }
                    }

                    if (matchingParentFields.Count > 0) {
                        fieldExtensions.Add(field, new Field(field) {
                            MatchingParentFields = matchingParentFields
                        });
                    }
                }
            }
        }   
    }
}

namespace REFrameworkNET {
public class AssemblyGenerator {
    static Dictionary<string, NamespaceDeclarationSyntax> namespaces = [];

    public static string FixBadChars(string name) {
        return name;
    }

    public static string CorrectTypeName(string fullName) {
        if (fullName.StartsWith("System.") || fullName.StartsWith("Internal.")) {
            return "_" + fullName;
        }

        return FixBadChars(fullName);
    }

    static public NamespaceDeclarationSyntax ExtractNamespaceFromType(REFrameworkNET.TypeDefinition t) {
        var ns = t.GetNamespace();

        if (ns is null || !ns.Any()) {
            return SyntaxTreeBuilder.CreateNamespace("_");
        }

        if (ns.StartsWith("System") || ns.StartsWith("Internal"))
            ns = "_" + ns;
        return SyntaxTreeBuilder.CreateNamespace(ns);
    }

    public static ConcurrentDictionary<uint, bool> generatedTypes = [];

    public static REFrameworkNET.TypeDefinition? GetEquivalentNestedTypeInParent(REFrameworkNET.TypeDefinition nestedT) {
        var isolatedNestedName = nestedT.FullName?.Split('.').Last();

        var t = nestedT.DeclaringType;

        if (t == null) {
            return null;
        }

        // Add the "new" keyword if this nested type is anywhere in the hierarchy
        for (var parent = t.ParentType; parent != null; parent = parent.ParentType) {
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

    static CompilationUnitSyntax? MakeFromTypeEntry(REFrameworkNET.TDB context, REFrameworkNET.TypeDefinition? t) {

        if (t == null) {
            Console.WriteLine("Failed to find type");
            return null;
        }

        if (t.DeclaringType != null) return null;
        if (!generatedTypes.TryAdd(t.Index, true)) return null;
        // if (generatedTypes.Contains(t.Index)) return null;
        // generatedTypes.Add(t.Index);
        var genNamespace = ExtractNamespaceFromType(t);
        if (genNamespace is null) {
            API.LogInfo($"Failed to create namespace for {t.FullName}");
            return null;
        }

        var member = TypeHandler.GenerateType(t);
        if (member is null) return null;
        genNamespace = genNamespace.AddMembers(member);
        return SyntaxFactory.CompilationUnit().AddMembers(genNamespace);
    }

    [REFrameworkNET.Attributes.PluginEntryPoint]
    public static List<REFrameworkNET.Compiler.DynamicAssemblyBytecode> Main() {
        try {
            return MainImpl();
        } catch (Exception e) {
            API.LogError("Exception: " + e);

            var ex = e;
            while (ex.InnerException != null) {
                ex = ex.InnerException;
                API.LogError("Inner Exception: " + ex);
            }
        }
        Environment.Exit(0);

        return [];
    }

    public static REFrameworkNET.Compiler.DynamicAssemblyBytecode? GenerateForAssembly(REFrameworkNET.Module assembly, List<REFrameworkNET.Compiler.DynamicAssemblyBytecode> previousCompilations) {
        //var strippedAssemblyName = assembly.get_FullName().Split(',')[0];
        var strippedAssemblyName = assembly.AssemblyName;

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

        if (strippedAssemblyName == "System.Private.CoreLib") {
            strippedAssemblyName = "_System.Private.CoreLib";
        }

        REFrameworkNET.API.LogInfo("Generating assembly " + strippedAssemblyName);

        var tdb = REFrameworkNET.API.GetTDB();

        List<dynamic> typeList = [];

        foreach (var tIndex in assembly.Types) {
            var t = tdb.GetType(tIndex);

            if (t == null) {
                Console.WriteLine("Failed to get type " + tIndex);
                continue;
            }

            dynamic runtimeType = t.GetRuntimeType();

            if (runtimeType == null) {
                Console.WriteLine("Failed to get runtime type for " + t.GetFullName());
                continue;
            }

            // We don't want array types, pointers, etc
            // Assembly.GetTypes usually filters this out but we have to manually do it
            if (runtimeType.IsPointerImpl() == false && runtimeType.IsByRefImpl() == false) {
                typeList.Add(runtimeType);
            }
        }
        
        HashSet<dynamic> delegateList = new();
        // Special case System.Action and System.Func, some of the generic implementations are dangling outside of modules
        if (strippedAssemblyName.StartsWith("_System")) {
            foreach (dynamic t in tdb.Types) {
                if (t is not TypeDefinition type) continue;
                var name = type.FullName ?? "";
                if (!(name.StartsWith("System.Action") || name.StartsWith("System.Func")))
                    continue;
                if (!type.IsGenericType()) continue;
                var def = type.GetGenericTypeDefinition();
                if (delegateList.Add(def.GetRuntimeType())) {
                    API.LogInfo($"Added delegate {def.FullName}({def.Name}):{def.Index} ({def.GetNamespace()})");
                    API.LogInfo($"Name: {TypeHandler.ProperType(def)}");
                }

            }
        }
        typeList.AddRange(delegateList);

        // Clean up all the local objects
        // Mainly because some of the older games don't play well with a ton of objects on the thread local heap
        REFrameworkNET.API.LocalFrameGC();

        int count = typeList.Count;
        var syntaxTreeParseOption = CSharpParseOptions.Default.WithLanguageVersion(LanguageVersion.CSharp12);

        var syntaxTrees = typeList
            // .AsParallel()  /// Causes memory violations for now, not exactly sure why
            .Select((dynamic reEngineT) => {
                var th = reEngineT.get_TypeHandle();
                var thisCount = Interlocked.Decrement(ref count);
                if (thisCount % 1000 == 0) Console.WriteLine($"{thisCount} remaining");

                if (th == null) {
                    API.LogInfo("Failed to get type handle for " + reEngineT.get_FullName());
                    return null;
                }

                var t = th as REFrameworkNET.TypeDefinition;
                if (t == null) {
                    API.LogError("Failed to convert type handle for " + reEngineT.get_FullName());
                    return null;
                }

                var properType = TypeHandler.ProperType(t);
                var sanitizedTypeName = properType
                    .ToFullString()
                    .Replace('<', '_')
                    .Replace('>', '_')
                    .Replace(':', '_');
                var compilationUnit = MakeFromTypeEntry(tdb, t);
                if (compilationUnit is null) return null;

                // Clean up all the local objects
                // Mainly because some of the older games don't play well with a ton of objects on the thread local heap
                // REFrameworkNET.API.LocalFrameGC();
                return SyntaxFactory.SyntaxTree(
                    compilationUnit.NormalizeWhitespace(),
                    syntaxTreeParseOption,
                    $"{sanitizedTypeName}.cs"
                );
            })
            .Where(s => s is not null)
            .Select(s => s!)
            .ToList();




        string? assemblyPath = Path.GetDirectoryName(typeof(object).Assembly.Location);
        var references = REFrameworkNET.Compiler.GenerateExhaustiveMetadataReferences(typeof(REFrameworkNET.API).Assembly, new List<Assembly>());

        // Add the previous compilations as references
        foreach (var compilationbc in previousCompilations) {
            var ms = new MemoryStream(compilationbc.Bytecode);
            references.Add(MetadataReference.CreateFromStream(ms));
        }

        //compilationUnit = compilationUnit.AddUsings(SyntaxFactory.UsingDirective(SyntaxFactory.ParseName("System")));

        System.Console.WriteLine("Compiling " + strippedAssemblyName + " with " + syntaxTrees.Count + " syntax trees...");

        var csoptions = new CSharpCompilationOptions(OutputKind.DynamicallyLinkedLibrary, 
            optimizationLevel: OptimizationLevel.Release,
            assemblyIdentityComparer: DesktopAssemblyIdentityComparer.Default,
            platform: Platform.X64,
            allowUnsafe: true
        );
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
                const string DEBUG_PATH = "reframework/debug-src";
                bool debugOut = Directory.Exists(DEBUG_PATH);
                if (debugOut) {
                    foreach (var f in Directory.EnumerateFiles(DEBUG_PATH)) {
                        File.Delete(f);
                    }
                }
                //var textLines = syntaxTrees.GetText().Lines;
                List<(SyntaxTree, List<Diagnostic>)> sortedDiagnostics = result.Diagnostics
                    .OrderBy(d => (d.Location.SourceTree?.FilePath, d.Location.SourceSpan.Start))
                    .GroupBy(d => d.Location.SourceTree)
                    .Select(g => (g.Key!, g.ToList()))
                    .ToList();
                foreach (var (tree, diags) in sortedDiagnostics) {
                    var textLines = tree.GetText().Lines;
                    var errors = "\n";

                    foreach (var diagnostic in diags) {
                        var lineSpan = diagnostic.Location.GetLineSpan();
                        var errorLineNumber = lineSpan.StartLinePosition.Line;
                        var errorLineText = textLines?[errorLineNumber].ToString();
                        API.LogError($"{diagnostic.Id}: {diagnostic.GetMessage()}");
                        API.LogError($"Error in {tree.FilePath}:{errorLineNumber + 1}: {errorLineText}");
                        errors += $"/* at {errorLineNumber + 1}: {diagnostic.Id}: {diagnostic.GetMessage()} */\n";
                    }

                    if (debugOut) {
                        File.WriteAllText($"{DEBUG_PATH}/{tree.FilePath}", tree.GetText().ToString() + errors);
                    }
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
        TypeHandler.BuildProperTypes();

        List<REFrameworkNET.Module> modules = [];

        // First module is an invalid module
        for (uint i = 0; i < tdb.GetNumModules(); i++) {
            var module = tdb.GetModule(i);

            if (module == null) {
                continue;
            }
            modules.Add(module);
        }

        List<REFrameworkNET.Compiler.DynamicAssemblyBytecode> bytecodes = [];

        foreach (Module module in modules) {
            var assemblyName = module.AssemblyName;
            var location = module.Location;
            var moduleName = module.ModuleName;

            if (assemblyName == null || location == null || moduleName == null) {
                continue;
            }

            if (assemblyName == "") continue;
            // if (assemblyName.Contains("application")) continue;
            // if (assemblyName.Contains("viacore")) continue;

            REFrameworkNET.API.LogInfo("Assembly: " + assemblyName);
            REFrameworkNET.API.LogInfo("Location: " + location);
            REFrameworkNET.API.LogInfo("Module: " + moduleName);

            var bytecode = GenerateForAssembly(module, bytecodes);

            if (bytecode is null) throw new Exception("Failed");
            bytecodes.Add(bytecode);

            // Clean up all the local objects
            // Mainly because some of the older games don't play well with a ton of objects on the thread local heap
            REFrameworkNET.API.LocalFrameGC();
        }
        return bytecodes;
    }
};
}