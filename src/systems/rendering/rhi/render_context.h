#pragma once

#include "systems/rendering/rhi/command_list.h"

namespace Primitive::RHI {

    class RenderContext {
    public:
        explicit RenderContext(RHI::ICommandList& command_list)
            : m_command_list(command_list)
        {
        }

        RHI::ICommandList& get_command_list()
        {
            return m_command_list;
        }

    private:
        RHI::ICommandList& m_command_list;
    };

}
