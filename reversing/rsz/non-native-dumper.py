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
    "F16": als(2, 2),
    
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

    "Quaternion": als(16, 16),
    "Guid": als(8, 16),
    "GameObjectRef": als(8, 16),
    "Color": als(4, 4),
    "DateTime": als(8, 8),
    # Enum could have variable size and alignment, so we fallback to its base type and never use it.

    "Uint2": als(4, 8),
    "Uint3": als(4, 12),
    "Uint4": als(4, 16),
    "Int2": als(4, 8),
    "Int3": als(4, 12),
    "Int4": als(4, 16),
    "Float2": als(4, 8),
    "Float3": als(4, 12),
    "Float4": als(4, 16),
    "Mat4": als(16, 64),
    "Vec2": als(16, 16),
    "Vec3": als(16, 16),
    "Vec4": als(16, 16),

    "AABB": als(16, 32),
    "Capsule": als(16, 48),
    "Cone": als(16, 32),
    "LineSegment": als(16, 32),
    "OBB": als(16, 80),
    "Plane": als(16, 16),
    "Point": als(4, 8),
    "Range": als(4, 8),
    "RangeI": als(4, 8),
    "Size": als(4, 8),
    "Sphere": als(16, 16),
    "Triangle": als(16, 48),
    "Cylinder": als(16, 48),
    "Area": als(16, 48),
    "Rect": als(4, 16),
    "Frustum": als(16, 96),
    "KeyFrame": als(16, 16),

    "Sfix": als(4, 4),
    "Sfix2": als(4, 8),
    "Sfix3": als(4, 12),
    "Sfix4": als(4, 16),
}

hardcoded_native_type_to_TypeCode = {
    'float': 'F32',
    'int': 'S32',
    'size_t': 'U32',

    'via.GameObject': 'Object',

    'System.Boolean': 'Bool',
    'System.Char': 'C8',
    'System.SByte': 'S8',
    'System.Byte': 'U8',
    'System.Int16': 'S16',
    'System.UInt16': 'U16',
    'System.Int32': 'S32',
    'System.UInt32': 'U32',
    'System.Int64': 'S64',
    'System.UInt64': 'U64',
    'System.Single': 'F32',
    'System.Double': 'F64',
}

TypeCode = [
    # "Undefined",
    "Object",
    "Action",
    "Struct",
    "NativeObject",
    "Resource",
    "UserData",
    "Bool",
    "C8",
    "C16",
    "S8",
    "U8",
    "S16",
    "U16",
    "S32",
    "U32",
    "S64",
    "U64",
    "F32",
    "F64",
    "String",
    "MBString",
    "Enum",
    "Uint2",
    "Uint3",
    "Uint4",
    "Int2",
    "Int3",
    "Int4",
    "Float2",
    "Float3",
    "Float4",
    "Float3x3",
    "Float3x4",
    "Float4x3",
    "Float4x4",
    "Half2",
    "Half4",
    "Mat3",
    "Mat4",
    "Vec2",
    "Vec3",
    "Vec4",
    "VecU4",
    "Quaternion",
    "Guid",
    "Color",
    "DateTime",
    "AABB",
    "Capsule",
    "TaperedCapsule",
    "Cone",
    "Line",
    "LineSegment",
    "OBB",
    "Plane",
    "PlaneXZ",
    "Point",
    "Range",
    "RangeI",
    "Ray",
    "RayY",
    "Segment",
    "Size",
    "Sphere",
    "Triangle",
    "Cylinder",
    "Ellipsoid",
    "Area",
    "Torus",
    "Rect",
    "Rect3D",
    "Frustum",
    "KeyFrame",
    "Uri",
    "GameObjectRef",
    "RuntimeType",
    "Sfix",
    "Sfix2",
    "Sfix3",
    "Sfix4",
    "Position",
    "F16",
    "Decimal",
    # "End",
]

TypeCodeSearch = dict([(k.lower(),v) for k,v in hardcoded_native_type_to_TypeCode.items()] + [(a.lower(), a) for a in TypeCode] + [("via."+a.lower(), a) for a in TypeCode] + [("system."+a.lower(), a) for a in TypeCode])
# print(TypeCodeSearch)

def generate_native_name(element, use_potential_name, reflection_property, il2cpp_dump={}):
    if element is None:
        os.system("Error")

    if element["string"] == True:
        if use_potential_name:
            # try to find if it is Resource type, return either "String" or "Resource"
            property_type = reflection_property["type"]
            if (property_type.endswith("ResourceHandle") or property_type.endswith("ResorceHandle")) and property_type.startswith("via."):
                property_type = property_type.replace("ResourceHandle", "ResourceHolder")
                property_type = property_type.replace("ResorceHandle", "ResorceHolder") # arrrrrrrrrr
                native_element = il2cpp_dump.get(property_type, None)
                chain = native_element.get('deserializer_chain', None)
                if "via.ResourceHolder" in [a['name'] for a in chain]: # ResourcePath
                    return "Resource"
            elif property_type == "via.resource_handle":
                return "Resource"
        return "String"
    elif element["list"] == True:
        return generate_native_name(element["element"], use_potential_name, reflection_property, il2cpp_dump)
    elif use_potential_name and reflection_property is not None:
        property_type = reflection_property["type"]
        type_code = TypeCodeSearch.get(property_type.lower(), "Data")

        if type_code == "Data" and property_type.startswith("via."):
            native_element = il2cpp_dump.get(property_type, None)

            if native_element is None:
                return type_code

            parent = native_element.get('parent', None)
            if parent is not None:
                parent_type_code = TypeCodeSearch.get(parent.lower(), "Data")
                if parent_type_code != 'Data':
                    return parent_type_code
                
            chain = native_element.get('deserializer_chain', None)
            if chain is not None:
                for chain_i in reversed(chain):
                    chain_type_code = TypeCodeSearch.get(chain_i['name'].lower(), "Data")
                    if chain_type_code != 'Data':
                        return chain_type_code
        
        return type_code
    return "Data"

def enum_fallback(reflection_property, il2cpp_dump={}):
    native_element = il2cpp_dump.get(reflection_property["type"], None)
    # Enum type should have and only have one "RSZ" field. These check are just for safety. 
    if native_element is None:
        return "Enum"
    if "RSZ" not in native_element:
        return "Enum"
    if len(native_element["RSZ"]) != 1:
        return "Enum"
    return native_element["RSZ"][0]["code"]

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

            # Get reflection_properties for guessing native type name.
            reflection_properties = il2cpp_dump[chain["name"]].get("reflection_properties", None)
            append_potential_name = False

            # If len not match, we give-up
            if len(layout) == len(reflection_properties):
                append_potential_name = True

                # sort reflection_properties by its native order
                order_rp = [(int(v["order"]), (k,v)) for k, v in reflection_properties.items()]
                reflection_properties = dict([v for _, v in sorted(order_rp)])

                # check align and size to increase accuracy
                for (property_name, property_value), field in zip(reflection_properties.items(), layout):
                    property_type_code = generate_native_name(field, True, property_value, il2cpp_dump)

                    if property_type_code == "Enum":
                        property_type_code = enum_fallback(property_value, il2cpp_dump)

                    reflection_properties[property_name]["TypeCode"] = property_type_code

                    if property_type_code == "Data":
                        continue
                    if "element" in field and "list" in field and field["list"] == True:
                        field_align = field["element"]["align"]
                        field_size = field["element"]["size"]
                    else:
                        field_align = field["align"]
                        field_size = field["size"]

                    property_align = hardcoded_align_sizes[property_type_code]["align"]
                    property_size = hardcoded_align_sizes[property_type_code]["size"]

                    if property_align != field_align or property_size != field_size:
                        append_potential_name = False

            if append_potential_name:
                rp_names = list(reflection_properties.keys())
                rp_values = list(reflection_properties.values())

            for rp_idx, field in enumerate(layout):
                native_type_name = generate_native_name(field, False, None)
                native_field_name = "v" + str(i)
                native_org_type_name = ""
                if append_potential_name:
                    if rp_values[rp_idx]["TypeCode"] != "Data":
                        native_type_name = rp_values[rp_idx]["TypeCode"]

                    native_field_name += "_" + rp_names[rp_idx]
                    # native_field_name = rp_names[rp_idx] # without start with v_
                    native_org_type_name = rp_values[rp_idx]['type']
                    if native_type_name != "Data" and not native_org_type_name.startswith("via"):
                        native_org_type_name = "" # those would be sth like "bool" "s32"

                new_entry = {
                    "type": native_type_name,
                    "name": native_field_name,
                    "original_type": native_org_type_name,
                    "align": field["align"],
                    "size": field["size"],
                    "native": True
                }

                if "element" in field and "list" in field and field["list"] == True:
                    '''
                    new_entry["element"] = {
                        "type": generate_native_name(field["element"]),
                        "original_type": "",
                        "align": field["element"]["align"],
                        "size": field["element"]["size"],
                    }
                    '''
                    new_entry["align"] = field["element"]["align"]
                    new_entry["size"] = field["element"]["size"]
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

                '''
                if rsz_entry["array"] == True:
                    code = code + "List"
                '''

                fields_out.append({
                    "type": code,
                    "name": prefix + name,
                    "original_type": type,
                    "array": rsz_entry["array"] == 1,
                    "align": align_size["align"],
                    "size": align_size["size"],
                    "native": False
                })

                field_str = "    " + code + " " + name + "; //\"" + type + "\""
                struct_str = struct_str + field_str + "\n"
                
                i = i + 1
    
    return fields_out, struct_str, i, struct_i


def main(out_postfix="", il2cpp_path="", natives_path=None, use_typedefs=False, use_hashkeys=False):
    if il2cpp_path is None:
        return

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
        json.dump(out_json, f, indent='\t', sort_keys=True)


if __name__ == '__main__':
    fire.Fire(main)