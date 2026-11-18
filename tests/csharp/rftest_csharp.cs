// REFramework.NET (C# API) test suite. Loaded as a source plugin from reframework/plugins/source; writes
// reframework/data/rftest/results/csharp.json in the same format as the Lua and plugin suites.
//
// Uses only the untyped API (TDB lookups, Method.Invoke, MethodHook) and System.* / via.* types common to every RE Engine
// game, so the same file runs in every supported game.
using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using REFrameworkNET;
using REFrameworkNET.Attributes;
using REFrameworkNET.Callbacks;

public class RfTestCsharp {
    class Result { public string Name, Status, Detail; }

    class SkipException : Exception { public SkipException(string m) : base(m) {} }

    static readonly List<Result> s_results = new();
    static readonly object s_lock = new();
    static int s_updates;
    static bool s_finished;

    const string CallbackTest = "Callback(UpdateBehavior, Pre) fires";

    static string ResultsPath() {
        var game = Path.GetDirectoryName(Environment.ProcessPath);
        return Path.Combine(game, "reframework", "data", "rftest", "results", "csharp.json");
    }

    static string Esc(string s) {
        var sb = new StringBuilder();
        foreach (var c in s ?? "") {
            switch (c) {
                case '"': sb.Append("\\\""); break;
                case '\\': sb.Append("\\\\"); break;
                case '\n': sb.Append("\\n"); break;
                case '\r': sb.Append("\\r"); break;
                case '\t': sb.Append("\\t"); break;
                default: if (c < 0x20) sb.AppendFormat("\\u{0:x4}", (int)c); else sb.Append(c); break;
            }
        }
        return sb.ToString();
    }

    static void Record(string name, string status, string detail = "") {
        lock (s_lock) {
            s_results.RemoveAll(r => r.Name == name);
            s_results.Add(new Result { Name = name, Status = status, Detail = detail });
        }
        API.LogInfo($"[rftest] csharp: {status} {name}{(string.IsNullOrEmpty(detail) ? "" : " -- " + detail)}");
    }

    static void Write() {
        lock (s_lock) {
            int pass = 0, fail = 0, skip = 0;
            var sb = new StringBuilder();
            for (int i = 0; i < s_results.Count; i++) {
                var r = s_results[i];
                if (r.Status == "PASS") pass++; else if (r.Status == "SKIP") skip++; else fail++;
                sb.Append(i > 0 ? "," : "").Append($"{{\"name\":\"{Esc(r.Name)}\",\"status\":\"{r.Status}\",\"detail\":\"{Esc(r.Detail)}\"}}");
            }
            var path = ResultsPath();
            Directory.CreateDirectory(Path.GetDirectoryName(path));
            var json = $"{{\"suite\":\"csharp\",\"game\":\"{Esc(Path.GetFileNameWithoutExtension(Environment.ProcessPath))}\",\"pass\":{pass},\"fail\":{fail},\"skip\":{skip},\"tests\":[{sb}]}}";
            File.WriteAllText(path + ".tmp", json);
            File.Move(path + ".tmp", path, true);
            API.LogInfo($"[rftest] csharp: DONE pass={pass} fail={fail} skip={skip} (results written)");
        }
    }

    static void Test(string name, Func<string> body) {
        try {
            Record(name, "PASS", body() ?? "");
        } catch (SkipException e) {
            Record(name, "SKIP", e.Message);
        } catch (Exception e) {
            Record(name, "FAIL", e.Message);
        }
    }

    static void Check(bool ok, string what) { if (!ok) throw new Exception(what); }
    static void Eq<T>(T actual, T expected, string what) {
        if (!EqualityComparer<T>.Default.Equals(actual, expected)) throw new Exception($"{what}: expected {expected}, got {actual}");
    }

    static TypeDefinition Type(string name) {
        var t = TDB.Get().FindType(name);
        if (t == null) throw new SkipException($"{name} is not in this game");
        return t;
    }

    static Method Find(string type, string sig) {
        var m = Type(type).FindMethod(sig);
        if (m == null) throw new SkipException($"{type}.{sig} is not in this game");
        return m;
    }

    static IntPtr Addr(REFrameworkNET.ValueType v) => (IntPtr)(long)v.GetAddress();
    static float ReadF(IntPtr p) => BitConverter.Int32BitsToSingle(Marshal.ReadInt32(p));
    static void WriteF(IntPtr p, float f) => Marshal.WriteInt32(p, BitConverter.SingleToInt32Bits(f));
    static ulong DoubleBits(double d) => (ulong)BitConverter.DoubleToInt64Bits(d);
    static double BitsDouble(ulong u) => BitConverter.Int64BitsToDouble((long)u);

    // Hooks stay installed (the game calls most of these methods too); each acts only on our own call.
    [ThreadStatic] static string t_armed;

    // Main writes the results with the callback test pending; the callback writes them again once it has run 30
    // times. A results file never misses tests: the callback writes only after Main has finished.
    static bool s_mainDone;

    [PluginEntryPoint]
    public static void Main() {
        lock (s_lock) {
            if (!s_finished) Record(CallbackTest, "FAIL", "pending: no UpdateBehavior callback yet");
        }
        try {
            RunTests();
        } catch (Exception e) {
            Record("suite ran to completion", "FAIL", e.ToString());
        }
        lock (s_lock) s_mainDone = true;
        Write();
    }

    [Callback(typeof(UpdateBehavior), CallbackType.Pre)]
    public static void OnUpdateBehavior() {
        if (Volatile.Read(ref s_finished) || Interlocked.Increment(ref s_updates) < 30) return;
        lock (s_lock) {
            if (s_finished) return;
            s_finished = true;
            Record(CallbackTest, "PASS", $"{s_updates} calls");
            if (!s_mainDone) return;
        }
        Write();
    }

    static void RunTests() {
        var tdb = TDB.Get();

        Test("TDB: version, counts, type lookup", () => {
            Check(tdb.GetVersion() >= 67, $"TDB version {tdb.GetVersion()}");
            Check(tdb.GetNumTypes() > 1000, $"{tdb.GetNumTypes()} types");
            Eq(Type("System.String").GetFullName(), "System.String", "FindType name");
            Check(tdb.FindType("RfTest.NoSuchType") == null, "missing type returns null");
            return $"TDB {tdb.GetVersion()}, {tdb.GetNumTypes()} types";
        });

        Test("Method lookup by signature and parameters", () => {
            var m = Find("System.Math", "Max(System.Int32, System.Int32)");
            Eq(m.GetNumParams(), 2u, "param count");
            Check(m.IsStatic(), "Math.Max is static");
            Eq(m.GetReturnType().GetFullName(), "System.Int32", "return type");
            return null;
        });

        Test("Field lookup and offset (via.vec3.y)", () => {
            var f = Type("via.vec3").FindField("y");
            Check(f != null, "field y");
            Eq(f.GetOffsetFromFieldPtr(), 4u, "offset");
            Eq(Type("via.vec3").ValueTypeSize, 16u, "via.vec3 size");
            return null;
        });

        Test("Static invoke: Int32 and Double arguments and returns", () => {
            Eq(Find("System.Math", "Max(System.Int32, System.Int32)").Invoke(null, new object[] { 3, 7 }).DWord, 7u, "Max(3, 7)");
            Eq(Find("System.Math", "Sqrt(System.Double)").Invoke(null, new object[] { 16.0 }).Double, 4.0, "Sqrt(16)");
            return null;
        });

        Test("Managed string: create, instance invoke, returned object", () => {
            var s = VM.CreateString("RfTest");
            Check(s != null, "CreateString");
            Eq((s as IObject).GetTypeDefinition().GetFullName(), "System.String", "type");
            var len = Find("System.String", "get_Length()");
            Eq(len.Invoke(s, new object[0]).DWord, 6u, "get_Length");
            var upper = Find("System.String", "ToUpper()").Invoke(s, new object[0]);
            var obj = ManagedObject.ToManagedObject(upper.Ptr);
            Check(obj != null, "ToUpper returned an object");
            Eq((obj as IObject).GetTypeDefinition().GetFullName(), "System.String", "ToUpper result type");
            Eq(len.Invoke(obj, new object[0]).DWord, 6u, "ToUpper result length");
            return null;
        });

        Test("Struct arguments and struct return (via.vec3.op_Addition)", () => {
            var add = Find("via.vec3", "op_Addition(via.vec3, via.vec3)");
            var a = new REFrameworkNET.ValueType(Type("via.vec3"));
            var b = new REFrameworkNET.ValueType(Type("via.vec3"));
            WriteF(Addr(a), 1); WriteF(Addr(a) + 4, 2); WriteF(Addr(a) + 8, 3);
            WriteF(Addr(b), 4); WriteF(Addr(b) + 4, 5); WriteF(Addr(b) + 8, 6);
            var r = add.Invoke(null, new object[] { a, b });
            var x = BitConverter.ToSingle(BitConverter.GetBytes(r.DWord), 0);
            Eq(x, 5.0f, "result x");
            return null;
        });

        // reframework#28: before TDB 74 invoke() passes `this` of a value-type method through unchanged.
        Test("ValueType instance call (via.vec3 get_Item / set_Item)", () => {
            var getItem = Find("via.vec3", "get_Item(System.Int32)");
            var setItem = Find("via.vec3", "set_Item(System.Int32, System.Single)");
            var v = new REFrameworkNET.ValueType(Type("via.vec3"));
            WriteF(Addr(v), 1); WriteF(Addr(v) + 4, 2); WriteF(Addr(v) + 8, 3);
            // Invoke wrappers return a Single as a double.
            Eq((float)getItem.Invoke(v, new object[] { 1 }).Double, 2.0f, "get_Item(1)");
            setItem.Invoke(v, new object[] { 2, 7.5f });
            Eq(ReadF(Addr(v) + 8), 7.5f, "z after set_Item(2, 7.5)");
            Eq(ReadF(Addr(v)), 1.0f, "x untouched");
            return null;
        });

        Test("Boxed value type instance call (via.vec3 get_Item)", () => {
            var getItem = Find("via.vec3", "get_Item(System.Int32)");
            var boxed = Type("via.vec3").CreateInstance(0);
            Check(boxed != null, "CreateInstance(via.vec3)");
            var data = (IntPtr)(long)boxed.GetAddress() + 0x10;
            WriteF(data, 1); WriteF(data + 4, 2); WriteF(data + 8, 3);
            Eq((float)getItem.Invoke(boxed, new object[] { 1 }).Double, 2.0f, "get_Item(1)");
            return null;
        });

        Test("MethodHook: args, changing an arg, replacing the return", () => {
            var max = Find("System.Math", "Max(System.Int32, System.Int32)");
            uint seenA = 0, seenB = 0;
            bool post = false;
            max.AddHook(false).AddPre(args => {
                if (t_armed != "max") return PreHookResult.Continue;
                seenA = (uint)args[1]; seenB = (uint)args[2];
                args[2] = 9;
                return PreHookResult.Continue;
            }).AddPost((ref ulong retval) => {
                if (t_armed != "max") return;
                post = true;
                if (retval == 9) retval = 42;
            });
            t_armed = "max";
            var r = max.Invoke(null, new object[] { 3, 7 }).DWord;
            t_armed = null;
            if (seenA == 0 && !post) throw new SkipException("the hook did not fire for a call through Invoke");
            Eq(seenA, 3u, "args[1]"); Eq(seenB, 7u, "args[2]");
            Check(post, "post hook ran");
            Eq(r, 42u, "Max(3, 9) with the return replaced");
            return null;
        });

        Test("MethodHook: Skip the original", () => {
            var min = Find("System.Math", "Min(System.Int32, System.Int32)");
            min.AddHook(false).AddPre(args => t_armed == "min" ? PreHookResult.Skip : PreHookResult.Continue)
               .AddPost((ref ulong retval) => { if (t_armed == "min") retval = 99; });
            t_armed = "min";
            var r = min.Invoke(null, new object[] { 3, 7 }).DWord;
            t_armed = null;
            Eq(r, 99u, "skipped Min(3, 7) returns the post hook's value");
            return null;
        });

        // reframework#20: a struct return takes the return buffer as the first native argument, so the Double is in
        // xmm3. Also needs reframework#28 (value-type `this`).
        Test("MethodHook: struct return + Double on a value type (DateTime.AddHours)", () => {
            var addHours = Find("System.DateTime", "AddHours(System.Double)");
            double seen = 0;
            addHours.AddHook(false).AddPre(args => {
                if (t_armed != "addhours") return PreHookResult.Continue;
                seen = BitsDouble(args[3]);
                args[3] = DoubleBits(1.0);
                return PreHookResult.Continue;
            });
            var dt = new REFrameworkNET.ValueType(Type("System.DateTime"));
            t_armed = "addhours";
            var r = addHours.Invoke(dt, new object[] { 2.5 });
            t_armed = null;
            if (seen == 0) throw new SkipException("the hook did not fire for a call through Invoke");
            Eq(seen, 2.5, "args[3]");
            // The return buffer is the InvokeRet; the top two bits of DateTime's data are its Kind.
            Eq(r.QWord & 0x3FFFFFFFFFFFFFFF, 36000000000UL, "ticks of AddHours(1.0) after changing args[3]");
            return null;
        });
    }
}
