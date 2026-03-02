using System;
using System.Collections;
using System.Collections.Generic;
using System.Dynamic;
using System.Reflection;
using Hexa.NET.ImGui;
using REFrameworkNET;
using REFrameworkNET.Callbacks;
using REFrameworkNET.Attributes;

public class ObjectExplorerPlugin {
    [Callback(typeof(ImGuiRender), CallbackType.Pre)]
    public static void RenderImGui() {
        if (ImGui.Begin("Test Window")) {
            ImGui.SetNextItemOpen(true, ImGuiCond.Once);
            if (ImGui.TreeNode("Object Explorer")) {
                ObjectExplorer.Render();

                ImGui.TreePop();
            }

            ImGui.End();
        }
    }

    [REFrameworkNET.Attributes.PluginExitPoint]
    public static void OnUnload() {
    }

    [REFrameworkNET.Attributes.PluginEntryPoint]
    public static void Main() {
        //REFrameworkNET.Callbacks.ImGuiRender.Pre += RenderImGui;
    }
}

class ObjectExplorer {
    static System.Numerics.Vector4 TYPE_COLOR = new System.Numerics.Vector4(78.0f / 255.0f, 201 / 255.0f, 176 / 255.0f, 1.0f);
    static System.Numerics.Vector4 FIELD_COLOR = new(156 / 255.0f, 220 / 255.0f, 254 / 255.0f, 1.0f);
    static System.Numerics.Vector4 METHOD_COLOR = new(220 / 255.0f, 220 / 255.0f, 170 / 255.0f, 1.0f);

    static _System.Enum SystemEnumT = REFrameworkNET.TDB.Get().GetTypeT<_System.Enum>();

    // Resolve boxEnum vs InternalBoxEnum at startup (varies by game)
    static REFrameworkNET.Method s_boxEnumMethod = ResolveBoxEnumMethod();
    static bool s_boxEnumUsesRuntimeType;

    static REFrameworkNET.Method ResolveBoxEnumMethod() {
        var enumTDef = REFrameworkNET.TDB.Get().GetType("System.Enum");
        var m = enumTDef.FindMethod("InternalBoxEnum");
        if (m != null) { s_boxEnumUsesRuntimeType = true; return m; }
        m = enumTDef.FindMethod("boxEnum");
        if (m != null) { s_boxEnumUsesRuntimeType = false; return m; }
        return null;
    }

    static IObject BoxEnum(REFrameworkNET.TypeDefinition enumType, long value) {
        if (s_boxEnumMethod == null) return null;
        var rtType = enumType.GetRuntimeType();
        if (rtType == null) return null;
        // InternalBoxEnum takes RuntimeType, boxEnum takes Type
        var typeArg = s_boxEnumUsesRuntimeType ? rtType.As<_System.RuntimeType>() as IObject : rtType.As<_System.Type>() as IObject;
        object result = null;
        s_boxEnumMethod.HandleInvokeMember_Internal(null, new object[] { typeArg, value }, ref result);
        return result as IObject;
    }

    public static void DisplayColorPicker() {
        ImGui.ColorEdit4("Type Color", ref TYPE_COLOR);
        ImGui.ColorEdit4("Field Color", ref FIELD_COLOR);
        ImGui.ColorEdit4("Method Color", ref METHOD_COLOR);
    }

    public static void DisplayField(REFrameworkNET.IObject obj, REFrameworkNET.Field field) {
        var t = field.GetType();
        string tName = t != null ? t.GetFullName() : "null";

        var unified = obj != null ? obj as REFrameworkNET.UnifiedObject : null;
        ulong address = unified != null ? unified.GetAddress() : 0;

        if (field.IsStatic()) {
            address = field.GetDataRaw(obj.GetAddress(), false);
        } else if (obj != null) {
            address += field.GetOffsetFromBase();
        }

        // Make a tree node that spans the entire width of the window
        ImGui.PushID(address.ToString("X"));
        ImGui.SetNextItemOpen(false, ImGuiCond.Once);
        var made = ImGui.TreeNodeEx("", ImGuiTreeNodeFlags.SpanFullWidth);

        // Context menu to copy address to clipboard
        if (ImGui.BeginPopupContextItem()) {
            if (ImGui.MenuItem("Copy Address to Clipboard")) {
                ImGui.SetClipboardText(address.ToString("X"));
            }

            ImGui.EndPopup();
        }
        
        ImGui.SameLine();
        //ImGui.Text(" " + tName);
        ImGui.TextColored(TYPE_COLOR, " " + tName);

        ImGui.SameLine();

        ImGui.TextColored(FIELD_COLOR, field.GetName());

        ImGui.SameLine();

        if (field.IsStatic()) {
            // Red text
            ImGui.TextColored(new System.Numerics.Vector4(0.75f, 0.2f, 0.0f, 1.0f), "Static");
        } else {
            ImGui.Text("0x" + field.GetOffsetFromBase().ToString("X"));
        }

        if (obj == null) {
            if (made) {
                ImGui.Text("Value: null");
                ImGui.TreePop();
            }

            ImGui.PopID();
            return;
        }

        if (made) {
            if (!t.IsValueType()) {
                var objValue = obj.GetField(field.GetName()) as REFrameworkNET.IObject;

                if (objValue != null) {
                    DisplayObject(objValue);
                } else {
                    ImGui.Text("Value: null");
                }
            } else {
                object fieldData = null;
                var finalName = t.IsEnum() ? t.GetUnderlyingType().GetFullName() : tName;

                switch (finalName) {
                    case "System.Byte":
                        fieldData = field.GetDataT<byte>(obj.GetAddress(), false);
                        break;
                    case "System.SByte":
                        fieldData = field.GetDataT<sbyte>(obj.GetAddress(), false);
                        break;
                    case "System.Int16":
                        fieldData = field.GetDataT<short>(obj.GetAddress(), false);
                        break;
                    case "System.UInt16":
                        fieldData = field.GetDataT<ushort>(obj.GetAddress(), false);
                        break;
                    case "System.Int32":
                        fieldData = field.GetDataT<int>(obj.GetAddress(), false);
                        break;
                    case "System.UInt32":
                        fieldData = field.GetDataT<uint>(obj.GetAddress(), false);
                        break;
                    case "System.Int64":
                        fieldData = field.GetDataT<long>(obj.GetAddress(), false);
                        break;
                    case "System.UInt64":
                        fieldData = field.GetDataT<ulong>(obj.GetAddress(), false);
                        break;
                    case "System.Single":
                        fieldData = field.GetDataT<float>(obj.GetAddress(), false);
                        break;
                    case "System.Boolean":
                        fieldData = field.GetDataT<bool>(obj.GetAddress(), false);
                        break;
                    /*case "System.String":
                        ImGui.Text("Value: " + field.GetDataT<string>(obj.GetAddress(), false));
                        break;*/
                    default:
                        break;
                }

                if (t.IsEnum() && fieldData != null) {
                    long longValue = Convert.ToInt64(fieldData);
                    var boxedEnum = BoxEnum(t, longValue);
                    ImGui.Text("Result: " + (boxedEnum != null ? boxedEnum.Call("ToString()") : "?") + " (" + fieldData.ToString() + ")");
                } else if (fieldData != null) {
                    ImGui.Text("Value: " + fieldData.ToString());
                    //ImGui.Text("Value (" + t.FullName + ")" + field.GetDataRaw(obj.GetAddress(), false).ToString("X"));
                } else {
                    ImGui.Text("Value: null");
                }
            }

            ImGui.TreePop();
        }

        ImGui.PopID();
    }

    public static void DisplayMethod(REFrameworkNET.IObject obj, REFrameworkNET.Method method) {
        ImGui.PushID(method.GetDeclaringType().FullName + method.GetMethodSignature());
        var made = ImGui.TreeNodeEx("", ImGuiTreeNodeFlags.SpanFullWidth);
        ImGui.SameLine(0.0f, 0.0f);

        var returnT = method.GetReturnType();
        var returnTName = returnT != null ? returnT.GetFullName() : "null";

        //ImGui.Text(" " + returnTName);
        ImGui.TextColored(TYPE_COLOR, " " + returnTName);
        ImGui.SameLine(0.0f, 0.0f);

        ImGui.TextColored(METHOD_COLOR, " " + method.GetName());


        ImGui.SameLine(0.0f, 0.0f);
        ImGui.Text("(");
        
        var ps = method.GetParameters();

        if (ps.Count > 0) {
            for (int i = 0; i < ps.Count; i++) {
                var p = ps[i];

                ImGui.SameLine(0.0f, 0.0f);
                ImGui.TextColored(TYPE_COLOR, p.Type.GetFullName());

                if (p.Name != null && p.Name.Length > 0) {
                    ImGui.SameLine(0.0f, 0.0f);
                    ImGui.Text(" " + p.Name);
                }

                if (i < ps.Count - 1) {
                    ImGui.SameLine(0.0f, 0.0f);
                    ImGui.Text(", ");
                }
                //postfix += p.Type.GetFullName() + " " + p.Name + ", ";
            }

            ImGui.SameLine(0.0f, 0.0f);
            ImGui.Text(")");

            //postfix = postfix.Substring(0, postfix.Length - 3);
        } else {
            ImGui.SameLine(0.0f, 0.0f);
            ImGui.Text(")");
        }

        if (made) {
            if ((method.Name.StartsWith("get_") || method.Name.StartsWith("Get") || method.Name == "ToString") && ps.Count == 0) {
                object result = null;
                obj.HandleInvokeMember_Internal(method, null, ref result);

                if (result != null) {
                    if (result is IObject objResult) {
                        DisplayObject(objResult);
                    } else {
                        var returnType = method.GetReturnType();

                        if (returnType.IsEnum()) {
                            long longValue = Convert.ToInt64(result);
                            var boxedEnum = BoxEnum(returnType, longValue);
                            ImGui.Text("Result: " + (boxedEnum != null ? boxedEnum.Call("ToString()") : "?") + " (" + result.ToString() + ")");
                        } else {
                            ImGui.Text("Result: " + result.ToString() + " (" + result.GetType().FullName + ")");
                        }
                    }
                } else {
                    ImGui.Text("Result: null");
                }
            }

            ImGui.TreePop();
        }

        ImGui.PopID();
    }

    public static void DisplayType(REFrameworkNET.TypeDefinition t) {
        ImGui.Text("Name: " + t.GetFullName());
        ImGui.Text("Namespace: " + t.GetNamespace());

        if (t.DeclaringType != null) {
            var made = ImGui.TreeNodeEx("Declaring Type: ", ImGuiTreeNodeFlags.SpanFullWidth);
            ImGui.SameLine(0.0f, 0.0f);
            ImGui.TextColored(TYPE_COLOR, t.DeclaringType.GetFullName());
            if (made) {
                DisplayType(t.DeclaringType);
                ImGui.TreePop();
            }
        }

        if (t.ParentType != null) {
            var made = ImGui.TreeNodeEx("Parent Type: ", ImGuiTreeNodeFlags.SpanFullWidth);
            ImGui.SameLine(0.0f, 0.0f);
            ImGui.TextColored(TYPE_COLOR, t.ParentType.GetFullName());
            if (made) {
                DisplayType(t.ParentType);
                ImGui.TreePop();
            }
        }
        
        var runtimeTypeRaw = t.GetRuntimeType();

        if (runtimeTypeRaw != null) {
            var runtimeType = runtimeTypeRaw.As<_System.Type>();
            var assembly = runtimeType.Assembly;

            if (assembly != null) {
                if (ImGui.TreeNode("Assembly: " + assembly.FullName.Split(',')[0])) {
                    DisplayObject(assembly as IObject);
                    ImGui.TreePop();
                }
            }

            var baseType = runtimeType.BaseType;

            /*if (baseType != null) {
                if (ImGui.TreeNode("Base Type (" + (baseType.get_TypeHandle() as REFrameworkNET.TypeDefinition).FullName + ")")) {
                    DisplayObject(baseType as IObject);
                    ImGui.TreePop();
                }
            }*/

            if (ImGui.TreeNode("Runtime Type")) {
                DisplayObject(runtimeType as IObject);
                ImGui.TreePop();
            }
        }
    }

    private static TypeDefinition SystemArrayT = REFrameworkNET.TDB.Get().GetType("System.Array");

    public static void DisplayObject(REFrameworkNET.IObject obj) {
        bool isPinnedObject = obj is REFrameworkNET.ManagedObject ? (obj as REFrameworkNET.ManagedObject).GetReferenceCount() >= 0 : false;

        if (isPinnedObject) {
            ImGui.PushID(obj.GetAddress().ToString("X"));
        }

        if (ImGui.TreeNode("Internals")) {
            if (ImGui.TreeNode("Type Info")) {
                DisplayType(obj.GetTypeDefinition());
                ImGui.TreePop();
            }

            ImGui.Text("Address: 0x" + obj.GetAddress().ToString("X"));

            if (obj is REFrameworkNET.ManagedObject) {
                var managed = obj as REFrameworkNET.ManagedObject;
                ImGui.Text("Reference count: " + managed.GetReferenceCount().ToString());
            }

            ImGui.TreePop();
        }

        if (ImGui.TreeNode("Methods")) {
            var tdef = obj.GetTypeDefinition();
            List<Method> methods = new List<Method>();

            for (var parent = tdef; parent != null; parent = parent.ParentType) {
                var parentMethods = parent.GetMethods();
                methods.AddRange(parentMethods);
            }

            // Sort methods by name
            methods.Sort((a, b) => a.GetName().CompareTo(b.GetName()));

            // Remove methods that have "!" in their parameters
            methods.RemoveAll((m) => m.GetParameters().Exists((p) => p.Type.Name.Contains("!")));

            foreach (var method in methods) {
                DisplayMethod(obj, method);
            }
        }

        if (ImGui.TreeNode("Fields")) {
            var tdef = obj.GetTypeDefinition();
            List<Field> fields = new List<Field>();

            for (var parent = tdef; parent != null; parent = parent.ParentType) {
                var parentFields = parent.GetFields();
                fields.AddRange(parentFields);
            }

            // Sort fields by name
            fields.Sort((a, b) => a.GetName().CompareTo(b.GetName()));

            foreach (var field in fields) {
                DisplayField(obj, field);
            }

            ImGui.TreePop();
        }

        if (obj.GetTypeDefinition().IsDerivedFrom(SystemArrayT)) {
            ImGui.Text("Array Length: " + (int)obj.Call("get_Length"));
            
            var easyArray = obj.As<_System.Array>();
            var elementType = obj.GetTypeDefinition().GetElementType();
            var elementSize = elementType.GetSize();

            for (int i = 0; i < easyArray.Length; i++) {
                var element = easyArray.GetValue(i);
                if (element == null) {
                    ImGui.Text("Element " + i + ": null");
                    continue;
                }
                
                var made = ImGui.TreeNodeEx("Element " + i, ImGuiTreeNodeFlags.SpanFullWidth);

                ImGui.SameLine(0.0f, 0.0f);
                
                if (element is IObject) {
                    var asString = (element as IObject).Call("ToString()") as string;
                    ImGui.TextColored(TYPE_COLOR, " ("+ asString + ")");
                } else {
                    ImGui.TextColored(TYPE_COLOR, " ("+ element.ToString() + ")");
                }

                if (made) {
                    if (element is IObject objElement) {
                        //ImGui.PushID((obj.GetAddress() + 0x10 + (ulong)(i * elementSize)).ToString("X"));
                        ImGui.PushID(i);
                        DisplayObject(objElement);
                        ImGui.PopID();
                    } else {
                        ImGui.Text("Element: " + element.ToString());
                    }

                    ImGui.TreePop();
                }
            }
        }

        if (isPinnedObject) {
            ImGui.PopID();
        }
    }

    public static void RenderNativeSingletons() {
        var singletons = REFrameworkNET.API.GetNativeSingletons();

        // Sort by type name
        singletons.Sort((a, b) => a.Instance.GetTypeDefinition().GetFullName().CompareTo(b.Instance.GetTypeDefinition().GetFullName()));

        foreach (var singletonDesc in singletons) {
            var singleton = singletonDesc.Instance;
            if (singleton == null) {
                continue;
            }
            var singletonName = singleton.GetTypeDefinition().GetFullName();

            if (ImGui.TreeNode(singletonName)) {
                DisplayObject(singleton);
                ImGui.TreePop();
            }
        }
    }

    public static void RenderManagedSingletons() {
        var singletons = REFrameworkNET.API.GetManagedSingletons();

        // Sort singletons by type name
        // Remove any singletons that are null first
        singletons.RemoveAll((s) => s.Instance == null);
        singletons.Sort((a, b) => a.Instance.GetTypeDefinition().GetFullName().CompareTo(b.Instance.GetTypeDefinition().GetFullName()));

        foreach (var singletonDesc in singletons) {
            var singleton = singletonDesc.Instance;
            if (singleton == null) {
                continue;
            }
            var singletonName = singleton.GetTypeDefinition().GetFullName();

            if (ImGui.TreeNode(singletonName)) {
                DisplayObject(singleton);
                ImGui.TreePop();
            }
        }
    }

    public static void Render() {
        ImGui.SetNextItemOpen(true, ImGuiCond.Once);
        if (ImGui.TreeNode("Color Picker")) {
            DisplayColorPicker();
            ImGui.TreePop();
        }


        try {
            if (ImGui.TreeNode("Managed Singletons")) {
                RenderManagedSingletons();
                ImGui.TreePop();
            }

            if (ImGui.TreeNode("Native Singletons")) {
                RenderNativeSingletons();
                ImGui.TreePop();
            }

            var appdomain = _System.AppDomain.CurrentDomain;

            if (ImGui.TreeNode("AppDomain")) {
                // GetAssemblies doesn't exist in all games' proxies, use reflection
                try {
                    var assembliesObj = (appdomain as IObject).Call("GetAssemblies()");
                    if (assembliesObj is IObject arrObj && ImGui.TreeNode("Assemblies")) {
                        int len = (int)arrObj.Call("get_Length");
                        for (int i = 0; i < len; i++) {
                            var element = arrObj.As<_System.Array>().GetValue(i);
                            if (element is IObject elemObj) {
                                var location = elemObj.Call("get_Location") as string ?? "null";
                                if (ImGui.TreeNode(location)) {
                                    DisplayObject(elemObj);
                                    ImGui.TreePop();
                                }
                            }
                        }
                        ImGui.TreePop();
                    }
                } catch { }

                DisplayObject(appdomain as IObject);
                ImGui.TreePop();
            }
        } catch (Exception e) {
            System.Console.WriteLine(e.ToString());
        }
    }
} // class ObjectExplorer