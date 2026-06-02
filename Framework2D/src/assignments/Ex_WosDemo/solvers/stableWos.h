//
// Created by zhdds on 2026/3/19.
//

#ifndef USTC_CG_2025_FRAMEWORK2D_STABLEWOS_H
#define USTC_CG_2025_FRAMEWORK2D_STABLEWOS_H

#include <utility>
#include <random>
#include "wos_solver.h"

namespace USTC_CG {
using Eigen::Vector2f;




class WosStableSolver2D : public WosSolver2D{

    struct SolveInfo{
        int solve_point_count;
        int walk_count;
        float walk_range_sum;
    }solveInfo_{};
protected:
    int N_ ;
    float epsilon_;
public:
    WosStableSolver2D(std::shared_ptr<Scene> shape,int n,float e)
    : WosSolver2D(std::move(shape)),N_(n),epsilon_(e)
    {}
    void resetInfo()
    {
        solveInfo_ = SolveInfo{};
    }
    void precompute(std::vector<std::pair<float, float>> poi) override;
    float solve(float, float) override;
    const SolveInfo& getInfo(){return  solveInfo_;}
};
}


#endif //USTC_CG_2025_FRAMEWORK2D_STABLEWOS_H