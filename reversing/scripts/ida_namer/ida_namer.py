# Script to generate an IDA python script to name all of the
# Functions in the IL2CPP Dump.
# partial credits to deepdarkkapustka for the script this is based off of (@muhopensores on github)
import json
import fire
import os

def main(il2cpp_path=None, out_path=None, imagebase = None, new_imagebase = None):
    if il2cpp_path is None:
        print("--il2cpp_path not specified")
        return

    if out_path is None:
        print("--out_path not specified")
        return

    if not os.path.exists(il2cpp_path):
        print("--il2cpp_path does not exist")
        return

    with open(il2cpp_path, "r", encoding="utf8") as f:
        il2cpp_dump = json.load(f)

    out_str = ""
    bad_chars = ['<', '>', '`', ".", ",", "[", "]", "|", ' ', '=']

    num_methods_found = 0
    num_reflection_methods_found = 0

    seen_functions = set()

    for class_name, entry in il2cpp_dump.items():
        try:
            for bad_char in bad_chars:
                class_name = class_name.replace(bad_char, "_")

            if "methods" in entry:
                for method_name, method_entry in entry["methods"].items():
                    if method_entry is None:
                        continue

                    # filter the method name and class name for bad chars
                    for bad_char in bad_chars:
                        method_name = method_name.replace(bad_char, "_")

                    #print(hex(int("0x" + method_entry["function"], 16)))
                    address = str(hex(int("0x" + method_entry["function"], 16))) # is a string not an int

                    if address == "0" or address in seen_functions:
                        continue

                    seen_functions.add(address)

                    if imagebase is not None and new_imagebase is not None:
                        address_int = int("0x" + method_entry["function"], 16)
                        address_int = address_int - imagebase
                        address_int = address_int + new_imagebase
                        address = str(hex(address_int))

                    out_str = out_str + "idc.MakeName(%s, '%s__%s')\n" % (address, class_name, method_name)
                    num_methods_found = num_methods_found + 1

            if "reflection_methods" in entry:
                try:
                    for method_name, method_entry in entry["reflection_methods"].items():
                        if method_entry is None:
                            continue

                        for bad_char in bad_chars:
                            method_name = method_name.replace(bad_char, "_")

                        address = str(hex(int("0x" + method_entry["function"], 16))) # is a string not an int

                        if address == "0" or address in seen_functions:
                            continue
                        
                        seen_functions.add(address)

                        if imagebase is not None and new_imagebase is not None:
                            address_int = int("0x" + method_entry["function"], 16)
                            address_int = address_int - imagebase
                            address_int = address_int + new_imagebase
                            address = str(hex(address_int))

                        out_str = out_str + "idc.MakeName(%s, 'reflection_methods_%s')\n" % (address, method_name)
                        num_reflection_methods_found = num_reflection_methods_found + 1
                except:
                    continue
        except (KeyError, TypeError):
            print("Error processing class %s" % class_name)
            continue

    print("Found %d methods" % num_methods_found)
    print("Found %d reflection methods" % num_reflection_methods_found)
    print("Writing to %s..." % out_path)

    with open(out_path, "w", encoding="utf8") as f:
        f.write(out_str)

    print("Done!")


if __name__ == '__main__':
    fire.Fire(main)