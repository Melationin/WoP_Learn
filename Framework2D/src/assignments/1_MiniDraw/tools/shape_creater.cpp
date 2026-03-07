//
// Created by zhdds on 2026/3/7.
//

#include "shape_creater.h"
#include "canvas_widget.h"
#include "shapes/line.h"
#include "shapes/ellipse.h"
#include "shapes/rect.h"
#include "shapes/freehand.h"
#include "shapes/polygon.h"

namespace USTC_CG {
ShapeCreator::ShapeCreator(Canvas *canvas, ShapeType type) : shapes_(canvas->shape_list_),type_(type)
{
    this->canvas_ = canvas;
}

void ShapeCreator::display(float offset_x, float offset_y) const
{
    if (draw_status_&& current_shape_)
    {
        current_shape_->draw(offset_x,offset_y);
    }
}

void ShapeCreator::on_mouse_right_click(float x, float y)
{
    if (draw_status_&& current_shape_)
    {
        if (type_ == kPolygon )
        {
            current_shape_->add_control_point(start_point_.x,start_point_.y);
            shapes_.push_back(current_shape_);
            current_shape_.reset();
            draw_status_ = false;
        }else
        {
            current_shape_.reset();
            draw_status_ = false;
        }
    }
}

void ShapeCreator::on_mouse_move(float x, float y)
{
    if (draw_status_ && current_shape_)
    {
        end_point_ = {x,y};
        current_shape_->update(x,y);
        current_shape_->setConfig(canvas_->current_config_);
    }
}

void ShapeCreator::on_mouse_left_release(float x, float y)
{

}

void ShapeCreator::on_mouse_left_click(float x, float y)
{


    if (draw_status_)
    {
        if (type_ == kPolygon)
        {
            current_shape_->add_control_point(x,y);
        }else
        {
            shapes_.push_back(current_shape_);
            current_shape_.reset();
            draw_status_ = false;
        }
    }else
    {
        draw_status_ = true;
        start_point_ = {x,y};
        switch (type_)
        {
            case kLine:
                current_shape_ = std::make_shared<Line>(x, y, x, y);
                break;
            case kRect:
                current_shape_ = std::make_shared<Rect>(x, y, x, y);
                break;
            case kEllipse:
                current_shape_ = std::make_shared<Ellipse>(x, y, x,y);
                break;
            case kFreehand:
                current_shape_ = std::make_shared<Freehand>(x, y);
                break;
            case kPolygon:
                current_shape_ = std::make_shared<Polygon>(x, y);
                break;
        }
        current_shape_->setConfig(canvas_->current_config_);
    }
}


} // USTC_CG