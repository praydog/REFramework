using System;
using System.Reflection;
using REFrameworkNET;

var fixture = typeof(IObject).Assembly.GetType("REFrameworkNET.Tests.ValueTypePinning", throwOnError: true)!;
try { fixture.GetMethod("Run")!.Invoke(null, null); }
catch (TargetInvocationException error) { throw error.InnerException!; }
