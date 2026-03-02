using System.IO.Pipes;
using System.Text;
using System.Text.Json;

namespace REFrameworkMcp;

static class Pipe
{
    static readonly Encoding Utf8NoBom = new UTF8Encoding(false);
    static NamedPipeClientStream? _pipe;
    static StreamReader? _reader;
    static StreamWriter? _writer;
    static int _nextId = 1;

    public static bool IsAvailable => _pipe is { IsConnected: true };

    static async Task<bool> EnsureConnected()
    {
        if (_pipe is { IsConnected: true })
            return true;

        Disconnect();

        try
        {
            _pipe = new NamedPipeClientStream(".", "REFrameworkNET", PipeDirection.InOut, PipeOptions.Asynchronous);
            await _pipe.ConnectAsync(1000); // 1-second timeout
            _reader = new StreamReader(_pipe, Utf8NoBom, detectEncodingFromByteOrderMarks: false, leaveOpen: true);
            _writer = new StreamWriter(_pipe, Utf8NoBom, leaveOpen: true) { AutoFlush = true };
            return true;
        }
        catch
        {
            Disconnect();
            return false;
        }
    }

    static void Disconnect()
    {
        try { _reader?.Dispose(); } catch { }
        try { _writer?.Dispose(); } catch { }
        try { _pipe?.Dispose(); } catch { }
        _reader = null;
        _writer = null;
        _pipe = null;
    }

    /// <summary>
    /// Send a request and get the JSON response string.
    /// Returns null if the pipe is not available.
    /// </summary>
    public static Task<string?> Request(string method) => Request(method, null, 5000);

    /// <summary>
    /// Send a request with extra parameters and a custom read timeout.
    /// </summary>
    public static async Task<string?> Request(string method, Dictionary<string, object>? extra, int readTimeoutMs = 5000)
    {
        if (!await EnsureConnected())
            return null;

        var id = Interlocked.Increment(ref _nextId);

        // Build request object with method, id, and any extra params
        var requestDict = new Dictionary<string, object> { ["method"] = method, ["id"] = id };
        if (extra != null)
            foreach (var kv in extra)
                requestDict[kv.Key] = kv.Value;

        var request = JsonSerializer.Serialize(requestDict);

        try
        {
            await _writer!.WriteLineAsync(request);
            await _writer.FlushAsync();
            _pipe!.Flush();

            // Read with timeout to avoid hanging forever
            using var cts = new CancellationTokenSource(readTimeoutMs);
            var response = await _reader!.ReadLineAsync(cts.Token);
            if (response == null)
            {
                Disconnect();
                return null;
            }

            // Extract just the "result" or "error" from the response
            using var doc = JsonDocument.Parse(response);
            var root = doc.RootElement;

            if (root.TryGetProperty("error", out var err))
                return JsonSerializer.Serialize(new { error = err.GetString() });

            if (root.TryGetProperty("result", out var result))
                return result.GetRawText();

            return response;
        }
        catch
        {
            Disconnect();
            return null;
        }
    }
}
