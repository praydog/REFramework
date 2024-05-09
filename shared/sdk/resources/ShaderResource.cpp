#include <spdlog/spdlog.h>
#include <utility/Scan.hpp>
#include <utility/Module.hpp>

#include "../MurmurHash.hpp"

#include "ShaderResource.hpp"

namespace sdk::renderer {
ShaderResource::FindFn ShaderResource::get_find_fn() {
    static FindFn fn = []() -> FindFn {
        spdlog::info("[ShaderResource::get_find_fn] Scanning for ShaderResource::find");

        const auto game = utility::get_executable();
        const auto string_data = utility::scan_string(game, "UpdateDepthBlockerState");

        if (!string_data) {
            spdlog::error("[ShaderResource::get_find_fn] Failed to find UpdateDepthBlockerState string");
            return nullptr;
        }

        const auto string_ref = utility::scan_displacement_reference(game, *string_data);

        if (!string_ref) {
            spdlog::error("[ShaderResource::get_find_fn] Failed to find UpdateDepthBlockerState reference");
            return nullptr;
        }

        const auto next_fn_call = utility::scan_mnemonic(*string_ref + 4, 100, "CALL");

        if (!next_fn_call) {
            spdlog::error("[ShaderResource::get_find_fn] Failed to find next CALL instruction");
            return nullptr;
        }

        const auto next_next_fn_call = utility::scan_mnemonic(*next_fn_call + 5, 100, "CALL");

        if (!next_next_fn_call) {
            spdlog::error("[ShaderResource::get_find_fn] Failed to find next next CALL instruction");
            return nullptr;
        }

        const auto result = utility::resolve_displacement(*next_next_fn_call);

        if (!result) {
            spdlog::error("[ShaderResource::get_find_fn] Failed to resolve displacement");
            return nullptr;
        }

        spdlog::info("[ShaderResource::get_find_fn] Found ShaderResource::find at {0:x}", *result);

        return (FindFn)*result;
    }();

    return fn;
}

sdk::renderer::PipelineState* ShaderResource::find(uint32_t murmur_hash, uint8_t unk) {
    auto fn = get_find_fn();

    if (fn == nullptr) {
        return nullptr;
    }

    return fn(this, murmur_hash, unk);
}

sdk::renderer::PipelineState* ShaderResource::find(std::string_view name, uint8_t unk) {
    const auto murmur_hash = sdk::murmur_hash::calc32_as_utf8(name.data());

    return find(murmur_hash, unk);
}
}