//
// Created by zhdds on 2026/3/3.
//

#ifndef USTC_CG_2025_FRAMEWORK2D_POLYGON_H
#define USTC_CG_2025_FRAMEWORK2D_POLYGON_H
#include <vector>

#include "imgui.h"
#include "shape.h"

namespace USTC_CG {
class Polygon : public Shape {
public:
    Polygon(Polygon &&polygon) = default;

    Polygon(
        float start_point_x,
        float start_point_y)
    {
        points_list_.emplace_back(start_point_x, start_point_y);
        points_list_.emplace_back(start_point_x, start_point_y);
    }

    ~Polygon() override = default;

    void draw_stroke(const Config &config) const override;


    /**
        *只能实现简单多边形的正确填充。
        */
    void draw_fill(const Config &config) const override;

    void update(float x, float y) override;

    void add_control_point(float x, float y) override;

    bool selected(ImVec2 mouse_p, bool can_selected_inner) const override;

    void move(float x, float y) override;

private:
    // Coordinates of the top-left and bottom-right corners of the rectangle
    std::vector<ImVec2> points_list_;
};
} // USTC_CG

#endif //USTC_CG_2025_FRAMEWORK2D_POLYGON_H
