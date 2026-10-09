#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <Metal/Metal.hpp>

namespace Engine::RHI
{

    enum class PrimitiveTopology {
        TriangleList,
        TriangleStrip,
        LineList,
        PointList
    };

    enum class CullMode {
        None,
        Front,
        Back
    };

    enum class WindingOrder {
        CounterClockwise,
        Clockwise
    };

    enum class CompareFunc {
        Never,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always
    };

    enum class BlendFactor {
        Zero,
        One,
        SrcColor,
        OneMinusSrcColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstAlpha,
        OneMinusDstAlpha
    };

    enum class BlendOp {
        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max
    };

    struct BlendState {
        bool enable_blending = false;
        BlendFactor src_color_factor = BlendFactor::SrcAlpha;
        BlendFactor dst_color_factor = BlendFactor::OneMinusSrcAlpha;
        BlendOp color_op = BlendOp::Add;
        BlendFactor src_alpha_factor = BlendFactor::One;
        BlendFactor dst_alpha_factor = BlendFactor::Zero;
        BlendOp alpha_op = BlendOp::Add;
    };

    struct DepthStencilDesc {
        bool depth_test_enable = true;
        bool depth_write_enable = true;
        CompareFunc depth_compare_op = CompareFunc::LessEqual;
    };

    struct RasterizerDesc {
        CullMode cull_mode = CullMode::Back;
        WindingOrder winding_order = WindingOrder::Clockwise;
        bool wireframe = false;
    };

    struct PipelineDesc {
        void* vertex_shader = nullptr;
        void* fragment_shader = nullptr;

        RasterizerDesc rasterizer;
        DepthStencilDesc depth_stencil;
        BlendState blend_state;
        PrimitiveTopology topology = PrimitiveTopology::TriangleList;

        uint32_t color_format = 0;
        uint32_t depth_format = 0;
        int sample_count = 4;

        std::string debug_name;
    };

    class IRHIPipelineState {
        public:
            virtual ~IRHIPipelineState() = default;

            virtual const std::string& get_debug_name() const = 0;
    };

}
