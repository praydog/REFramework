using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Reflection.Metadata;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.Text;

namespace REFrameworkNET
{
    public class Compiler
    {
        public class DynamicAssemblyBytecode {
            public byte[] Bytecode {
                get;
                set;
            }

            public string AssemblyName {
                get;
                set;
            }
        }

        static public byte[] Compile(string filepath, Assembly executingAssembly, List<Assembly> deps)
        {
            var sourceCode = File.ReadAllText(filepath);

            using (var peStream = new MemoryStream())
            {
                var result = GenerateCode(sourceCode, filepath, executingAssembly, deps).Emit(peStream);

                if (!result.Success)
                {
                    Console.WriteLine("Compilation done with error.");

                    var failures = result.Diagnostics.Where(diagnostic => diagnostic.IsWarningAsError || diagnostic.Severity == DiagnosticSeverity.Error);

                    foreach (var diagnostic in failures)
                    {
                        Console.Error.WriteLine("{0}: {1}", diagnostic.Id, diagnostic.GetMessage());
                    }

                    return null;
                }

                Console.WriteLine("Compilation done without any error.");

                peStream.Seek(0, SeekOrigin.Begin);
                return peStream.ToArray();

            }
        }

        public static List<PortableExecutableReference> GenerateExhaustiveMetadataReferences(Assembly executingAssembly, List<Assembly> deps) {
            string assemblyPath = System.Runtime.InteropServices.RuntimeEnvironment.GetRuntimeDirectory();
            string[] dlls = [];

            System.Console.WriteLine("assemblyPath: " + assemblyPath);

            string targetFramework = System.Runtime.InteropServices.RuntimeInformation.FrameworkDescription.Split(' ')[1].TrimStart('v');
            System.Console.WriteLine("targetFramework: " + targetFramework);

            // Extract only the major + minor out
            string moniker = string.Concat("net", targetFramework.AsSpan(0, targetFramework.LastIndexOf('.')));
            System.Console.WriteLine("moniker: " + moniker);

            // Go backwards from assemblyPath to construct the baseRefDir path
            string dotnetDirectory = Path.GetDirectoryName(Path.GetDirectoryName(Path.GetDirectoryName(Path.GetDirectoryName(assemblyPath)))); // least hacky code am i right
            string baseRefDir = Path.Combine(dotnetDirectory, "packs", "Microsoft.NETCore.App.Ref");

            if (Directory.Exists(baseRefDir)) {
                var refFolder = Path.Combine(baseRefDir, targetFramework, "ref", moniker);

                System.Console.WriteLine("Looking for reference assemblies in " + refFolder);

                if (Directory.Exists(refFolder)) {
                    System.Console.WriteLine("Found reference assemblies in " + refFolder);
                    dlls = Directory.GetFiles(refFolder, "*.dll");
                }
            } else {
                Console.WriteLine("No reference assemblies found in " + baseRefDir);
            }
            
            if (dlls.Length == 0) {
                Console.WriteLine("No reference assemblies found in " + baseRefDir + ". Falling back to implementation assemblies in " + assemblyPath);

                dlls = Directory.GetFiles(assemblyPath, "*.dll");
            }

            var referencesStr = new System.Collections.Generic.SortedSet<string>
            {
                /*typeof(object).Assembly.Location,
                typeof(Console).Assembly.Location,
                typeof(System.Linq.Enumerable).Assembly.Location,
                typeof(System.Runtime.AssemblyTargetedPatchBandAttribute).Assembly.Location,
                typeof(Microsoft.CSharp.RuntimeBinder.CSharpArgumentInfo).Assembly.Location*/
            };

            if (executingAssembly != null) {
                referencesStr.Add(executingAssembly.Location);
            }

            // Add all the dependencies to the references
            foreach (var dep in deps)
            {
                referencesStr.Add(dep.Location);
            }

            // Add all the DLLs to the references
            foreach (var dll in dlls)
            {
                referencesStr.Add(dll);
            }

            referencesStr.RemoveWhere(r =>
            {
                try
                {
                    using var fs = new FileStream(r, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
                    using var peReader = new System.Reflection.PortableExecutable.PEReader(fs);

                    MetadataReader mr = peReader.GetMetadataReader();

                    if (!mr.IsAssembly)
                    {
                        Console.WriteLine("Removed reference: " + r);
                        return true;
                    }
                }
                catch (Exception)
                {
                    Console.WriteLine("Error adding reference: " + r);
                    //Console.WriteLine(e);
                    return true;
                }

                return false;
            });
            return referencesStr.Select(r => MetadataReference.CreateFromFile(r)).ToList();
        }

        private static CSharpCompilation GenerateCode(string sourceCode, string filePath, Assembly executingAssembly, List<Assembly> deps)
        {
            var codeString = SourceText.From(sourceCode);
            var options = CSharpParseOptions.Default.WithLanguageVersion(LanguageVersion.CSharp12);
            var parsedSyntaxTree = SyntaxFactory.ParseSyntaxTree(codeString, options);

            var references = GenerateExhaustiveMetadataReferences(executingAssembly, deps);

            foreach (var reference in references)
            {
                Console.WriteLine(reference.Display);
            }

            // Get only the filename from the path
            var dllName = Path.GetFileNameWithoutExtension(filePath) + ".dll";

            return CSharpCompilation.Create(dllName,
                new[] { parsedSyntaxTree }, 
                references: references, 
                options: new CSharpCompilationOptions(OutputKind.DynamicallyLinkedLibrary, 
                    optimizationLevel: OptimizationLevel.Release,
                    assemblyIdentityComparer: DesktopAssemblyIdentityComparer.Default));
        }
    }
}