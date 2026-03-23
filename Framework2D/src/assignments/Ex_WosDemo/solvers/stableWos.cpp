//
// Created by zhdds on 2026/3/19.
//

#include "stableWos.h"


#include <iostream>
#include "util.h"
namespace USTC_CG {



void WosStableSolver2D::precompute(std::vector<std::pair<double, double>> poi)
{
    rand_ = std::minstd_rand{std::random_device{}()};
}

double WosStableSolver2D::solve(double x, double y)
{

    std::uniform_real_distribution<double> dist{0,1};
    double sum = 0;
    int i{};
    int K = has_source?1:0;

    for (i = 0; i < N_; ++i)
    {
        int n =0;
        double x2 = x;
        double y2 = y;
        double source = 0;
        while (true)
        {
            auto [bx,by] = shape->distance_to_boundary(x2,y2);
            double r = (bx - x2)*(bx-x2) + (by - y2)*(by - y2);
            if (r < epsilon_*epsilon_)
            {
                sum += boundary(bx,by) - source;
                break;
            }
            r = sqrt(r);
            for (int k = 0; k < K; k++)
            {
                double source_r =std::sqrt(dist(rand_))*r;
                double source_arg = dist(rand_)*2*pi;
                source += f(source_r*cos(source_arg) + x2,source_r*sin(source_arg) + y2)*r*r/4/K;
            }
            double b_arg = dist(rand_)*2*pi;

            x2 += r*std::cos(b_arg);
            y2 += r*std::sin(b_arg);
        }
        //solveInfo_.walk_count += 1;
        //solveInfo_.walk_range_sum += n;
        //std::cout<< boundary(bx,by)<<"\n";
    }
    //solveInfo_.solve_point_count++;

    return sum/N_;
}
}
