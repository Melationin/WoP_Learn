//
// Created by zhdds on 2026/3/19.
//

#ifndef USTC_CG_2025_FRAMEWORK2D_WOS_SOLVER_H
#define USTC_CG_2025_FRAMEWORK2D_WOS_SOLVER_H
#include "../shapes/shape.h"
#include "memory"
#include "functional"
#include <util.h>

namespace USTC_CG {
using BoundaryConditionFunc = std::function<double(double,double)>;
using SourceFunc = std::function<double(double,double)>;
constexpr auto ZERO_SOURCE = [](double, double) { return 0.0; };
class WosSolver2D {


public:
    WosSolver2D() = default;
    WosSolver2D(std::shared_ptr<Shape> shape)
        : shape(std::move(shape))
    {
    }

    void set_boundary_condition(BoundaryConditionFunc boundary_func)
    {
        boundary = std::move(boundary_func);
    }

    void set_source_function(SourceFunc source_func)
    {
        has_source = true;
        f = std::move(source_func);
    }

    virtual ~WosSolver2D() = default;

    std::shared_ptr<Shape> shape;

    BoundaryConditionFunc  boundary;
    SourceFunc f = ZERO_SOURCE;

    bool has_source = false;
    virtual void precompute(std::vector<std::pair<double,double>> poi) = 0;
    virtual double solve(double,double) = 0;

};
} // USTC_CG

#endif //USTC_CG_2025_FRAMEWORK2D_WOS_SOLVER_H