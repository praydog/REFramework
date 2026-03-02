using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Pipes;
using System.Text;
using System.Text.Json;
using System.Threading;

namespace REFrameworkNET
{
    public class PipeServer
    {
        // --- Ring buffer entries ---

        struct ErrorEntry
        {
            public string Message;
            public string Timestamp;
        }

        struct LogEntry
        {
            public string Level;
            public string Message;
            public string Timestamp;
        }

        // --- Ring buffers ---

        static readonly object s_lock = new();
        static readonly List<ErrorEntry> s_errors = new();
        static readonly List<LogEntry> s_logEntries = new();
        const int MaxErrors = 200;
        const int MaxLogEntries = 500;

        // --- Compile cycle tracking ---
        static int s_compileCycleId = 0;
        static readonly ManualResetEventSlim s_compileDone = new(true); // starts signaled (no compile in progress)

        // --- Game info (set once during init) ---

        public static string GameExe { get; set; }
        public static string GameDir { get; set; }
        public static string WorkingDir { get; set; }
        public static string PluginsDir { get; set; }
        public static string FrameworkVersion { get; set; }

        // --- Plugin state accessor (set by C++/CLI) ---

        public static Func<string> GetPluginStatesJson { get; set; }

        // --- Lifecycle ---

        static Thread s_thread;
        static CancellationTokenSource s_cts;

        public static void Start()
        {
            if (s_thread != null) return;

            s_cts = new CancellationTokenSource();
            s_thread = new Thread(ServerLoop)
            {
                IsBackground = true,
                Name = "REFrameworkNET.PipeServer"
            };
            s_thread.Start();
            Console.WriteLine("[PipeServer] Started");
        }

        public static void Stop()
        {
            s_cts?.Cancel();
            s_thread = null;
            Console.WriteLine("[PipeServer] Stopped");
        }

        // --- Public API to push data ---

        public static void BeginCompileCycle()
        {
            lock (s_lock)
            {
                s_compileCycleId++;
                s_errors.Clear();
            }
            s_compileDone.Reset();
        }

        public static void EndCompileCycle()
        {
            s_compileDone.Set();
        }

        public static void AddError(string message)
        {
            var entry = new ErrorEntry
            {
                Message = message,
                Timestamp = DateTime.UtcNow.ToString("o")
            };
            lock (s_lock)
            {
                if (s_errors.Count >= MaxErrors)
                    s_errors.RemoveAt(0);
                s_errors.Add(entry);
            }
        }

        public static void AddLog(string level, string message)
        {
            var entry = new LogEntry
            {
                Level = level,
                Message = message,
                Timestamp = DateTime.UtcNow.ToString("o")
            };
            lock (s_lock)
            {
                if (s_logEntries.Count >= MaxLogEntries)
                    s_logEntries.RemoveAt(0);
                s_logEntries.Add(entry);
            }
        }

        // --- Server loop ---

        const int MaxPipeInstances = 4;

        static void ServerLoop()
        {
            var token = s_cts.Token;

            while (!token.IsCancellationRequested)
            {
                NamedPipeServerStream pipe = null;
                try
                {
                    pipe = new NamedPipeServerStream(
                        "REFrameworkNET",
                        PipeDirection.InOut,
                        MaxPipeInstances,
                        PipeTransmissionMode.Byte,
                        PipeOptions.Asynchronous);

                    // Wait for connection (check cancellation periodically)
                    var connectTask = pipe.WaitForConnectionAsync(token);
                    connectTask.Wait(token);

                    Console.WriteLine("[PipeServer] Client connected");

                    // Handle client on a separate thread so we can accept more connections
                    var clientPipe = pipe;
                    pipe = null; // prevent finally from disposing
                    var clientThread = new Thread(() => {
                        try { HandleClient(clientPipe, token); }
                        finally { try { clientPipe.Dispose(); } catch { } }
                    }) { IsBackground = true, Name = "PipeServer.Client" };
                    clientThread.Start();
                }
                catch (OperationCanceledException)
                {
                    break;
                }
                catch (Exception ex)
                {
                    Console.WriteLine("[PipeServer] Error: " + ex.Message);
                    Thread.Sleep(500); // avoid tight loop on repeated errors
                }
                finally
                {
                    try { pipe?.Dispose(); } catch { }
                }
            }
        }

        static readonly Encoding s_utf8NoBom = new UTF8Encoding(false);

        static void HandleClient(NamedPipeServerStream pipe, CancellationToken token)
        {
            using var reader = new StreamReader(pipe, s_utf8NoBom, detectEncodingFromByteOrderMarks: false, leaveOpen: true);
            using var writer = new StreamWriter(pipe, s_utf8NoBom, leaveOpen: true) { AutoFlush = true };

            while (!token.IsCancellationRequested && pipe.IsConnected)
            {
                string line;
                try
                {
                    line = reader.ReadLine();
                }
                catch
                {
                    break; // pipe broken
                }

                if (line == null) break; // client disconnected

                line = line.Trim();
                if (line.Length == 0) continue;

                string response;
                try
                {
                    response = DispatchRequest(line);
                }
                catch (Exception ex)
                {
                    response = JsonSerializer.Serialize(new { id = 0, error = ex.Message });
                }

                try
                {
                    writer.WriteLine(response);
                    pipe.Flush();
                }
                catch
                {
                    break; // pipe broken
                }
            }

            Console.WriteLine("[PipeServer] Client disconnected");
        }

        // --- Request dispatch ---

        static string DispatchRequest(string json)
        {
            using var doc = JsonDocument.Parse(json);
            var root = doc.RootElement;

            int id = 0;
            if (root.TryGetProperty("id", out var idProp))
                id = idProp.GetInt32();

            string method = "";
            if (root.TryGetProperty("method", out var methodProp))
                method = methodProp.GetString() ?? "";

            object result;
            try
            {
                result = method switch
                {
                    "ping" => HandlePing(),
                    "get_info" => HandleGetInfo(),
                    "get_errors" => HandleGetErrors(),
                    "get_log" => HandleGetLog(),
                    "get_plugins" => HandleGetPlugins(),
                    "clear_errors" => HandleClearErrors(),
                    "clear_log" => HandleClearLog(),
                    "compile_status" => HandleCompileStatus(),
                    "wait_compile" => HandleWaitCompile(root),
                    _ => throw new Exception($"Unknown method: {method}")
                };
            }
            catch (Exception ex)
            {
                return JsonSerializer.Serialize(new { id, error = ex.Message });
            }

            return JsonSerializer.Serialize(new { id, result });
        }

        static object HandlePing()
        {
            return new { status = "ok" };
        }

        static object HandleGetInfo()
        {
            return new
            {
                gameExe = GameExe ?? "",
                gameDir = GameDir ?? "",
                workingDir = WorkingDir ?? "",
                pluginsDir = PluginsDir ?? "",
                frameworkVersion = FrameworkVersion ?? ""
            };
        }

        static object HandleGetErrors()
        {
            lock (s_lock)
            {
                var list = new List<object>(s_errors.Count);
                foreach (var e in s_errors)
                    list.Add(new { message = e.Message, timestamp = e.Timestamp });
                return new { errors = list, count = list.Count, compileCycleId = s_compileCycleId };
            }
        }

        static object HandleGetLog()
        {
            lock (s_lock)
            {
                var list = new List<object>(s_logEntries.Count);
                foreach (var e in s_logEntries)
                    list.Add(new { level = e.Level, message = e.Message, timestamp = e.Timestamp });
                return new { entries = list, count = list.Count };
            }
        }

        static object HandleClearErrors()
        {
            lock (s_lock)
            {
                var count = s_errors.Count;
                s_errors.Clear();
                return new { cleared = count };
            }
        }

        static object HandleClearLog()
        {
            lock (s_lock)
            {
                var count = s_logEntries.Count;
                s_logEntries.Clear();
                return new { cleared = count };
            }
        }

        static object HandleCompileStatus()
        {
            lock (s_lock)
            {
                var files = new HashSet<string>();
                foreach (var e in s_errors)
                {
                    // Error format: "filepath(line,col): CSxxxx: message"
                    // Extract filename from the path before the first '('
                    var msg = e.Message;
                    var parenIdx = msg.IndexOf('(');
                    if (parenIdx > 0)
                    {
                        var path = msg.Substring(0, parenIdx);
                        try { files.Add(Path.GetFileName(path)); } catch { }
                    }
                }

                return new
                {
                    compileCycleId = s_compileCycleId,
                    status = s_errors.Count == 0 ? "ok" : "error",
                    errorCount = s_errors.Count,
                    compiling = !s_compileDone.IsSet,
                    files = files
                };
            }
        }

        static object HandleWaitCompile(JsonElement root)
        {
            int timeoutMs = 15000; // default 15 seconds
            if (root.TryGetProperty("timeout", out var timeoutProp))
                timeoutMs = timeoutProp.GetInt32();

            var startCycle = 0;
            lock (s_lock) { startCycle = s_compileCycleId; }

            bool completed = s_compileDone.Wait(timeoutMs);

            lock (s_lock)
            {
                return new
                {
                    compileCycleId = s_compileCycleId,
                    status = s_errors.Count == 0 ? "ok" : "error",
                    errorCount = s_errors.Count,
                    timedOut = !completed
                };
            }
        }

        static object HandleGetPlugins()
        {
            var accessor = GetPluginStatesJson;
            if (accessor == null)
                return new { plugins = new List<object>() };

            // The accessor returns a JSON string - parse and re-embed
            var json = accessor();
            if (string.IsNullOrEmpty(json))
                return new { plugins = new List<object>() };

            using var doc = JsonDocument.Parse(json);
            return new { plugins = doc.RootElement.Clone() };
        }
    }
}
