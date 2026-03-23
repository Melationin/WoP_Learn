//
// Created by zhdds on 2026/3/20.
//

#ifndef USTC_CG_2025_FRAMEWORK2D_RECT_H
#define USTC_CG_2025_FRAMEWORK2D_RECT_H

#include <shapes/shape.h>
namespace USTC_CG {
class Rect : public Shape{

    double x1,x2,y1,y2;

public:

    Rect(double x1,double x2,double y1,double y2):x1(x1),x2(x2),y1(y1),y2(y2)
    {
        if (x1 > x2) std::swap(x1,x2);
        if (y1 > y2) std::swap(y1,y2);
    }

    std::pair<int,int> getCoord(double x, double y, int w, int h) override
    {
        int rx = w*(x-x1)/(x2-x1) ;
        int ry = h*(y - x1)/(y2-y1) ;
        return {rx,ry};
    };
    std::pair<double,double> getPos(int x, int y, int w, int h) override
    {
        double rx = x1 + (x2-x1)*x/w ;
        double ry = y1 + (y2-y1)*y/h ;
        return {rx,ry};
    };
    void draw(const Config &config) const override{};

    void update(float x, float y) override{};

    std::vector<std::pair<int, int>> get_interior_pixels() const override{return {};};

    std::pair<double, double> distance_to_boundary(double x, double y) override;
};
} // USTC_Cg

#endif //USTC_CG_2025_FRAMEWORK2D_RECT_H