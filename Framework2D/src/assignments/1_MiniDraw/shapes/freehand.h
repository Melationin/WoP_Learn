//
// Created by zhdds on 2026/3/3.
//

#ifndef USTC_CG_2025_FRAMEWORK2D_FREEHAND_H
#define USTC_CG_2025_FRAMEWORK2D_FREEHAND_H
#include "shapes/shape.h"
#include <vector>
#include "imgui.h"

namespace USTC_CG {

    class Freehand : public Shape {
    public:
        Freehand(
            float start_point_x,
            float start_point_y)
        {
            points_list_.emplace_back(start_point_x, start_point_y);
        }

        void draw_stroke(const Config &config) const override;

        void update(float x, float y) override;

        void add_control_point(float x, float y) override;

        bool selected(ImVec2 mouse_p, bool can_selected_inner) const override;

    private:
        std::vector<ImVec2> points_list_;
    };
} // USTC_GC

#endif //USTC_CG_2025_FRAMEWORK2D_FREEHAND_H