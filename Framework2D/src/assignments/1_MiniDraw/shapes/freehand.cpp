//
// Created by zhdds on 2026/3/3.
//

#include "freehand.h"
#include "line.h"
namespace USTC_CG {
    void Freehand::draw_stroke(const Config &config) const
    {
        auto draw_list =  ImGui::GetWindowDrawList();

        for (int i = 0 ;i<points_list_.size()-1;i++)
        {
            ImVec2 start_point = points_list_[i];
            ImVec2 end_point = points_list_[i+1];

            draw_list->AddLine(
          ImVec2(
              config.bias[0] + start_point.x, config.bias[1] + start_point.y),
          ImVec2(config.bias[0] + end_point.x, config.bias[1] + end_point.y),
          IM_COL32(
              config.line_color[0],
              config.line_color[1],
              config.line_color[2],
              config.line_color[3]),
          config.line_thickness);
        }
    }

    void Freehand::update(float x, float y)
    {
        auto [lx,ly] = points_list_[points_list_.size() - 1];
        if (lx!=x || ly!=y)points_list_.emplace_back(x, y);
    }

    void Freehand::add_control_point(float x, float y)
    {

    }

    bool Freehand::selected(ImVec2 mouse_p, bool can_selected_inner) const
    {
        static auto is_near_line = Line::is_point_near_line;

        for (int i = 0;i<points_list_.size();i++)
        {
            auto [x1,y1] = points_list_[i];
            auto [x2,y2] = points_list_[(i+1)%points_list_.size()];

            if (is_near_line(mouse_p.x, mouse_p.y, x1, y1, x2, y2, SELECTION_RADIUS))
            {
                return true;
            }
        }
        return false;
    }

    void Freehand::move(float x, float y)
    {
        for (auto &it: points_list_)
        {
            it.x += x;
            it.y += y;
        }
    }
} // USTC_GC