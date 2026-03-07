//
// Created by zhdds on 2026/3/3.
//

#include "polygon.h"

namespace USTC_CG {
void Polygon::update(float x, float y)
{
    points_list_[points_list_.size() - 1] = ImVec2(x, y);
}

void Polygon::add_control_point(float x, float y)
{
    points_list_.emplace_back(x, y);
}

void Polygon::draw_stroke(const Config &config) const
{
    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    std::vector<ImVec2> point_with_offset;
    point_with_offset.reserve(points_list_.size());
    for (auto [x,y]: points_list_)
    {
        point_with_offset.emplace_back(x + config.bias[0],y + config.bias[1]);
    }

    draw_list->AddPolyline(
        point_with_offset.data(),
        point_with_offset.size(),
        IM_COL32(
            config.line_color[0],
            config.line_color[1],
            config.line_color[2],
            config.line_color[3]
        ),
        0,
        config.line_thickness
    );
}

void Polygon::draw_fill(const Config &config) const
{
    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    std::vector<ImVec2> point_with_offset;
    point_with_offset.reserve(points_list_.size());
    for (auto [x,y]: points_list_)
    {
        point_with_offset.emplace_back(x + config.bias[0],y + config.bias[1]);
    }
    draw_list->AddConcavePolyFilled(
        point_with_offset.data(),
        point_with_offset.size(),
        IM_COL32(
            config.fill_color[0],
            config.fill_color[1],
            config.fill_color[2],
            config.fill_color[3]
        )
    );
};
} // USTC_CG
