//
// Created by zhdds on 2026/3/22.
//

#ifndef USTC_CG_2025_FRAMEWORK2D_WOP_H
#define USTC_CG_2025_FRAMEWORK2D_WOP_H
#include "HC_solver.h"


namespace USTC_CG {
class WoP_solver : public HC_solver{

    //行走系数
    double a_walk = 0.7;

    struct WalkData
    {
        double x;
        double y;
        //记录每次行走的球的索引和角度和pdf
        std::vector<std::tuple<int,double,double>> walked_spheres{};


    };

public:


    WoP_solver(int N, double epsilon, double k_re,std::shared_ptr<Shape> shape)
            : HC_solver(N, epsilon, k_re, std::move(shape))
    {
        w_min_ = 1;

    }

    void precompute(std::vector<std::pair<double, double>> poi) override;

    int getSize()
    {
        return spheres_.size();
    }

    double solve(double, double) override;

    void computeProbes(std::vector<std::pair<double, double>> poi);

    //返回 一次采样后的位置和角度和pdf
    std::tuple<Vector2d,double,double> sample_poisson_kernel(Vector2d x, Vector2d c, float R) {
        Vector2d d = x - c;
        double r = d.norm();

        if (r < float_epsilon)
        {
            double theta = 2 * pi * dist_(rand_);
            Vector2d z = c + R * Vector2d(std::cos(theta), std::sin(theta));
            double pdf = 1.0 / (2 * pi);
            return {z, theta, pdf};
        }


        double rho = r / R;
        double phi = std::atan2(d.y(), d.x());

        double u = dist_(rand_);

        double t = std::tan(pi * (u - 0.5));
        double factor = (1 - rho) / (1 + rho);

        double theta = phi + 2 * std::atan(factor * t);


        Vector2d z = c + R * Vector2d(std::cos(theta), std::sin(theta));


        double denom = 1 + rho * rho - 2 * rho * std::cos(theta - phi);
        double pdf = (1 - rho * rho) / (2 * pi * denom);

        return {z, theta, pdf};
    }
};
} // USTC_CG

#endif //USTC_CG_2025_FRAMEWORK2D_WOP_H