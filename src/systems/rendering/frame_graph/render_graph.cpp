#include "render_graph.h"

#include <limits>
#include <stdexcept>
#include <utility>

namespace Primitive {

    RenderGraphBuilder::RenderGraphBuilder(
        std::vector<RenderGraphResourceUse>& uses
    )
        : m_uses(uses) {
    }

    void RenderGraphBuilder::read(RenderGraphResource resource) {
        m_uses.push_back({resource, RenderGraphAccess::Read});
    }

    void RenderGraphBuilder::write(RenderGraphResource resource) {
        m_uses.push_back({resource, RenderGraphAccess::Write});
    }

    void RenderGraphBuilder::read_write(RenderGraphResource resource) {
        m_uses.push_back({resource, RenderGraphAccess::ReadWrite});
    }

    RenderGraphResource RenderGraph::create_texture(
        const RenderGraphResourceDesc& desc
    ) {
        if (m_resources.size() >= InvalidRenderGraphResource) {
            throw std::overflow_error("Render graph resource limit reached");
        }

        if (desc.texture.width == 0 || desc.texture.height == 0) {
            throw std::invalid_argument("Render graph texture dimensions must be non-zero");
        }

        if (desc.texture.sample_count == 0) {
            throw std::invalid_argument("Render graph sample count must be non-zero");
        }

        ResourceNode node;
        node.desc = desc;
        node.desc.imported = false;

        const auto resource = static_cast<RenderGraphResource>(m_resources.size());

        m_resources.push_back(std::move(node));
        m_compiled = false;

        return resource;
    }

    RenderGraphResource RenderGraph::import_texture(
        const std::string& name,
        Texture2D& texture
    ) {
        if (m_resources.size() >= InvalidRenderGraphResource) {
            throw std::overflow_error("Render graph resource limit reached");
        }

        ResourceNode node;
        node.desc.name = name;
        node.desc.imported = true;
        node.desc.texture.width = texture.get_width();
        node.desc.texture.height = texture.get_height();
        node.desc.texture.sample_count = 1;
        node.imported_texture = &texture;

        const auto resource = static_cast<RenderGraphResource>(m_resources.size());

        m_resources.push_back(std::move(node));
        m_compiled = false;

        return resource;
    }

    RenderGraphPass RenderGraph::add_pass(RenderGraphPassDesc desc) {
        if (m_passes.size() >= InvalidRenderGraphResource) {
            throw std::overflow_error("Render graph pass limit reached");
        }

        if (!desc.execute) {
            throw std::invalid_argument("Render graph pass requires an execute callback");
        }

        PassNode node;
        node.desc = std::move(desc);

        const auto pass = static_cast<RenderGraphPass>(m_passes.size());

        m_passes.push_back(std::move(node));
        m_compiled = false;

        return pass;
    }

    void RenderGraph::compile() {
        m_execution_order.clear();

        for (RenderGraphPass pass = 0; pass < m_passes.size(); ++pass) {
            auto& node = m_passes[pass];
            node.resources.clear();

            if (node.desc.setup) {
                RenderGraphBuilder builder(node.resources);
                node.desc.setup(builder);
            }

            for (const auto& use : node.resources) {
                if (use.resource >= m_resources.size()) {
                    throw std::out_of_range(
                        "Render graph pass references an invalid resource: " +
                        node.desc.name
                    );
                }
            }

            m_execution_order.push_back(pass);
        }

        m_compiled = true;
    }

    void RenderGraph::execute(RenderContext& context) {
        if (!m_compiled) {
            compile();
        }

        for (const auto pass : m_execution_order) {
            m_passes[pass].desc.execute(context);
        }
    }

    void RenderGraph::clear() {
        m_resources.clear();
        m_passes.clear();
        m_execution_order.clear();
        m_compiled = false;
    }

}
