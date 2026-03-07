//
// Created by zhdds on 2026/3/7.
//

#include "selector.h"

#include <cmath>
#include <iostream>
#include "canvas_widget.h"
#include "cmath"

namespace USTC_CG {
void Selector::on_mouse_left_click(float x, float y)
{
    begin_point_ = ImVec2(x, y);
    end_point_ = ImVec2(x, y);
}


void Selector::on_mouse_left_release(float x, float y)
{

}

void Selector::on_mouse_move(float x, float y)
{
    selecting_shapes_.clear();
    if (ImGui::IsKeyDown(ImGuiKey_MouseLeft))
    {
        end_point_ = ImVec2(x, y);
        for (const auto& shape_ptr: shapes_)
        {
            if (shape_ptr->selected(begin_point_,end_point_))
            {
                selecting_shapes_.push_back(std::weak_ptr(shape_ptr));
            }
        }

    }else
    {

        for (const auto& shape_ptr: shapes_)
        {
            if (shape_ptr->selected({x,y}))
            {
                selecting_shapes_.push_back(std::weak_ptr(shape_ptr));
            }
        }
        //std::cout<<ImGui::GetIO().MouseWheel;

        static float mouse_wheel_accumulate = 0;
        if (!selecting_shapes_.empty())
        {
            mouse_wheel_accumulate += ImGui::GetIO().MouseWheel;
            if (std::abs(mouse_wheel_accumulate) >2)
            {
                selecting_shapes_index_ += selecting_shapes_.size() + mouse_wheel_accumulate/3 * 1.01;
                selecting_shapes_index_ %= selecting_shapes_.size();
                mouse_wheel_accumulate = 0;
            }

        }
    }

}

void Selector::display(float offset_x, float offset_y) const
{
   if (!selecting_shapes_.empty())
   {
       if (ImGui::IsKeyDown(ImGuiKey_MouseLeft))
       {

       }else
       {
           if (selecting_shapes_index_ < selecting_shapes_.size())
           {
               auto shape_ptr = selecting_shapes_[selecting_shapes_index_].lock();
                auto config = shape_ptr->getConfig();
                config.line_color[0] = 255;
               config.line_color[1] = 255;
               config.line_color[2] = 255;
               config.line_color[3] = 128;
               config.fill_color[0] = 255;
               config.fill_color[1] = 255;
               config.fill_color[2] = 255;
               config.fill_color[3] = 64;
               config.line_thickness += 3.0f;
               config.bias[0] = offset_x;
                config.bias[1] = offset_y;
               shape_ptr->draw(config);
           }
       }


   }
}
}
