#pragma once

#include "../ResourceManager.hpp"
#include "../renderer/PipelineState.hpp"

namespace sdk::renderer {
class ShaderResource : public sdk::Resource {
public:
    using FindFn = PipelineState* (*)(ShaderResource*, uint32_t, uint8_t);
    static FindFn get_find_fn();

    PipelineState* find(uint32_t murmur_hash, uint8_t unk);
    PipelineState* find(std::string_view name, uint8_t unk);

private:
};
}