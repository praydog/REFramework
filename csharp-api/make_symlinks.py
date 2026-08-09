# Script to deploy symlinks from our game folder to our build folder for various DLLs and other files

import fire
import os
import ctypes
import shutil

def symlink_main(gamedir=None, bindir="build/bin", just_copy=False, skip_test_scripts=False):
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
    
    # Always create the source directory so users have a place to drop scripts
    os.makedirs(os.path.join(gamedir, "reframework", "plugins", "source"), exist_ok=True)

    if not skip_test_scripts:
        source_dir_files = [
            "Test/Test/Test.cs",
            "Test/Test/TestDMC5.cs",
            "Test/Test/TestRE2.cs",
            "Test/Test/TestRE4.cs",
            "Test/Test/TestMHWilds.cs",
            "Test/Test/TestWebAPI.cs",
            "Test/Test/TestRE9.cs",
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

        source_dir_dirs = [
            "Test/Test/WebAPI",
        ]

        for dir in source_dir_dirs:
            src = os.path.abspath(dir)
            dirname_only = os.path.basename(dir)
            dst = os.path.join(gamedir, "reframework", "plugins", "source", dirname_only)
            try:
                os.remove(dst)  # remove symlink if exists
            except FileNotFoundError:
                pass
            try:
                shutil.rmtree(dst)  # remove real dir if exists
            except FileNotFoundError:
                pass

            if just_copy == True:
                shutil.copytree(src, dst)
            else:
                os.symlink(src, dst, target_is_directory=True)

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
        "Hexa.NET.ImGui.dll",
        "Hexa.NET.ImGui.xml",
        "HexaGen.Runtime.dll",
        "HexaGen.Runtime.xml",
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