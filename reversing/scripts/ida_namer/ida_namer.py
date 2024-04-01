# Script to generate an IDA python script to name all of the
# Functions in the IL2CPP Dump.
# partial credits to deepdarkkapustka for the script this is based off of (@muhopensores on github)
import json
import fire
import os

def main(il2cpp_path = None, out_path = None, imagebase = None, new_imagebase = None, no_reflection = None, ida77 = None, undefine = None):
    if not il2cpp_path:
        print("--il2cpp_path not specified")
        return
    elif not out_path:
        print("--out_path not specified")
        return
    elif imagebase and not new_imagebase:
        print("--imagebase specified but --new_imagebase not specified")
        return
    elif not imagebase and new_imagebase:
        print("--new_imagebase specified but --imagebase not specified")
        return
    elif not os.path.exists(il2cpp_path):
        print("--il2cpp_path does not exist")
        return

    if imagebase and isinstance(imagebase, str):
        imagebase = int(imagebase, 16)

    if new_imagebase and isinstance(new_imagebase, str):
        new_imagebase = int(new_imagebase, 16)

    with open(il2cpp_path, "r", encoding="utf8") as f:
        il2cpp_dump = json.load(f)

    out_lines = []
    seen_functions = set()

    def output_code_old(address, name):
        nonlocal out_lines, undefine

        if undefine:
            out_lines.append("MakeUnkn(%s, BADADDR);\n" % (address))
        out_lines.append("MakeNameEx(%s, \"%s\", SN_CHECK);\n" % (address, name))
        if undefine:
            out_lines.append("MakeFunction(%s, BADADDR);\n" % (address))

    def output_code_new(address, name):
        nonlocal out_lines, undefine
        if undefine:
            out_lines.append("del_items(%s);\n" % (address))
        out_lines.append("set_name(%s, \"%s\", SN_CHECK);\n" % (address, name))
        if undefine:
            out_lines.append("add_func(%s);\n" % (address))

    def output_code(address, name):
        nonlocal out_lines, imagebase, new_imagebase, undefine, ida77

        if address == 0 or address in seen_functions:
            return False

        seen_functions.add(address)

        if imagebase is not None and new_imagebase is not None:
            address = address - imagebase
            address = address + new_imagebase

        address = str(hex(address))

        if ida77:
            output_code_new(address, name)
        else:
            output_code_old(address, name)

        return True

    bad_chars = ['<', '>', '`', '.', ',', '[', ']', '|', ' ', '=']

    def filter_name(name):
        nonlocal bad_chars
        for bad_char in bad_chars:
            name = name.replace(bad_char, "_")
        return name

    out_lines.append("#include <idc.idc>\n")
    out_lines.append("static main()\n")
    out_lines.append("{\n")

    num_methods_found = 0
    num_reflection_methods_found = 0

    for class_name, entry in il2cpp_dump.items():
        try:
            for bad_char in bad_chars:
                class_name = class_name.replace(bad_char, "_")

            if "methods" in entry:
                for method_name, method_entry in entry["methods"].items():
                    if method_entry is None:
                        continue

                    address = int("0x" + method_entry["function"], 16) # is a string not an int

                    if output_code(address, filter_name(class_name) + "__" + filter_name(method_name)):
                        num_methods_found = num_methods_found + 1

            if not no_reflection and "reflection_methods" in entry:
                for method_name, method_entry in entry["reflection_methods"].items():
                    if method_entry is None:
                        continue

                    address = int(method_entry["function"], 16) # is a string not an int

                    if output_code(address, "reflection_methods_" + filter_name(method_name)):
                        num_reflection_methods_found = num_reflection_methods_found + 1

        except (KeyError, TypeError):
            print("Error processing class %s" % class_name)
            continue

    out_lines.append("}\n")

    print("Found %d methods" % num_methods_found)
    print("Found %d reflection methods" % num_reflection_methods_found)
    print("Writing to %s..." % out_path)

    with open(out_path, "w", encoding="utf8") as f:
        f.writelines(out_lines)

    print("Done!")


if __name__ == '__main__':
    fire.Fire(main)
