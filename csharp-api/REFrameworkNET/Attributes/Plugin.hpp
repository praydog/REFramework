#pragma once

namespace REFrameworkNET {
    namespace Attributes {
        [System::AttributeUsage(System::AttributeTargets::Method, AllowMultiple = true)]
        public ref class PluginEntryPoint : System::Attribute {
        public:
            PluginEntryPoint() {}
        };
    }
}