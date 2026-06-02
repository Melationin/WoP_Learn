//
// Created by zhdds on 2026/5/26.
//

#ifndef USTC_CG_2025_FRAMEWORK2D_LINE_H
#define USTC_CG_2025_FRAMEWORK2D_LINE_H

#include "shape.h"
class line: public USTC_CG::Shape {


public:
    line()= default;

    void add_control_point(float x, float y) override
    {

    }

    // 这里简单设为更新最后一个点；如果没有点则新增
    void update(float x, float y) override
    {

    }

    // 将逻辑坐标 [-2,2]^2 映射到屏幕像素坐标 [0,w-1] x [0,h-1]
    std::pair<int, int> getCoord(float x, float y, int w, int h) override
    {
        int px = static_cast<int>((x + 2.0f) / 4.0f * (w - 1));
        int py = static_cast<int>((2.0f - y) / 4.0f * (h - 1));  // y轴翻转
        return { px, py };
    }

    // 将屏幕像素坐标还原到逻辑坐标 [-2,2]^2
    std::pair<float, float> getPos(int x, int y, int w, int h) override
    {
        float fx = -2.0f + 4.0f * static_cast<float>(x) / static_cast<float>(w - 1);
        float fy =  2.0f - 4.0f * static_cast<float>(y) / static_cast<float>(h - 1);
        return { fx, fy };
    }

    // 射线法判断点是否在多边形内部
    bool interior(float x, float y) override
    {
        return y>-1 && y<1;
    }

    // 返回点(x,y)到多边形边界最近的边界点
    std::pair<float, float> distance_to_boundary(float x, float y) override
    {
        if ( y > 0) return {x,1};
        else return {x,-1};
       // return std::min(abs(y-1),std::abs(y+1))
    }

};


#endif //USTC_CG_2025_FRAMEWORK2D_LINE_H