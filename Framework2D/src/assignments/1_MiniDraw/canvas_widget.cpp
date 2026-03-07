#include "canvas_widget.h"
#include "tools/shape_mover.h"
#include "tools/shape_setter.h"

#include <cmath>
#include <iostream>
#include <memory>
#include "imgui.h"


namespace USTC_CG {
void Canvas::draw()
{
    draw_background();
    // HW1_TODO: more interaction events
    if (is_hovered_ && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        mouse_click_event();
    if (is_hovered_ && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        mouse_right_click_event();
    if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
        mouse_release_event();
    mouse_move_event();
    if (current_tool_)    current_tool_->update();

    draw_shapes();
}

void Canvas::set_attributes(const ImVec2 &min, const ImVec2 &size)
{
    canvas_min_ = min;
    canvas_size_ = size;
    canvas_minimal_size_ = size;
    canvas_max_ =
            ImVec2(canvas_min_.x + canvas_size_.x, canvas_min_.y + canvas_size_.y);
}

void Canvas::show_background(bool flag)
{
    show_background_ = flag;
}

void Canvas::set_line_color(const ImVec4 &color)
{
    current_config_.line_color[0] = static_cast<unsigned char>(color.x * 255);
    current_config_.line_color[1] = static_cast<unsigned char>(color.y * 255);
    current_config_.line_color[2] = static_cast<unsigned char>(color.z * 255);
    current_config_.line_color[3] = static_cast<unsigned char>(color.w * 255);
}

void Canvas::set_line_thickness(float thickness)
{
    current_config_.line_thickness = thickness;
}

void Canvas::set_fill_mode(bool fill_shape)
{
    current_config_.fill_shape = fill_shape;
}

void Canvas::set_fill_color(const ImVec4 &color)
{
    current_config_.fill_color[0] = static_cast<unsigned char>(color.x * 255);
    current_config_.fill_color[1] = static_cast<unsigned char>(color.y * 255);
    current_config_.fill_color[2] = static_cast<unsigned char>(color.z * 255);
    current_config_.fill_color[3] = static_cast<unsigned char>(color.w * 255);
}

bool Canvas::hasShapeSelected() const
{
    if (!current_tool_)return false;
    if (!isSelector(current_tool_->type_)) return false;
    auto* selector = dynamic_cast<Selector*>(current_tool_.get());
    if (selector->get_selected_shapes_count() >0 )
    {
        return true;
    }
    return false;
}

void Canvas::set_default()
{
    current_tool_.reset();
}

void Canvas::set_line()
{
    current_tool_ = std::make_unique<ShapeCreator>(this,kLine);
}

void Canvas::set_rect()
{
    current_tool_ = std::make_unique<ShapeCreator>(this,kRect);
}

void Canvas::set_ellipse()
{
    current_tool_ = std::make_unique<ShapeCreator>(this,kEllipse);
}

void Canvas::set_polygon()
{
    current_tool_ = std::make_unique<ShapeCreator>(this,kPolygon);
}

void Canvas::set_freehand()
{
    current_tool_ = std::make_unique<ShapeCreator>(this,kFreehand);
}

void Canvas::set_selector()
{
    current_tool_ = std::make_unique<Selector>(this->shape_list_);
}

void Canvas::set_shape_setter()
{
    if (current_tool_)
    {
        if (isSelector(current_tool_->type_))
        {
            auto* temp = dynamic_cast<Selector*>(&(*current_tool_));
            auto* setter = new ShapeSetter(*temp,current_config_);
            current_tool_.reset(setter);
        }
    }

}

void Canvas::set_shape_mover()
{
    if (current_tool_)
    {
        if (isSelector(current_tool_->type_))
        {
            auto* temp = dynamic_cast<Selector*>(&(*current_tool_));
            auto* setter = new ShapeMover(*temp);
            current_tool_.reset(setter);
        }
    }
}

void Canvas::remove_selected()
{
    if (current_tool_)
    {
        if (isSelector(current_tool_->type_))
        {
            auto* temp = dynamic_cast<Selector*>(&(*current_tool_));
            auto selected_shapes = temp->get_selected_shapes();
            for (auto &weak_shape_ptr: selected_shapes)
            {
                if (auto shape_ptr = weak_shape_ptr.lock())
                {
                    shape_ptr->removed();
                }
            }
        }
    }
}


// HW1_TODO: more shape types, implements

void Canvas::clear_shape_list()
{
    shape_list_.clear();
}

void Canvas::draw_background()
{
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    if (show_background_)
    {
        // Draw background recrangle
        draw_list->AddRectFilled(canvas_min_, canvas_max_, background_color_);
        // Draw background border
        draw_list->AddRect(canvas_min_, canvas_max_, border_color_);
    }
    /// Invisible button over the canvas to capture mouse interactions.
    ImGui::SetCursorScreenPos(canvas_min_);
    ImGui::InvisibleButton(
        label_.c_str(), canvas_size_, ImGuiButtonFlags_MouseButtonLeft);
    // Record the current status of the invisible button
    is_hovered_ = ImGui::IsItemHovered();
    is_active_ = ImGui::IsItemActive();
}

void Canvas::draw_shapes()
{
    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    //删除被绘制的
    std::erase_if(shape_list_, [](const auto &shape_ptr) { return shape_ptr->isRemoved(); });

    // ClipRect can hide the drawing content outside of the rectangular area
    draw_list->PushClipRect(canvas_min_, canvas_max_, true);
    for (const auto &shape: shape_list_)
    {
        shape->draw(canvas_min_.x, canvas_min_.y);
    }

    if (current_tool_)
    {
        current_tool_->display(canvas_min_.x, canvas_min_.y);
    }
    draw_list->PopClipRect();
}

void Canvas::mouse_click_event()
{
    // HW1_TODO: Drawing rule for more primitives
    if (current_tool_)
    {
        auto [x,y] = mouse_pos_in_canvas();
        current_tool_->on_mouse_left_click(x,y);
    }
}

void Canvas::mouse_right_click_event()
{
    if (current_tool_)
    {
        auto [x,y] = mouse_pos_in_canvas();
        current_tool_->on_mouse_right_click(x,y);
    }
}

void Canvas::mouse_move_event()
{
    // HW1_TODO: Drawing rule for more primitives
    if (current_tool_)
    {
        auto [x,y] = mouse_pos_in_canvas();
        current_tool_->on_mouse_move(x,y);
    }
}

void Canvas::mouse_release_event()
{
    if (current_tool_)
    {
        auto [x,y] = mouse_pos_in_canvas();
        current_tool_->on_mouse_left_release(x,y);
    }
}

ImVec2 Canvas::mouse_pos_in_canvas() const
{
    ImGuiIO &io = ImGui::GetIO();
    const ImVec2 mouse_pos_in_canvas(
        io.MousePos.x - canvas_min_.x, io.MousePos.y - canvas_min_.y);
    return mouse_pos_in_canvas;
}
} // namespace USTC_CG
