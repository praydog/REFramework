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

        private static CSharpCompilation GenerateCode(string sourceCode, string filePath, Assembly executingAssembly, List<Assembly> deps)
        {
            var codeString = SourceText.From(sourceCode);
            var options = CSharpParseOptions.Default.WithLanguageVersion(LanguageVersion.CSharp12);
            string assemblyPath = Path.GetDirectoryName(typeof(object).Assembly.Location);
            // get all DLLs in that directory
            var dlls = Directory.GetFiles(assemblyPath, "*.dll");

            var parsedSyntaxTree = SyntaxFactory.ParseSyntaxTree(codeString, options);

            var referencesStr = new System.Collections.Generic.SortedSet<string>
            {
                typeof(object).Assembly.Location,
                typeof(Console).Assembly.Location,
                typeof(System.Linq.Enumerable).Assembly.Location,
                typeof(System.Runtime.AssemblyTargetedPatchBandAttribute).Assembly.Location,
                typeof(Microsoft.CSharp.RuntimeBinder.CSharpArgumentInfo).Assembly.Location,
                executingAssembly.Location
            };

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

            var references = referencesStr.Select(r => MetadataReference.CreateFromFile(r)).ToArray();

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