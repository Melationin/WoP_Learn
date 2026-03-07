#pragma once
#include "imgui.h"

namespace USTC_CG
{

constexpr float SELECTION_RADIUS = 10.0f;
class Shape
{
   public:
    // Draw Settings
    struct Config
    {
        // Offset to convert canvas position to screen position
        float bias[2] = { 0.f, 0.f };
        // Line color in RGBA format
        unsigned char line_color[4] = { 255, 0, 0, 255 };
        float line_thickness = 2.0f;
        bool fill_shape = false;
        unsigned char fill_color[4] = { 255, 0, 0, 255 };
    };

   public:
    virtual ~Shape() = default;

    /**
     * Draws the shape on the screen.
     * This is a pure virtual function that must be implemented by all derived
     * classes.
     *
     * @param config The configuration settings for drawing, including line
     * color, thickness, and bias.
     *               - line_color defines the color of the shape's outline.
     *               - line_thickness determines how thick the outline will be.
     *               - bias is used to adjust the shape's position on the
     * screen.
     */
    virtual void draw(const Config& config)
    {
        if (config.fill_shape)
        {
            draw_fill(config);
        }
        draw_stroke(config);
    }


    /**
   * 在有配置的情况下绘制形状。
   * 只需要提供偏移量，其他配置项将使用图形配置的值。
    * @param offset_x, offset_y 偏移量，用于调整形状在屏幕上的位置。
   */
    void draw(float offset_x,float offset_y)
    {
        config_.bias[0] = offset_x;
        config_.bias[1] = offset_y;
        draw(config_);
    }
    /**
     * Updates the state of the shape.
     * This function allows for dynamic modification of the shape, in response
     * to user interactions like dragging.
     *
     * @param x, y Dragging point. e.g. end point of a line.
     */
    virtual void update(float x, float y) = 0;
    /**
     * Adds a control point to the shape.
     * This function is used to add control points to the shape, which can be
     * used to modify the shape's appearance.
     *
     * @param x, y Control point to be added. e.g. vertex of a polygon.
     */
    virtual void add_control_point(float x, float y) {}


    //单点选择。can_selected_inner为是否可以通过内部选择
    virtual bool selected(ImVec2 mouse_p,bool can_selected_inner ) const{return  false;}

    bool selected(ImVec2 mouse_p) const{return selected(mouse_p,config_.fill_shape);}


    //框选
    virtual bool  selected(ImVec2 mouse_p,ImVec2 mouse_p2) const{return  false;}

    void setConfig(const Config& config)
    {
        config_ = config;
    }

    const Config& getConfig() const
    {
        return config_;
    }
private:

    /**
       *使用参数配置绘制填充形状。
       *
       *不重写表示无法定义填充
       *
       * @param config  使用这个配置绘制填充形状，包括填充颜色和偏移量。
       */
    virtual void draw_fill(const Config& config) const
    {
    }


    /**
       *使用参数配置绘制轮廓形状。
       *
       *必须重写以定义轮廓绘制
       * @param config 使用配置绘制填充形状
       */
    virtual void draw_stroke(const Config& config) const = 0;


    Config config_{};
};
}  // namespace USTC_CG