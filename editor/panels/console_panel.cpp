
#include "editor/panels/console_panel.h"
#include "core/events/rendering_events.h"
#include "editor/ui_context.h"
#include "imgui.h"
#include <memory>

namespace Engine {
    void ConsolePanel::on_imgui_render() {

        ImGui::Begin("Output");

        ImGui::End();

    }


}
