//
// Created by zhdds on 2026/3/7.
//

#ifndef USTC_CG_2025_FRAMEWORK2D_SELECTOR_H
#define USTC_CG_2025_FRAMEWORK2D_SELECTOR_H

#include "tool.h"
#include <vector>
#include <shapes/shape.h>
#include <memory>

#include "imgui.h"

namespace USTC_CG {

//工具类最重要的模块：选择工具
//操作逻辑：不按下鼠标时，为预选阶段，可以用鼠标滚轮选择图形。按下未拖动时，为单选。拖动时为多选
class Selector :public Tool {

public:

    explicit Selector(const std::vector<std::shared_ptr<Shape>>& shapes) : Tool(), shapes_(shapes)
    {
    }

    void on_mouse_left_click(float x, float y) override;

    void on_mouse_left_release(float x, float y) override;

    void on_mouse_move(float x, float y) override;

    void display(float offset_x, float offset_y) const override;

private:
    //选中的图形
    std::vector<std::weak_ptr<USTC_CG::Shape>> selected_shapes_;

    //预选中的图像
    std::vector<std::weak_ptr<USTC_CG::Shape>> selecting_shapes_;


    //单选模式，在预选图形中的编号
    int selecting_shapes_index_ = 0;

    ImVec2 begin_point_;

    ImVec2 end_point_;

    //来自Canvas的图像列表。创建时需要
    const std::vector<std::shared_ptr<Shape>>& shapes_;



};
}

#endif //USTC_CG_2025_FRAMEWORK2D_SELECTOR_H