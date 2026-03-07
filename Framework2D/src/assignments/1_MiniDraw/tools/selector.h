//
// Created by zhdds on 2026/3/7.
//

#ifndef USTC_CG_2025_FRAMEWORK2D_SELECTOR_H
#define USTC_CG_2025_FRAMEWORK2D_SELECTOR_H

#include "tool.h"
#include <vector>
#include <shapes/shape.h>
#include <memory>

namespace USTC_CG {

//工具类最重要的模块：选择工具
//操作逻辑：不按下鼠标时，为预选阶段，可以用鼠标滚轮选择图形。按下未拖动时，为单选。拖动时为多选
class Selector :public Tool {

public:

    explicit Selector(const std::vector<std::shared_ptr<Shape>>& shapes,ShapeType type = kSelector)
    : Tool(), shapes_(shapes)
    {
        this->type_ = type;
    }


    void on_mouse_left_click(float x, float y) override;

    void on_mouse_right_click(float x, float y) override;

    void on_mouse_move(float x, float y) override;

    void display(float offset_x, float offset_y) const override;

    int get_selected_shapes_count()const{return selected_shapes_.size();}

    std::vector<std::weak_ptr<USTC_CG::Shape>> get_selected_shapes()const{return selected_shapes_;}

    //必须要清理掉失效的图形指针
    void update() override;

protected:
    std::vector<std::weak_ptr<USTC_CG::Shape>> selected_shapes_;
    const std::vector<std::shared_ptr<Shape>>& shapes_;
private:
    //选中的图形
    //预选中的图像
    std::vector<std::weak_ptr<USTC_CG::Shape>> selecting_shapes_;

    //单选模式，在预选图形中的编号
    int selecting_shapes_index_ = 0;

};
}

#endif //USTC_CG_2025_FRAMEWORK2D_SELECTOR_H