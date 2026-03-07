//
// Created by zhdds on 2026/3/7.
//

#ifndef USTC_CG_2025_FRAMEWORK2D_SHAPE_SETTER_H
#define USTC_CG_2025_FRAMEWORK2D_SHAPE_SETTER_H
#include "selector.h"
namespace USTC_CG {

//同 ShapeMover
class ShapeSetter : public Selector{
public:
    explicit ShapeSetter(const Selector& selector,Shape::Config& config,ShapeType type = kShapeConfigSetter)
        : Selector(selector), config_(config)
    {
        type_ = type;
    }

    void on_mouse_left_click(float, float) override{};

    void on_mouse_left_release(float, float) override{};

    void on_mouse_move(float, float) override{};

    void update() override;

private:
    Shape::Config& config_;
};

} // USTC_CG

#endif //USTC_CG_2025_FRAMEWORK2D_SHAPE_SETTER_H