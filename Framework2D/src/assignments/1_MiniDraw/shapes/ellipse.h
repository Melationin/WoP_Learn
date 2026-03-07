//
// Created by zhdds on 2026/3/3.
//

#ifndef USTC_CG_2025_FRAMEWORK2D_ELLIPSE_H
#define USTC_CG_2025_FRAMEWORK2D_ELLIPSE_H

#include <utility>
#include <shapes/shape.h>
#include <imgui/imgui.h>

namespace USTC_CG {
class Ellipse : public Shape {
public:
    // 通过其包围方形两个顶点构造
    Ellipse(
        float start_point_x,
        float start_point_y,
        float end_point_x,
        float end_point_y)
        : start_point_x_(start_point_x),
          start_point_y_(start_point_y),
          end_point_x_(end_point_x),
          end_point_y_(end_point_y)
    {
    }

    ~Ellipse() override = default;

    void draw_stroke(const Config &config) const override;

    void draw_fill(const Config &config) const override;

    void update(float x, float y) override;

    bool selected(ImVec2 mouse_p, bool can_selected_inner) const override;

    void move(float x, float y) override;

private:
    // Coordinates of the top-left and bottom-right corners of the rectangle
    float start_point_x_ = 0.0f, start_point_y_ = 0.0f;
    float end_point_x_ = 0.0f, end_point_y_ = 0.0f;

    bool isCircle_ = false;
    bool isCenter_ = false;


    std::pair<ImVec2, ImVec2> get_center_and_radius(bool is_center) const;
};
} // USTC_CG

#endif //USTC_CG_2025_FRAMEWORK2D_ELLIPSE_H
