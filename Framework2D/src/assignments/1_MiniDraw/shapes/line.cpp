#include "line.h"

#include <imgui.h>

namespace USTC_CG {
// Draw the line using ImGui
void Line::draw_stroke(const Config &config) const
{
    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    draw_list->AddLine(
        ImVec2(
            config.bias[0] + start_point_x_, config.bias[1] + start_point_y_),
        ImVec2(config.bias[0] + end_point_x_, config.bias[1] + end_point_y_),
        IM_COL32(
            config.line_color[0],
            config.line_color[1],
            config.line_color[2],
            config.line_color[3]),
        config.line_thickness);
}

void Line::update(float x, float y)
{
    end_point_x_ = x;
    end_point_y_ = y;
}

bool Line::selected(ImVec2 mouse_p, bool can_selected_inner) const
{
    return is_point_near_line(mouse_p.x, mouse_p.y, start_point_x_, start_point_y_, end_point_x_, end_point_y_,
                              SELECTION_RADIUS);
}

bool Line::is_point_near_line(float px, float py, float x1, float y1, float x2, float y2, float threshold)

{
    // Calculate the distance from point (px, py) to the line segment defined by (x1, y1) and (x2, y2)
    float A = px - x1;
    float B = py - y1;
    float C = x2 - x1;
    float D = y2 - y1;

    float dot = A * C + B * D;
    float len_sq = C * C + D * D;
    float param = (len_sq != 0) ? (dot / len_sq) : -1;

    float nearest_x, nearest_y;

    if (param < 0)
    {
        nearest_x = x1;
        nearest_y = y1;
    } else if (param > 1)
    {
        nearest_x = x2;
        nearest_y = y2;
    } else
    {
        nearest_x = x1 + param * C;
        nearest_y = y1 + param * D;
    }

    float dist_x = px - nearest_x;
    float dist_y = py - nearest_y;
    return (dist_x * dist_x + dist_y * dist_y) <= (threshold * threshold);
}

void Line::move(float x, float y)
{
    start_point_x_ += x;
    start_point_y_ += y;
    end_point_x_ += x;
    end_point_y_ += y;
}
} // namespace USTC_CG
