//
// Created by zhdds on 2026/3/7.
//

#ifndef USTC_CG_2025_FRAMEWORK2D_SHAPE_CREATER_H
#define USTC_CG_2025_FRAMEWORK2D_SHAPE_CREATER_H


#include <tools/tool.h>
#include <vector>
#include <memory>


#include "shapes/shape.h"


namespace USTC_CG {
class Canvas;
//将创建图形的过程抽象出来，使用Shape_creator 建立图形
class ShapeCreator :public Tool  {

public:

    explicit ShapeCreator(Canvas* canvas,ShapeType type);


    void on_mouse_right_click(float x, float y) override;
    void on_mouse_move(float x, float y) override;
    void on_mouse_left_release(float x, float y) override;
    void on_mouse_left_click(float x, float y) override;
    void display(float offset_x, float offset_y) const override;


private:

    std::shared_ptr<Shape> current_shape_;

    std::vector<std::shared_ptr<Shape>>& shapes_;

    ImVec2 start_point_, end_point_;
    bool draw_status_ = false;



    Canvas* canvas_ ;
};
} // USTC_CG

#endif //USTC_CG_2025_FRAMEWORK2D_SHAPE_CREATER_H