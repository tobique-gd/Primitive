// src/editor/ui_context.cpp
#include "editor/ui/ui_context.h"
#include "editor/ui/ui_theme.h"
#include "editor/ui/IconsFontAwesome6.h"
#include "systems/display/iwindow.h"

#include "imgui.h"

#include <stdexcept>
#include <utility>

namespace Primitive {
    UIContext::UIContext(std::unique_ptr<IUIPlatformBackend> platform,
                         std::unique_ptr<IUIRendererBackend> renderer)
                        : m_platform(std::move(platform))
                        , m_renderer(std::move(renderer)) {
        if (!m_platform || !m_renderer)
            throw std::invalid_argument("UIContext requires both a platform and a renderer backend");
    }

    UIContext::~UIContext()
    {
        shutdown();
    }

    void UIContext::init(IWindow& window)
    {
        if (m_initialized) return;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        load_fonts();
        UIStyle::setup_ui_style();

        m_platform->init(window);
        m_renderer->init();

        m_initialized = true;
    }

    void UIContext::shutdown()
    {
        if (!m_initialized) return;

        m_renderer->shutdown();
        m_platform->shutdown();
        ImGui::DestroyContext();

        m_initialized = false;
    }

    void UIContext::load_fonts()
    {
        ImGuiIO& io = ImGui::GetIO();

        io.Fonts->AddFontFromFileTTF("assets/fonts/IBMPlexSans-Regular.ttf", 16.0f);

        static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };

        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;

        io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 16.0f, &icons_config, icon_ranges);
    }

    void UIContext::begin_frame(void* target_pass_descriptor)
    {
        m_renderer->new_frame(target_pass_descriptor);
        m_platform->new_frame();
        ImGui::NewFrame();
    }

    void UIContext::end_frame()
    {
        ImGui::Render();
    }

    ImDrawData* UIContext::draw_data() const
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        return ImGui::GetDrawData();
    }

    void UIContext::render(void* native_command_buffer, void* native_encoder)
    {
        m_renderer->render(draw_data(), native_command_buffer, native_encoder);
    }
}
