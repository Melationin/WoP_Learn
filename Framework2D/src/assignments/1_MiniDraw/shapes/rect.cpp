#include "rect.h"

#include <imgui.h>
#include <vector>

namespace USTC_CG
{
// Draw the rectangle using ImGui
void Rect::draw_stroke(const Config& config) const
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRect(
    ImVec2(
    config.bias[0] + start_point_x_, config.bias[1] + start_point_y_),
    ImVec2(config.bias[0] + end_point_x_, config.bias[1] + end_point_y_),
    IM_COL32(
    config.line_color[0],
    config.line_color[1],
    config.line_color[2],
    config.line_color[3]
    ),
    0.f,  // No rounding of corners
    ImDrawFlags_None,
    config.line_thickness);
}


void Rect::draw_fill(const Config &config) const
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(
        ImVec2(
            config.bias[0] + start_point_x_, config.bias[1] + start_point_y_),
        ImVec2(config.bias[0] + end_point_x_, config.bias[1] + end_point_y_),
        IM_COL32(
            config.fill_color[0],
            config.fill_color[1],
            config.fill_color[2],
            config.fill_color[3]),
        0.f,  // No rounding of corners
        ImDrawFlags_None);
}


void Rect::update(float x, float y)
{
    end_point_x_ = x;
    end_point_y_ = y;
}

bool Rect::selected(ImVec2 point, bool can_selected_inner) const
{
    auto [x,y] = point;

    if (can_selected_inner)
    {
        return x >= std::min(start_point_x_, end_point_x_) &&
               x <= std::max(start_point_x_, end_point_x_) &&
               y >= std::min(start_point_y_, end_point_y_) &&
               y <= std::max(start_point_y_, end_point_y_);
    } else
    {
        float left = std::min(start_point_x_, end_point_x_);
        float right = std::max(start_point_x_, end_point_x_);
        float top = std::min(start_point_y_, end_point_y_);
        float bottom = std::max(start_point_y_, end_point_y_);

        float thickness = SELECTION_RADIUS;
        return (x >= left - thickness && x <= left + thickness && y >= top - thickness && y <= bottom + thickness) ||
               (x >= right - thickness && x <= right + thickness && y >= top - thickness && y <= bottom + thickness) ||
               (y >= top - thickness && y <= top + thickness && x >= left - thickness && x <= right + thickness) ||
               (y >= bottom - thickness && y <= bottom + thickness && x >= left - thickness && x <= right + thickness);
    }
}
}  // namespace USTC_CG
