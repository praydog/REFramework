#pragma once

namespace REFrameworkNET {
    namespace Attributes {
        [System::AttributeUsage(System::AttributeTargets::Method, AllowMultiple = true)]
        public ref class PluginEntryPoint : System::Attribute {
        public:
            PluginEntryPoint() {}
        };

        [System::AttributeUsage(System::AttributeTargets::Method, AllowMultiple = true)]
        public ref class PluginExitPoint : System::Attribute {
        public:
            PluginExitPoint() {}
        };
    }
}