//
// Created by zhdds on 2026/3/7.
//

#ifndef USTC_CG_2025_FRAMEWORK2D_TOOL_H
#define USTC_CG_2025_FRAMEWORK2D_TOOL_H
#include <shapeType.h>
namespace USTC_CG
{


class  Tool {
public:
    virtual ~Tool() = default;



    void virtual display(float offset_x,float offset_y) const
    {};

    void virtual on_mouse_left_click(float x, float y)
    {};

    void virtual on_mouse_right_click(float x, float y)
    {};

    void virtual on_mouse_move(float x, float y)
    {};

    void virtual on_mouse_right_release(float x, float y)
    {};

    void virtual on_mouse_left_release(float x, float y)
    {};

    void virtual on_mouse_scroll(float delta)
    {};



};
}

#endif //USTC_CG_2025_FRAMEWORK2D_TOOL_H