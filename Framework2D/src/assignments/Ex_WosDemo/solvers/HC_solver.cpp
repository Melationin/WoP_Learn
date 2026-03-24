//
// Created by zhdds on 2026/3/20.
//

#include "HC_solver.h"

namespace USTC_CG {
static Counter search_count{"search_HC"};
void HC_solver::precompute(std::vector<std::pair<double, double>> poi)
{
}

double HC_solver::solve(double x, double y)
{
    std::vector<int> results;
    search_count++;
    rtree_.search({x,y},results,a_rec_);

    double w_sum  = 0;

    for (auto i:  results)
    {
        Sphere<10>& it = spheres_[i];
        double r = it.r;
        double r_norm = (Eigen::Vector2d(x,y) - it.pos ).norm();
        w_sum += getW(r_norm/r);
    }
    if (w_sum < w_min_)
    {
        auto [bx,by]  = shape->distance_to_boundary(x,y);
        double r = sqrt((bx - x)*(bx - x) + (by - y)*(by - y));
        spheres_.emplace_back(x,y,r);
        Sphere<10>& now_sphere = spheres_[spheres_.size()-1];
        rtree_.insert(spheres_.size()-1);
        int n = N(r);
        double d_theta = 2*pi / n;
        double theta = 0;
        for (int i = 0; i < n; i++)
        {
            double sample = WosStableSolver2D::solve(x + r*cos(theta) , y + r*sin(theta));
            auto& sp = now_sphere;
            sp.w_sum += 1;
            sp.n++;
            sp.fourier_coefficients[0].first += sample;
            const double a_0 = sp.fourier_coefficients[0].first / sp.w_sum;
            const double c1 = std::cos(theta);
            const double s1 = std::sin(theta);

            double cn = c1 ;
            double sn = s1 ;

            for (int l = 1; l <= 10; l++)
            {
                sp.fourier_coefficients[l].first +=  (sample) * cn;
                sp.fourier_coefficients[l].second +=  (sample) * sn;
                double cn_1  =  cn * c1 - sn * s1;
                double sn_1 = cn * s1 + sn * c1;
                cn = cn_1;
                sn = sn_1;
            }
            theta += d_theta;
        }

        for (auto& [ a, b]: now_sphere.fourier_coefficients)
        {
            a /= now_sphere.w_sum;
            b /= now_sphere.w_sum;
        }
       // now_sphere.get_fourier_coefficients();
        results.push_back(spheres_.size()-1);
        w_sum += getW(0);
    }
    double res = 0;
    double source = 0;
    for (auto i:  results)
    {
        Sphere<10>& it = spheres_[i];
        double r = it.r;
        Eigen::Vector2d dv = Eigen::Vector2d(x,y) - it.pos ;
        double r_norm = dv.norm();
        double weight= getW(r_norm/r);
        res += weight * it.get_value(atan2(dv.y(),dv.x()),r_norm);
        if (has_source)
        {
            source += weight * compute_source(x,y,it.pos,r);
        }
    }
    res /= w_sum;
    source /= w_sum;
    return res - source;
}

double HC_solver::compute_source(double x, double y,Eigen::Vector2d O,double r)
{

    double source = 0;
    std::uniform_real_distribution<double> dist{0,1};
    for (int k = 0; k < K2_; k++)
    {
        double r2 =sqrt(dist(rand_)) * r;
        if (r2 < float_epsilon) continue;
        double arg = dist(rand_)*2*pi;
        Eigen::Vector2d v2 =O + Eigen::Vector2d{r2*cos(arg) ,r2*sin(arg) };

        source += f(v2.x(),v2.y())*Ball_Green_Func<2>({x,y},v2,O,r);
    }
    return  source*r*r*pi/K2_;
}
} // USTC_CG