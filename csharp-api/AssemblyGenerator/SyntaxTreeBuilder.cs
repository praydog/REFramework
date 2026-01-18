using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using System.Collections.Generic;

public static class SyntaxTreeBuilder {
    public static NamespaceDeclarationSyntax CreateNamespace(string namespaceName) {
        return SyntaxFactory.NamespaceDeclaration(SyntaxFactory.ParseName(namespaceName)).NormalizeWhitespace();
    }

    public static ClassDeclarationSyntax CreateClass(string className) {
        return SyntaxFactory.ClassDeclaration(className)
            .AddModifiers(SyntaxFactory.Token(SyntaxKind.PublicKeyword));
    }

    public static MethodDeclarationSyntax CreateMethod(string methodName) {
        return SyntaxFactory.MethodDeclaration(SyntaxFactory.PredefinedType(SyntaxFactory.Token(SyntaxKind.VoidKeyword)), methodName)
            .AddModifiers(SyntaxFactory.Token(SyntaxKind.PublicKeyword))
            .WithBody(SyntaxFactory.Block());
    }

    public static CompilationUnitSyntax AddMembersToCompilationUnit(CompilationUnitSyntax compilationUnit, params MemberDeclarationSyntax[] members) {
        return compilationUnit.AddMembers(members);
    }

    public static NamespaceDeclarationSyntax AddMembersToNamespace(NamespaceDeclarationSyntax namespaceDeclaration, params MemberDeclarationSyntax[] members) {
        return namespaceDeclaration.AddMembers(members);
    }

    public static ClassDeclarationSyntax AddMethodsToClass(ClassDeclarationSyntax classDeclaration, params MethodDeclarationSyntax[] methods) {
        return classDeclaration.AddMembers(methods);
    }
}
