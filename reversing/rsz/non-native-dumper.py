import json
import fire
import os

code_typedefs = {
    "F32": "RSZFloat",
    "F64": "RSZDouble",
    "U8": "ubyte",
    "S8": "byte",
    "S64": "RSZInt64",
    "S32": "RSZInt",
    "S16": "RSZShort",
    "U64": "RSZUInt64",
    "U32": "RSZUInt",
    "U16": "RSZUShort"
}

def main(il2cpp_path="il2cpp_dump.json", natives_path=None, use_typedefs=False):
    with open(il2cpp_path, "r", encoding="utf8") as f:
        il2cpp_dump = json.load(f)

    natives = None

    if natives_path is not None:
        with open(natives_path, "r", encoding="utf8") as f:
            natives = json.load(f)
    else:
        print("No natives file found, output may be incorrect for some types")

    out_str = ""
    out_json = {}
    
    for key, entry in il2cpp_dump.items():
        if entry is None or "RSZ" not in entry:
            continue

        out_json[key] = {}
        json_entry = out_json[key]
        json_entry["fqn"] = entry["fqn"]
        json_entry["crc"] = entry["crc"]
        json_entry["fields"] = []

        struct_str = "// " + entry["fqn"] + "\n"
        struct_str = struct_str + "struct " + key + " {\n"

        i = 0

        e = entry
        parent_name = key

        # Go through parents until we run into a native that we need to insert at the top of the structure
        for f in range(0, 10):
            if natives is None or "parent" not in e:
                break

            if not (parent_name in il2cpp_dump and "RSZ" not in il2cpp_dump[parent_name] and parent_name in natives):
                # Keep going up the heirarchy of parents until we reach something usable
                if "parent" in e and e["parent"] in il2cpp_dump:
                    parent_name = e["parent"]
                    e = il2cpp_dump[e["parent"]]

                continue

            parent_native = natives[parent_name]
            parent_il2cpp = il2cpp_dump[parent_name]

            found_anything = False

            for chain in parent_native:
                if "layout" not in chain or len(chain["layout"]) == 0:
                    continue

                found_anything = True
                struct_str = struct_str + "// " + chain["name"] + " BEGIN\n"
                
                layout = chain["layout"]
                for field in layout:
                    def generate_name(element):
                        if element is None:
                            os.system("Error")

                        if element["string"] == True:
                            return "String%iA%i" % (element["size"], element["align"])
                        elif element["list"] == True:
                            return "List%iA%i%s" % (element["size"], element["align"], generate_name(element["element"]))
                        
                        return "Data%iA%i" % (element["size"], element["align"])
                    
                    native_type_name = generate_name(field)
                    native_field_name = "v" + str(i)

                    json_entry["fields"].append({
                        "type": native_type_name,
                        "name": native_field_name,
                        "original_type": ""
                    })

                    struct_str = struct_str + "    " + native_type_name + " " + native_field_name + ";\n"
                    i = i + 1

                struct_str = struct_str + "// " + chain["name"] + " END\n"
            
            if found_anything:
                break

        for rsz_entry in entry["RSZ"]:
            name = "v" + str(i)

            if "potential_name" in rsz_entry:
                name = rsz_entry["potential_name"]

            code = rsz_entry["code"]
            type = rsz_entry["type"]
            
            if use_typedefs == True:
                if code in code_typedefs:
                    code = code_typedefs[code]
                else:
                    code = "RSZ" + code

            if rsz_entry["array"] == True:
                code = code + "List"

            json_entry["fields"].append({
                "type": code,
                "name": name,
                "original_type": type
            })

            field_str = "    " + code + " " + name + "; //\"" + type + "\""
            struct_str = struct_str + field_str + "\n"
            
            i = i + 1

        struct_str = struct_str + "};\n"
        out_str = out_str + struct_str

    with open("RSZ_dump_python.txt", "w", encoding="utf8") as f:
        f.write(out_str)

    with open("RSZ_dump_python.json", "w", encoding="utf8") as f:
        json.dump(out_json, f, indent=4, sort_keys=True)


if __name__ == '__main__':
    fire.Fire(main)