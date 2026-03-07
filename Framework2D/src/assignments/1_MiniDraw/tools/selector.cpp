//
// Created by zhdds on 2026/3/7.
//

#include "selector.h"


#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>



namespace USTC_CG {
void Selector::on_mouse_left_click(float x, float y)
{;

    if (!selecting_shapes_.empty())
    {
        if (!ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
        {
            selected_shapes_.clear();
        }
        selected_shapes_.push_back(selecting_shapes_[selecting_shapes_index_]);
    }
}

void Selector::on_mouse_right_click(float x, float y)
{
    selected_shapes_.clear();
}

void Selector::on_mouse_move(float x, float y)
{
    selecting_shapes_.clear();
    if (ImGui::IsKeyDown(ImGuiKey_MouseLeft))
    {
    } else
    {
        for (const auto &shape_ptr: shapes_)
        {
            if (shape_ptr->selected({x, y}))
            {
                selecting_shapes_.push_back(std::weak_ptr(shape_ptr));
            }
        }
        //std::cout<<ImGui::GetIO().MouseWheel;

        static float mouse_wheel_accumulate = 0;
        if (!selecting_shapes_.empty())
        {
            mouse_wheel_accumulate += ImGui::GetIO().MouseWheel;
            if (std::abs(mouse_wheel_accumulate) > 2)
            {
                selecting_shapes_index_ += selecting_shapes_.size() + mouse_wheel_accumulate / 3 * 1.01;
                mouse_wheel_accumulate = 0;
            }
            selecting_shapes_index_ %= selecting_shapes_.size();
        } else
        {
            selecting_shapes_index_ = 0;
        }
    }
}

void Selector::display(float offset_x, float offset_y) const
{
    if (!selecting_shapes_.empty())
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

    for (auto &weak_shape_ptr: selected_shapes_)
    {
        if (auto shape_ptr = weak_shape_ptr.lock())
        {
            auto config = shape_ptr->getConfig();
            config.line_color[0] = 255;
            config.line_color[1] = 255;
            config.line_color[2] = 0;
            config.line_color[3] = 128;
            config.fill_color[0] = 255;
            config.fill_color[1] = 255;
            config.fill_color[2] =0;
            config.fill_color[3] = 64;
            config.line_thickness += 5.0f;
            config.bias[0] = offset_x;
            config.bias[1] = offset_y;
            shape_ptr->draw(config);
        }
    }
}

void Selector::update()
{
    std::erase_if(selected_shapes_,[](const std::weak_ptr<Shape>& ptr){return ptr.expired();});
}
}
