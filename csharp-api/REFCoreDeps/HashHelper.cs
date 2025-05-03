using System;
using System.IO;
using System.Security.Cryptography;
using System.Text;

namespace REFrameworkNET {
public static class HashHelper
{
    public static string ComputeSHA256(string filePath)
    {
        using (var sha256 = SHA256.Create())
        using (var stream = File.OpenRead(filePath))
        {
            byte[] hash = sha256.ComputeHash(stream);
            return BitConverter.ToString(hash).Replace("-", "").ToLower();
        }
    }
}
}