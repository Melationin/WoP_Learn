//
// Created by zhdds on 2026/3/3.
//

#include "freehand.h"

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
} // USTC_GC