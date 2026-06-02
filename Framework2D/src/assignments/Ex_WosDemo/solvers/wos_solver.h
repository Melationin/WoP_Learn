//
// Created by zhdds on 2026/3/19.
//

#ifndef USTC_CG_2025_FRAMEWORK2D_WOS_SOLVER_H
#define USTC_CG_2025_FRAMEWORK2D_WOS_SOLVER_H
#include "memory"
#include "functional"
#include <util.h>

#include "Scene.h"

namespace USTC_CG {

class WosSolver2D {
public:
    WosSolver2D() = default;
    explicit WosSolver2D(std::shared_ptr<Scene> shape)
        : shape(std::move(shape))
    {
    }
    virtual ~WosSolver2D() = default;
protected:
    std::shared_ptr<Scene> shape;
    bool has_source = false;
    virtual void precompute(std::vector<std::pair<float,float>> poi) = 0;
    virtual float solve(float,float) = 0;
};
} // USTC_CG

#endif //USTC_CG_2025_FRAMEWORK2D_WOS_SOLVER_H