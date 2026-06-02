//
// Created by zhdds on 2026/3/19.
//



#include <iostream>
#include "util.h"
#include  "stableWos.h"
namespace USTC_CG {

void WosStableSolver2D::precompute(std::vector<std::pair<float, float>>)
{

}

float WosStableSolver2D::solve(float x, float y)
{

    float sum = 0;
    int i{};
    int K = has_source?1:0;

    for (i = 0; i < N_; ++i)
    {
        Vector2f walk_point = {x,y};
        float source = 0;

        while (true)
        {
            //distance_to_boundary_WoS++;
            auto [r,d] = shape->distance_to_boundaryAndDC(walk_point);
            if (r < epsilon_)
            {
                sum += d - source;
                break;
            }
            for (int k = 0; k < K; k++)
            {
                float source_r =std::sqrt(randFloat())*r;
                float source_arg = randFloat()*2*pi;
                const Eigen::Vector2f source_p = shape->walkUncheck(walk_point,source_arg,source_r);
                if (source_r > 1e-6f) source += shape->source(source_p) *((r * r / 2.0f) * std::log(r / source_r))/K;
                //source += f(source_r*cos(source_arg) + x2,source_r*sin(source_arg) + y2)*r*r/4/K;
            }
            float b_arg = randFloat()*2*pi;
            walk_point = shape->walkUncheck(  walk_point,b_arg,r);
        }
    }
    //solveInfo_.solve_point_count++;

    //std::cout<<sum/N_<<" ";
    return sum/N_;
}


}
