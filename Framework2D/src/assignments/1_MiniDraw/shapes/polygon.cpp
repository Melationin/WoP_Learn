//
// Created by zhdds on 2026/3/3.
//

#include "polygon.h"
#include "line.h"

namespace USTC_CG {
void Polygon::update(float x, float y)
{
    points_list_[points_list_.size() - 1] = ImVec2(x, y);
}

void Polygon::add_control_point(float x, float y)
{
    points_list_.emplace_back(x, y);
}

bool Polygon::selected(ImVec2 mouse_p, bool can_selected_inner) const
{
    if (can_selected_inner)
    {
        int crossing_count = 0;
        int n = points_list_.size();
        for (int i = 0; i < n; ++i)
        {
            const auto &p1 = points_list_[i];
            const auto &p2 = points_list_[(i + 1) % n];
            // 检查射线是否与边相交（边的两端点分别在射线上下）
            if (((p1.y > mouse_p.y) != (p2.y > mouse_p.y)))
            {
                // 计算交点 x 坐标
                float x_intersect = p1.x + (p2.x - p1.x) * (mouse_p.y - p1.y) / (p2.y - p1.y);
                if (x_intersect > mouse_p.x)
                {
                    ++crossing_count;
                }
            }
        }
        return (crossing_count % 2) == 1;
    } else
    {
        static auto is_near_line = Line::is_point_near_line;

        for (int i = 0; i < points_list_.size(); i++)
        {
            auto [x1,y1] = points_list_[i];
            auto [x2,y2] = points_list_[(i + 1) % points_list_.size()];

            if (is_near_line(mouse_p.x, mouse_p.y, x1, y1, x2, y2, SELECTION_RADIUS))
            {
                return true;
            }
        }
        return false;
    }
}

void Polygon::move(float x, float y)
{
    for (auto &it: points_list_)
    {
        it.x += x;
        it.y += y;
    }
}

void Polygon::draw_stroke(const Config &config) const
{
    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    std::vector<ImVec2> point_with_offset;
    point_with_offset.reserve(points_list_.size());
    for (auto [x,y]: points_list_)
    {
        point_with_offset.emplace_back(x + config.bias[0], y + config.bias[1]);
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
        point_with_offset.emplace_back(x + config.bias[0], y + config.bias[1]);
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
