//
// Created by zhdds on 2026/3/3.
//

#include "ellipse.h"
#include <imgui.h>
#include <cmath>

namespace USTC_CG {
void Ellipse::draw_stroke(const Config &config) const
{
    auto drawList = ImGui::GetWindowDrawList();
    auto [center,radius] = get_center_and_radius(isCenter_);
    center.x += config.bias[0];
    center.y += config.bias[1];

    drawList->AddEllipse(
        center,
        radius,
        IM_COL32(
            config.line_color[0],
            config.line_color[1],
            config.line_color[2],
            config.line_color[3]),
        0.0f,
        100,
        config.line_thickness
    );
}

void Ellipse::draw_fill(const Config &config) const
{
    auto drawList = ImGui::GetWindowDrawList();
    auto [center,radius] = get_center_and_radius(isCenter_);
    center.x += config.bias[0];
    center.y += config.bias[1];

    drawList->AddEllipseFilled(
        center,
        radius,
        IM_COL32(
            config.fill_color[0],
            config.fill_color[1],
            config.fill_color[2],
            config.fill_color[3]),
        0.0f,
        100
    );
}


void Ellipse::update(float x, float y)
{
    isCircle_ = ImGui::IsKeyDown(ImGuiKey_LeftShift);
    isCenter_ = ImGui::IsKeyDown(ImGuiKey_LeftAlt);

    if (isCircle_)
    {
        float radius = std::max(std::abs(x - start_point_x_), std::abs(y - start_point_y_));
        end_point_x_ = start_point_x_ + (x > start_point_x_ ? radius : -radius);
        end_point_y_ = start_point_y_ + (y > start_point_y_ ? radius : -radius);
    } else
    {
        end_point_x_ = x;
        end_point_y_ = y;
    }
}

bool Ellipse::selected(ImVec2 mouse_p, bool can_selected_inner) const
{
    auto [center,radius] = get_center_and_radius(isCenter_);
    float dx = mouse_p.x - center.x;
    float dy = mouse_p.y - center.y;
    if (can_selected_inner)
    {
        return (dx * dx) / (radius.x * radius.x) + (dy * dy) / (radius.y * radius.y) <= 1.0f;
    } else
    {
        //基于梯度的椭圆边界计算
        //思路来自ai
        float a = radius.x;
        float b = radius.y;
        float a2 = a * a;
        float b2 = b * b;

        float f = (dx * dx) / a2 + (dy * dy) / b2 - 1.0f;

        float grad_x = 2.0f * dx / a2;
        float grad_y = 2.0f * dy / b2;
        float grad_len_sq = grad_x * grad_x + grad_y * grad_y;

        if (grad_len_sq < 1e-6f)
            return false;

        float dist = std::fabs(f) / std::sqrt(grad_len_sq);
        return dist <= SELECTION_RADIUS;
    }
}

void Ellipse::move(float x, float y)
{
    start_point_x_ += x;
    start_point_y_ += y;
    end_point_x_ += x;
    end_point_y_ += y;
}

std::pair<ImVec2, ImVec2> Ellipse::get_center_and_radius(bool is_center) const
{
    ImVec2 center;
    ImVec2 radius;
    if (is_center)
    {
        center.x = (start_point_x_);
        center.y = (start_point_y_);
        radius.x = std::abs(start_point_x_ - end_point_x_);
        radius.y = std::abs(start_point_y_ - end_point_y_);
    } else
    {
        center.x = (start_point_x_ + end_point_x_) / 2;
        center.y = (start_point_y_ + end_point_y_) / 2;
        radius.x = std::abs(start_point_x_ - end_point_x_) / 2;
        radius.y = std::abs(start_point_y_ - end_point_y_) / 2;
    }
    return {center, radius};
}
} // USTC_CG
