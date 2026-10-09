
#pragma once

#include "imgui.h"

namespace Engine {

    struct UIColors {
        static inline const ImVec4 background        = ImVec4(0.690f, 0.700f, 0.715f, 1.0f);
        static inline const ImVec4 panel             = ImVec4(0.820f, 0.825f, 0.835f, 1.0f);
        static inline const ImVec4 panel_header      = ImVec4(0.745f, 0.755f, 0.770f, 1.0f);
        static inline const ImVec4 panel_hover       = ImVec4(0.875f, 0.885f, 0.900f, 1.0f);
        static inline const ImVec4 content           = ImVec4(0.930f, 0.935f, 0.945f, 1.0f);
        static inline const ImVec4 input             = ImVec4(0.965f, 0.968f, 0.975f, 1.0f);
        static inline const ImVec4 border            = ImVec4(0.600f, 0.615f, 0.635f, 1.0f);
        static inline const ImVec4 separator         = ImVec4(0.690f, 0.700f, 0.715f, 1.0f);
        static inline const ImVec4 text              = ImVec4(0.105f, 0.115f, 0.135f, 1.0f);
        static inline const ImVec4 text_disabled     = ImVec4(0.420f, 0.435f, 0.455f, 1.0f);
        static inline const ImVec4 accent        = ImVec4(0.420f, 0.690f, 0.850f, 1.0f);
        static inline const ImVec4 accent_hover  = ImVec4(0.330f, 0.570f, 0.760f, 1.0f);
        static inline const ImVec4 accent_active = ImVec4(0.240f, 0.440f, 0.620f, 1.0f);
        static inline const ImVec4 selection         = ImVec4(0.310f, 0.490f, 0.710f, 0.32f);
        static inline const ImVec4 transparent       = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    };

    class UIStyle {
    public:
        static void setup_ui_style() {
            ImGuiStyle& style = ImGui::GetStyle();
            ImVec4* colors = style.Colors;

            style.WindowPadding = ImVec2(8.0f, 8.0f);
            style.FramePadding = ImVec2(8.0f, 5.0f);
            style.CellPadding = ImVec2(6.0f, 5.0f);
            style.ItemSpacing = ImVec2(8.0f, 6.0f);
            style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
            style.IndentSpacing = 18.0f;
            style.ScrollbarSize = 13.0f;
            style.GrabMinSize = 10.0f;

            style.WindowRounding = 6.0f;
            style.ChildRounding = 6.0f;
            style.FrameRounding = 6.0f;
            style.PopupRounding = 4.0f;
            style.ScrollbarRounding = 4.0f;
            style.GrabRounding = 3.0f;
            style.TabRounding = 3.0f;

            style.WindowBorderSize = 0.0f;
            style.ChildBorderSize = 1.0f;
            style.PopupBorderSize = 1.0f;
            style.FrameBorderSize = 0.0f;
            style.TabBorderSize = 0.0f;

            style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
            style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
            style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

            colors[ImGuiCol_Text] = UIColors::text;
            colors[ImGuiCol_TextDisabled] = UIColors::text_disabled;

            colors[ImGuiCol_WindowBg] = UIColors::panel;
            colors[ImGuiCol_ChildBg] = UIColors::content;
            colors[ImGuiCol_PopupBg] = UIColors::content;

            colors[ImGuiCol_Border] = UIColors::border;
            colors[ImGuiCol_BorderShadow] = UIColors::transparent;

            colors[ImGuiCol_FrameBg] = UIColors::input;
            colors[ImGuiCol_FrameBgHovered] = UIColors::panel_hover;
            colors[ImGuiCol_FrameBgActive] = UIColors::panel_hover;

            colors[ImGuiCol_TitleBg] = UIColors::panel_header;
            colors[ImGuiCol_TitleBgActive] = UIColors::panel_header;
            colors[ImGuiCol_TitleBgCollapsed] = UIColors::panel_header;

            colors[ImGuiCol_MenuBarBg] = UIColors::background;

            colors[ImGuiCol_ScrollbarBg] = UIColors::panel;
            colors[ImGuiCol_ScrollbarGrab] = UIColors::separator;
            colors[ImGuiCol_ScrollbarGrabHovered] = UIColors::accent;
            colors[ImGuiCol_ScrollbarGrabActive] = UIColors::accent_hover;

            colors[ImGuiCol_CheckMark] = UIColors::accent;
            colors[ImGuiCol_SliderGrab] = UIColors::accent;
            colors[ImGuiCol_SliderGrabActive] = UIColors::accent_hover;

            colors[ImGuiCol_Button] = UIColors::panel_header;
            colors[ImGuiCol_ButtonHovered] = UIColors::panel_hover;
            colors[ImGuiCol_ButtonActive] = UIColors::selection;

            colors[ImGuiCol_Header] = UIColors::selection;
            colors[ImGuiCol_HeaderHovered] = UIColors::panel_hover;
            colors[ImGuiCol_HeaderActive] = UIColors::selection;

            colors[ImGuiCol_Separator] = ImVec4(1, 0, 1, 1);
            colors[ImGuiCol_SeparatorHovered] = UIColors::accent;
            colors[ImGuiCol_SeparatorActive] = UIColors::accent_hover;

            colors[ImGuiCol_ResizeGrip] = UIColors::transparent;
            colors[ImGuiCol_ResizeGripHovered] = UIColors::accent;
            colors[ImGuiCol_ResizeGripActive] = UIColors::accent_active;

            colors[ImGuiCol_Tab] = UIColors::panel_header;
            colors[ImGuiCol_TabHovered] = UIColors::panel_hover;
            colors[ImGuiCol_TabActive] = UIColors::content;
            colors[ImGuiCol_TabUnfocused] = UIColors::panel_header;
            colors[ImGuiCol_TabUnfocusedActive] = UIColors::panel;

            colors[ImGuiCol_TableHeaderBg] = UIColors::panel_header;
            colors[ImGuiCol_TableBorderStrong] = UIColors::border;
            colors[ImGuiCol_TableBorderLight] = UIColors::separator;
            colors[ImGuiCol_TableRowBg] = UIColors::transparent;
            colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.0f, 0.0f, 0.0f, 0.025f);

            colors[ImGuiCol_PlotLines] = UIColors::accent;
            colors[ImGuiCol_PlotLinesHovered] = UIColors::accent_hover;
            colors[ImGuiCol_PlotHistogram] = UIColors::accent;
            colors[ImGuiCol_PlotHistogramHovered] = UIColors::accent_hover;

            colors[ImGuiCol_TextSelectedBg] = UIColors::selection;
            colors[ImGuiCol_DragDropTarget] = UIColors::accent_hover;
            colors[ImGuiCol_NavHighlight] = UIColors::accent;
            colors[ImGuiCol_NavWindowingHighlight] = UIColors::accent;
            colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.20f);
            colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.35f);

            #ifdef IMGUI_HAS_DOCK
                    colors[ImGuiCol_DockingPreview] = ImVec4(0.25f, 0.45f, 0.72f, 0.45f);
                    colors[ImGuiCol_DockingEmptyBg] = UIColors::background;
                    style.DockingSeparatorSize = 0.0f;
            #endif
        }
    };

}
