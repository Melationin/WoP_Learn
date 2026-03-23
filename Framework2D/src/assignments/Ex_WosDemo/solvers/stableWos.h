//
// Created by zhdds on 2026/3/19.
//

#ifndef USTC_CG_2025_FRAMEWORK2D_STABLEWOS_H
#define USTC_CG_2025_FRAMEWORK2D_STABLEWOS_H

#include <utility>
#include <random>
#include "wos_solver.h"

namespace USTC_CG {
using Eigen::Vector2d;
class WosStableSolver2D : public WosSolver2D{



    struct SolveInfo{
        int solve_point_count;
        int walk_count;
        double walk_range_sum;
    }solveInfo_{};

protected:
    double epsilon_;
    int N_ ;
    std::minstd_rand rand_;
    std::uniform_real_distribution<double> dist_{0,1};

    double rand()
    {
        return dist_(rand_);
    }
public:
    WosStableSolver2D(std::shared_ptr<Shape> shape,int n,double e)
    : WosSolver2D(std::move(shape)),N_(n),epsilon_(e)
    {}

    void resetInfo()
    {
        solveInfo_ = SolveInfo{};
    }

    void precompute(std::vector<std::pair<double, double>> poi) override;

    double solve(double, double) override;

    const SolveInfo& getInfo(){return  solveInfo_;}

};
}

#endif //USTC_CG_2025_FRAMEWORK2D_STABLEWOS_H