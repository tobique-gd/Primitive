#pragma once

#include "systems/rendering/rhi/command_list.h"

namespace Primitive {

    class RenderContext {
    public:
        explicit RenderContext(ICommandList& command_list)
            : m_command_list(command_list)
        {
        }

        ICommandList& get_command_list()
        {
            return m_command_list;
        }

    private:
        ICommandList& m_command_list;
    };

}
