
#include "editor/panels/viewport_panel.h"
#include "core/events/event_type.h"
#include "core/events/new_frame_rendered_event.h"
#include "imgui.h"
#include "rendering/itexture_2d.h"
#include <memory>

namespace Engine {
    void ViewportPanel::set_texture(std::shared_ptr<ITexture2D> texture) {
        m_texture = texture;
    }

    inline void SetupImGuiStyle( bool bStyleDark_, float alpha_  )
        {
            ImGuiStyle& style = ImGui::GetStyle();

            // light style from Pacôme Danhiez (user itamago) https://github.com/ocornut/imgui/pull/511#issuecomment-175719267
            style.Alpha = 1.0f;
            style.FrameRounding = 3.0f;
            style.Colors[ImGuiCol_Text]                  = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
            style.Colors[ImGuiCol_TextDisabled]          = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
            style.Colors[ImGuiCol_WindowBg]              = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);

            style.Colors[ImGuiCol_PopupBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
            style.Colors[ImGuiCol_Border]                = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
            style.Colors[ImGuiCol_BorderShadow]          = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
            style.Colors[ImGuiCol_FrameBg]               = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
            style.Colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
            style.Colors[ImGuiCol_FrameBgActive]         = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
            style.Colors[ImGuiCol_TitleBg]               = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
            style.Colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
            style.Colors[ImGuiCol_TitleBgActive]         = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
            style.Colors[ImGuiCol_MenuBarBg]             = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
            style.Colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
            style.Colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
            style.Colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
            style.Colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);

            style.Colors[ImGuiCol_CheckMark]             = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
            style.Colors[ImGuiCol_SliderGrab]            = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
            style.Colors[ImGuiCol_SliderGrabActive]      = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
            style.Colors[ImGuiCol_Button]                = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
            style.Colors[ImGuiCol_ButtonHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
            style.Colors[ImGuiCol_ButtonActive]          = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
            style.Colors[ImGuiCol_Header]                = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
            style.Colors[ImGuiCol_HeaderHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
            style.Colors[ImGuiCol_HeaderActive]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);

            style.Colors[ImGuiCol_ResizeGrip]            = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
            style.Colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
            style.Colors[ImGuiCol_ResizeGripActive]      = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);

            style.Colors[ImGuiCol_PlotLines]             = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
            style.Colors[ImGuiCol_PlotLinesHovered]      = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
            style.Colors[ImGuiCol_PlotHistogram]         = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
            style.Colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
            style.Colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);

            if( bStyleDark_ )
            {
                for (int i = 0; i <= ImGuiCol_COUNT; i++)
                {
                    ImVec4& col = style.Colors[i];
                    float H, S, V;
                    ImGui::ColorConvertRGBtoHSV( col.x, col.y, col.z, H, S, V );

                    if( S < 0.1f )
                    {
                       V = 1.0f - V;
                    }
                    ImGui::ColorConvertHSVtoRGB( H, S, V, col.x, col.y, col.z );
                    if( col.w < 1.00f )
                    {
                        col.w *= alpha_;
                    }
                }
            }
            else
            {
                for (int i = 0; i <= ImGuiCol_COUNT; i++)
                {
                    ImVec4& col = style.Colors[i];
                    if( col.w < 1.00f )
                    {
                        col.x *= alpha_;
                        col.y *= alpha_;
                        col.z *= alpha_;
                        col.w *= alpha_;
                    }
                }
            }
        }

    void ViewportPanel::on_imgui_render() {

        // imgui started page
        ImGui::SetNextWindowPos(ImVec2(0, 0));                                                  // always at the window origin
        ImGui::SetNextWindowSize(ImVec2(float(1280), float(720)));    // always at the window size

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoBringToFrontOnFocus |                 // we just want to use this window as a host for the menubar and docking
            ImGuiWindowFlags_NoNavFocus |                                                      // so turn off everything that would make it act like a window
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_MenuBar |
            ImGuiWindowFlags_NoBackground;

        SetupImGuiStyle(false, 1.0);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));                          // we don't want any padding for windows docked to this window frame

        bool show = (ImGui::Begin("Main", NULL, windowFlags));                                   // show the "window"
        ImGui::PopStyleVar();                                                                    // restore the style so inner windows have fames

        // create a docking space inside our inner window that lets prevents anything from docking in the central node (so we can see our game content)
        ImGui::DockSpace(ImGui::GetID("Dockspace"), ImVec2(0.0f, 0.0f),  ImGuiDockNodeFlags_PassthruCentralNode);
        if (show)
        {
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Exit")) {

                    }

                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
        }

        ImGui::Begin("Viewport");
        if (m_texture) {
        ImGui::Image(m_texture->get_native_handle(), ImGui::GetContentRegionAvail());
        }
        ImGui::End();

        ImGui::End();
    }

    void ViewportPanel::on_event(std::shared_ptr<IEvent> event) {
        if (event->get_type() != EventType::NEW_FRAME_RENDERED) {
            return;
        }

        auto viewport_event = std::static_pointer_cast<NewFrameRenderedEvent>(event);

        set_texture(viewport_event->frame);
    }
}
