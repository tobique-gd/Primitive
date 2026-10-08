#include "core/layers/editor_layer.h"
#include "core/layers/render_layer.h"

#include "entrypoint.h"

namespace Engine
{
    class Editor : public Application
    {
    public:
        Editor()
            : Application()
        {
            _layer_stack->push_layer(
                std::make_shared<RenderLayer>(_renderer, _project_manager)
            );
            _layer_stack->push_layer(
                std::make_shared<EditorLayer>(_window, _project_manager, _renderer)
            );

        }

        ~Editor() override = default;
    };

    Application* create_application()
    {
        return new Editor();
    }
}
