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

def als(align, size):
    return { "align": align, "size": size }

hardcoded_align_sizes = {
    "Bool": als(1, 1),
    "C8": als(1, 1),
    "S8": als(1, 1),
    "U8": als(1, 1),

    "C16": als(2, 2),
    "S16": als(2, 2),
    "U16": als(2, 2),
    
    "S32": als(4, 4),
    "U32": als(4, 4),
    "F32": als(4, 4),

    "S64": als(8, 8),
    "U64": als(8, 8),
    "F64": als(8, 8),

    "Object": als(4, 4),
    "UserData": als(4, 4),
    "Resource": als(4, 4),
    "String": als(4, 4),
    "RuntimeType": als(4, 4),
}

def generate_native_name(element):
    if element is None:
        os.system("Error")

    if element["string"] == True:
        return "String%iA%i" % (element["size"], element["align"])
    elif element["list"] == True:
        return "List%iA%i%s" % (element["size"], element["align"], generate_native_name(element["element"]))
    
    return "Data%iA%i" % (element["size"], element["align"])

def generate_field_entries(il2cpp_dump, natives, key, il2cpp_entry, use_typedefs, prefix = "", i=0, struct_i=0):
    e = il2cpp_entry
    parent_name = key

    fields_out = []
    struct_str = ""

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
        found_anything = False

        for chain in parent_native:
            if "layout" not in chain or len(chain["layout"]) == 0:
                continue

            found_anything = True
            struct_str = struct_str + "// " + chain["name"] + " BEGIN\n"
            
            layout = chain["layout"]
            for field in layout:
                native_type_name = generate_native_name(field)
                native_field_name = "v" + str(i)

                new_entry = {
                    "type": native_type_name,
                    "name": native_field_name,
                    "original_type": "",
                    "align": field["align"],
                    "size": field["size"],
                }

                if "element" in field and "list" in field and field["list"] == True:
                    new_entry["element"] = {
                        "type": generate_native_name(field["element"]),
                        "original_type": "",
                        "align": field["element"]["align"],
                        "size": field["element"]["size"],
                    }
                    new_entry["array"] = True
                else:
                    new_entry["array"] = False

                fields_out.append(new_entry)

                struct_str = struct_str + "    " + native_type_name + " " + native_field_name + ";\n"
                i = i + 1

            struct_str = struct_str + "// " + chain["name"] + " END\n"
        
        if found_anything:
            break

    if "RSZ" in il2cpp_entry:
        for rsz_entry in il2cpp_entry["RSZ"]:
            name = "v" + str(i)

            if "potential_name" in rsz_entry:
                name = rsz_entry["potential_name"]

            code = rsz_entry["code"]
            type = rsz_entry["type"]

            if code == "Struct" and type in il2cpp_dump:
                nested_entry, nested_str, i, struct_i = generate_field_entries(il2cpp_dump, natives, type, il2cpp_dump[type], use_typedefs, "STRUCT_" + name + "_", i, struct_i)

                if len(nested_entry) > 0:
                    fields_out += nested_entry
                    struct_str = struct_str + nested_str
                    struct_i = struct_i + 1
            else:
                if code in hardcoded_align_sizes:
                    align_size = hardcoded_align_sizes[code]
                else:
                    align_size = als(rsz_entry["align"], int(rsz_entry["size"], 16))
                
                if use_typedefs == True:
                    if code in code_typedefs:
                        code = code_typedefs[code]
                    else:
                        code = "RSZ" + code

                if rsz_entry["array"] == True:
                    code = code + "List"

                fields_out.append({
                    "type": code,
                    "name": prefix + name,
                    "original_type": type,
                    "array": rsz_entry["array"] == 1,
                    "align": align_size["align"],
                    "size": align_size["size"],
                })

                field_str = "    " + code + " " + name + "; //\"" + type + "\""
                struct_str = struct_str + field_str + "\n"
                
                i = i + 1
    
    return fields_out, struct_str, i, struct_i


def main(out_postfix="", il2cpp_path="il2cpp_dump.json", natives_path=None, use_typedefs=False, use_hashkeys=False):
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
        if entry is None:
            continue

        if use_hashkeys:
            out_json[entry["fqn"]] = {}
            json_entry = out_json[entry["fqn"]]
            json_entry["name"] = key
        else:
            out_json[key] = {}
            json_entry = out_json[key]
            json_entry["fqn"] = entry["fqn"]
        
        json_entry["crc"] = entry["crc"]

        struct_str = "// " + entry["fqn"] + "\n"
        struct_str = struct_str + "struct " + key + " {\n"

        fields, struct_body, _, __ = generate_field_entries(il2cpp_dump, natives, key, entry, use_typedefs)

        json_entry["fields"] = fields
        struct_str = struct_str + struct_body

        struct_str = struct_str + "};\n"
        out_str = out_str + struct_str

    with open("rsz" + out_postfix + ".txt", "w", encoding="utf8") as f:
        f.write(out_str)

    with open("rsz" + out_postfix + ".json", "w", encoding="utf8") as f:
        json.dump(out_json, f, indent=4, sort_keys=True)


if __name__ == '__main__':
    fire.Fire(main)