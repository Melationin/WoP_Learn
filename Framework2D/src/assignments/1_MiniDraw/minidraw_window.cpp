#include "minidraw_window.h"

#include <iostream>

namespace USTC_CG
{
MiniDraw::MiniDraw(const std::string& window_name) : Window(window_name)
{
    p_canvas_ = std::make_shared<Canvas>("Widget.Canvas");
}

MiniDraw::~MiniDraw() = default;

void MiniDraw::draw()
{
    draw_canvas();
}

void MiniDraw::draw_canvas()
{
    // Set a full screen canvas view
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    if (ImGui::Begin(
            "Canvas",
            &flag_show_canvas_view_,
            ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_NoBackground))
    {
        // Buttons for shape types
        if (ImGui::Button("Clear"))
        {
            p_canvas_->set_default();
            p_canvas_->clear_shape_list();
        }
        ImGui::SameLine();
        if (ImGui::Button("Select"))
        {
            p_canvas_->set_selector();
        }

        if (p_canvas_->hasShapeSelected())
        {
            ImGui::SameLine();
            if (ImGui::Button("Shape Setter"))
            {
                p_canvas_->set_shape_setter();
            }
            ImGui::SameLine();
            if (ImGui::Button("Shape Mover"))
            {
                p_canvas_->set_shape_mover();
            }
            ImGui::SameLine();
            if (ImGui::Button("Remove Selected"))
            {
                p_canvas_->remove_selected();
            }
        }

        if (ImGui::Button("Line"))
        {
            std::cout << "Set shape to Line" << std::endl;
            p_canvas_->set_line();
        }
        ImGui::SameLine();
        if (ImGui::Button("Rect"))
        {
            std::cout << "Set shape to Rect" << std::endl;
            p_canvas_->set_rect();
        }
        ImGui::SameLine();
        if (ImGui::Button("Ellipse"))
        {

            std::cout << "Set shape to ellipse" << std::endl;
            p_canvas_->set_ellipse();
        }
        ImGui::SameLine();
        if (ImGui::Button(" Polygon"))
        {
            std::cout << "Set shape to polygon" << std::endl;
            p_canvas_->set_polygon();
        }
        ImGui::SameLine();
        if (ImGui::Button("Freehand"))
        {
            std::cout << "Set shape to Freehand" << std::endl;
            p_canvas_->set_freehand();
        }

        static bool show_line_color_editor = false;
        static bool show_fill_color_editor = false;
        static bool fill_shape = false;
        static float line_thickness = 2.0f;
        static ImVec4  line_color = {0, 1, 0, 1};
        static ImVec4  fill_color = {1, 0, 0, 1};

        show_line_color_editor = ImGui::ColorButton("Line Color  ",line_color, ImGuiColorEditFlags_NoColorMarkers )^show_line_color_editor;
        ImGui::SameLine();
        ImGui::Text("Line Color");
        ImGui::SameLine();

        ImGui::SetNextItemWidth(300);
        ImGui::SliderFloat("Line Thickness  ", &line_thickness, 0.01, 10.0);

        ImGui::SameLine();
        ImGui::Checkbox("Fill Shape", &fill_shape);
        if (fill_shape)
        {   ImGui::SameLine();
            show_fill_color_editor = ImGui::ColorButton("Fill Color  ",fill_color, ImGuiColorEditFlags_NoColorMarkers )^show_fill_color_editor;
            ImGui::SameLine();
            ImGui::Text("Fill Color");
        }else
        {
            show_fill_color_editor = false;
        }


        if (   show_line_color_editor     )
        {
            ImGui::ColorEdit4("Line Color",&line_color.x,ImGuiColorEditFlags_PickerHueWheel );
        }
        if (   show_fill_color_editor     )
        {
            ImGui::ColorEdit4("Fill Color",&fill_color.x,ImGuiColorEditFlags_PickerHueWheel );
        }



        // Canvas component
        ImGui::Text("Press left mouse to add shapes.");
        const auto& canvas_min = ImGui::GetCursorScreenPos();
        const auto& canvas_size = ImGui::GetContentRegionAvail();
        p_canvas_->set_attributes(canvas_min, canvas_size);

        p_canvas_->set_line_color(line_color);
        p_canvas_->set_line_thickness(line_thickness);
        p_canvas_->set_fill_mode(fill_shape);
        p_canvas_->set_fill_color(fill_color);

        p_canvas_->draw();
    }
    ImGui::End();
}
}  // namespace USTC_CG