#pragma once

#include <imgui.h>

#include <memory>
#include <vector>

#include "shapes/shape.h"
#include "shapeType.h"
#include "tools/selector.h"
#include "tools/shape_creater.h"
#include "common/widget.h"



namespace USTC_CG
{

// Canvas class for drawing shapes.
class Canvas : public Widget
{
   public:
    // Inherits constructor from Component.
    using Widget::Widget;

    // Override the draw method from the parent Component class.
    void draw() override;

    // Enumeration for supported shape types.


    // Shape type setters.
    void set_default();
    void set_line();
    void set_rect();
    void set_ellipse();
    void set_polygon();
    void set_freehand();
    void set_selector();
    // HW1_TODO: more shape types.

    // Clears all shapes from the canvas.
    void clear_shape_list();

    // Set canvas attributes (position and size).
    void set_attributes(const ImVec2& min, const ImVec2& size);

    // Controls the visibility of the canvas background.
    void show_background(bool flag);


    void set_line_color(const ImVec4& color);

    void set_line_thickness(float thickness);

    void set_fill_mode(bool fill_shape);

    void set_fill_color(const ImVec4 &color);


private:
    // Drawing functions.
    void draw_background();
    void draw_shapes();

    // Event handlers for mouse interactions.
    void mouse_click_event();
    void mouse_right_click_event();

    void mouse_move_event();
    void mouse_release_event();

    void mouse_scroll_event();

    // Calculates mouse's relative position in the canvas.
    ImVec2 mouse_pos_in_canvas() const;

    // Canvas attributes.
    ImVec2 canvas_min_;         // Top-left corner of the canvas.
    ImVec2 canvas_max_;         // Bottom-right corner of the canvas.
    ImVec2 canvas_size_;        // Size of the canvas.

    ImVec2 canvas_minimal_size_ = ImVec2(50.f, 50.f);
    ImU32 background_color_ = IM_COL32(50, 50, 50, 255);
    ImU32 border_color_ = IM_COL32(255, 255, 255, 255);

    Shape::Config current_config_;

    bool show_background_ = true;  // Controls background visibility.

    // Mouse interaction status.
    bool is_hovered_, is_active_;

    // Current shape being drawn.
    ShapeType shape_type_;


    std::unique_ptr<Tool> current_tool_;

    // List of shapes drawn on the canvas.
    std::vector<std::shared_ptr<Shape>> shape_list_;


//frined class
    friend Selector;
    friend ShapeCreator;
};

}  // namespace USTC_CG
