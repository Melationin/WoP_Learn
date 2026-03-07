//
// Created by zhdds on 2026/3/7.
//

#include "shape_mover.h"

namespace USTC_CG {
void ShapeMover::on_mouse_left_click(float x, float y)
{
    start_pos_ = {x,y};
    end_pos_ = {x,y};
    last_mouse_pos = {x,y};
    start_move_ = true;
}
void ShapeMover::on_mouse_left_release(float x, float y)
{
    start_move_ = false;

}

void ShapeMover::on_mouse_right_click(float x, float y)
{
    float dx = end_pos_.x - start_pos_.x;
    float dy = end_pos_.y - start_pos_.y;
    for (const auto &weak_shape_ptr: selected_shapes_)
    {
        if (auto shape_ptr = weak_shape_ptr.lock())
        {
            shape_ptr->move(-dx, -dy);
        }
    }
    start_move_ = false;
}

void ShapeMover::on_mouse_move(float x, float y)
{

    if (ImGui::IsKeyDown(ImGuiKey_MouseLeft))
    {
        if (!start_move_ ) return;
        end_pos_ = {x,y};
        float dx = x - last_mouse_pos.x;
        float dy = y - last_mouse_pos.y;

        if (dx !=0 || dy != 0)
        {
            for (const auto &weak_shape_ptr: selected_shapes_)
            {
                if (auto shape_ptr = weak_shape_ptr.lock())
                {
                    shape_ptr->move(dx, dy);
                }
            }
            last_mouse_pos = {x,y};
        }
    }else
    {
        start_move_ = false;
    }
}
} // USTC_CG