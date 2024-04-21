# Script to deploy symlinks from our game folder to our build folder for various DLLs and other files

import fire
import os
import ctypes
import shutil

def symlink_main(gamedir=None, bindir="build/bin", just_copy=False):
    if gamedir is None:
        print("Usage: make_symlinks.py --gamedir=<path to game directory>")
        return
    
    # Throw an error if the user is not a privileged user
    try:
        is_admin = os.getuid() == 0
    except AttributeError:
        is_admin = ctypes.windll.shell32.IsUserAnAdmin() != 0

    if not is_admin and not just_copy:
        print("Error: This script must be run as an administrator")
        return
    
    # Get the current working directory
    if not os.path.exists(bindir):
        print(f"Error: Directory {bindir} does not exist")
        return
    
    source_dir_files = [
        "Test/Test/Test.cs",
        "Test/Test/TestRE2.cs",
        "Test/Test/ObjectExplorer.cs",
    ]

    for file in source_dir_files:
        src = os.path.abspath(file)
        filename_only = os.path.basename(file)
        dst = os.path.join(gamedir, "reframework", "plugins", "source", filename_only)
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        try:
            os.remove(dst)
        except FileNotFoundError:
            pass

        if just_copy == True:
            shutil.copy(src, dst)
        else:
            os.symlink(src, dst)

    plugins_dir_files = [
        "REFramework.NET.dll",
        "REFramework.NET.runtimeconfig.json",
        "REFramework.NET.xml",
        "Ijwhost.dll",
    ]

    for file in plugins_dir_files:
        src = os.path.join(bindir, file)
        src = os.path.abspath(src)
        dst = os.path.join(gamedir, "reframework", "plugins", file)
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        try:
            os.remove(dst)
        except FileNotFoundError:
            pass

        if just_copy == True:
            shutil.copy(src, dst)
        else:
            os.symlink(src, dst)

    dependencies_dir_files = [
        "AssemblyGenerator.dll",
        "REFCoreDeps.dll",
        "Microsoft.CodeAnalysis.CSharp.dll",
        "Microsoft.CodeAnalysis.dll",
        "Microsoft.CodeAnalysis.CSharp.xml",
        "Microsoft.CodeAnalysis.xml",
        "ImGui.NET.dll",
        "ImGui.NET.xml",
    ]

    for file in dependencies_dir_files:
        src = os.path.join(bindir, file)
        src = os.path.abspath(src)
        dst = os.path.join(gamedir, "reframework", "plugins", "managed", "dependencies", file)
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        try:
            os.remove(dst)
        except FileNotFoundError:
            pass

        if just_copy == True:
            shutil.copy(src, dst)
        else:
            os.symlink(src, dst)

    print("Symlinks created successfully")

if __name__ == '__main__':
    fire.Fire(symlink_main)