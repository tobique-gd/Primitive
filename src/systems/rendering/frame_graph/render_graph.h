
#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "systems/rendering/rhi/texture_2d.h"

namespace Primitive {
    using RenderGraphResource = uint32_t;
    using RenderGraphPass = uint32_t;

    constexpr RenderGraphResource InvalidRenderGraphResource = UINT32_MAX;

    enum class RenderGraphAccess {
        Read,
        Write,
        ReadWrite
    };

    struct RenderGraphTextureDesc {
        uint32_t width = 1;
        uint32_t height = 1;
        uint32_t sample_count = 1;
        TextureFormat format;
    };

    struct RenderGraphResourceDesc {
        std::string name;
        RenderGraphTextureDesc texture;
        bool imported = false;
    };

    struct RenderGraphResourceUse {
        RenderGraphResource resource = InvalidRenderGraphResource;
        RenderGraphAccess access = RenderGraphAccess::Read;
    };

    class RenderContext;

    class RenderGraphBuilder {
        public:
            void read(RenderGraphResource resource);
            void write(RenderGraphResource resource);
            void read_write(RenderGraphResource resource);

        private:
            friend class RenderGraph;

            explicit RenderGraphBuilder(std::vector<RenderGraphResourceUse>& uses);

            std::vector<RenderGraphResourceUse>& m_uses;
    };

    struct RenderGraphPassDesc {
        std::string name;
        std::function<void(RenderGraphBuilder&)> setup;
        std::function<void(RenderContext&)> execute;
    };

    class RenderGraph {
        public:
            RenderGraphResource create_texture(const RenderGraphResourceDesc& desc);
            RenderGraphResource import_texture(const std::string& name, Texture2D& texture);

            RenderGraphPass add_pass(RenderGraphPassDesc desc);

            void compile();
            void execute(RenderContext& context);
            void clear();

        private:
            struct ResourceNode {
                RenderGraphResourceDesc desc;
                Texture2D* imported_texture = nullptr;
            };

            struct PassNode {
                RenderGraphPassDesc desc;
                std::vector<RenderGraphResourceUse> resources;
            };

            std::vector<ResourceNode> m_resources;
            std::vector<PassNode> m_passes;
            std::vector<RenderGraphPass> m_execution_order;
            bool m_compiled = false;
        };
}
