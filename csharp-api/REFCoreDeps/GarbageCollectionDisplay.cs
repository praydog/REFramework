using ImGuiNET;
using System;

namespace REFrameworkNET {
    public class GarbageCollectionDisplay {
        static bool doFullGC = false;
        public static void Render() {
            ImGui.Checkbox("Do Full GC", ref doFullGC);

            if (ImGui.Button("Do Gen 2 GC")) {
                GC.Collect(2, GCCollectionMode.Forced, false);
            }

            var heapBytes = GC.GetTotalMemory(false);
            var heapKb = heapBytes / 1024;
            var heapMb = heapKb / 1024;
            var heapGb = heapMb / 1024;

            if (doFullGC) {
                GC.Collect(0, GCCollectionMode.Forced, false);
                GC.Collect(1, GCCollectionMode.Forced, false);
                GC.Collect(2, GCCollectionMode.Forced, false);
            }

            ImGui.Text("GC Memory: " + GC.GetTotalMemory(false) / 1024 + " KB (" + heapMb + " MB, " + heapGb + " GB)");
            ImGui.Text("GC Collection Count (gen 0): " + GC.CollectionCount(0));
            ImGui.Text("GC Collection Count (gen 1): " + GC.CollectionCount(1));
            ImGui.Text("GC Collection Count (gen 2): " + GC.CollectionCount(2));
            ImGui.Text("GC Latency Mode: " + System.Runtime.GCSettings.LatencyMode.ToString());
            ImGui.Text("GC Is Server GC: " + System.Runtime.GCSettings.IsServerGC);
            ImGui.Text("GC Max Generation: " + GC.MaxGeneration);
            ImGui.Text("GC Force Full Collection: " + System.Runtime.GCSettings.LargeObjectHeapCompactionMode);
            
            // memory info
            var memoryInfo = GC.GetGCMemoryInfo();
            ImGui.Text("GC Is Concurrent: " + memoryInfo.Concurrent);
            ImGui.Text("GC Fragmentation: " + memoryInfo.FragmentedBytes);

            var gcHeapSize = GC.GetGCMemoryInfo().HeapSizeBytes;
            var gcHeapSizeKb = gcHeapSize / 1024;
            var gcHeapSizeMb = gcHeapSizeKb / 1024;
            var gcHeapSizeGb = gcHeapSizeMb / 1024;
            ImGui.Text("GC Heap Size During Last GC: " + memoryInfo.HeapSizeBytes + " bytes (" + gcHeapSizeKb + " KB, " + gcHeapSizeMb + " MB, " + gcHeapSizeGb + " GB)");
            ImGui.Text("GC High Memory Load Threshold: " + memoryInfo.HighMemoryLoadThresholdBytes);
            ImGui.Text("GC Memory Load: " + memoryInfo.MemoryLoadBytes);

            // Combo box for latency mode
            // Turn enum into string
            var latencyModeEnum = typeof(System.Runtime.GCLatencyMode);
            var latencyModeNames = Enum.GetNames(latencyModeEnum);

            int currentLatencyMode = (int)System.Runtime.GCSettings.LatencyMode;

            if (ImGui.BeginCombo("Latency Mode", latencyModeNames[currentLatencyMode])) {
                for (int i = 0; i < latencyModeNames.Length; i++) {
                    bool isSelected = i == currentLatencyMode;

                    if (ImGui.Selectable(latencyModeNames[i], isSelected)) {
                        System.Runtime.GCSettings.LatencyMode = (System.Runtime.GCLatencyMode)i;
                    }

                    if (isSelected) {
                        ImGui.SetItemDefaultFocus();
                    }
                }

                ImGui.EndCombo();
            }
        }
    }
}