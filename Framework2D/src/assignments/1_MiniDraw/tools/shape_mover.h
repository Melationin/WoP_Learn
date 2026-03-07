//
// Created by zhdds on 2026/3/7.
//

#ifndef USTC_CG_2025_FRAMEWORK2D_SHAPE_MOVER_H
#define USTC_CG_2025_FRAMEWORK2D_SHAPE_MOVER_H
#include "selector.h"
namespace USTC_CG {

//设定为选择工具的子类，是为了图形移动，图形外观设置等工具的自由切换。重点在于get_selected_shapes()
//另一种方法是定义一个只有get_selected_shapes()的接口类，但有点复杂了。这里就直接定义子类
class ShapeMover :public Selector{
public:
    explicit ShapeMover(const Selector& selector,ShapeType type = kShapeConfigSetter)
         : Selector(selector)
    {
        type_ = type;
    }
    void on_mouse_left_click(float x, float y) override;

    void on_mouse_left_release(float x, float y) override;

    void on_mouse_right_click(float x, float y) override;

    void on_mouse_move(float x, float y) override;
private:
    ImVec2 start_pos_;
    ImVec2 last_mouse_pos;
    ImVec2 end_pos_;

    bool start_move_ = false;
};
} // USTC_CG

#endif //USTC_CG_2025_FRAMEWORK2D_SHAPE_MOVER_H