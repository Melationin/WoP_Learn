//
// Created by zhdds on 2026/3/20.
//

#ifndef USTC_CG_2025_FRAMEWORK2D_UTIL_H
#define USTC_CG_2025_FRAMEWORK2D_UTIL_H
#include <cmath>
#include <Eigen/Dense>
namespace USTC_CG {
using ::std::exp;
using ::std::sqrt;
using ::std::sin;
using ::std::cos;
using ::std::pow;
using ::std::numbers::pi;
constexpr double float_epsilon = 1e-9f;

template<int DIM>
consteval double unit_ball_volume() {
    double half_n =  DIM / 2.0;
    double numerator = std::pow(pi, half_n);
    double denominator = std::tgamma(half_n + 1.0);
    return numerator / denominator;
}
template<int DIM>
consteval double unit_ball_surface_area() {
    return DIM * unit_ball_volume<DIM>();
}

template<int DIM>
double Green_Func(Eigen::Vector<double,DIM> p1,Eigen::Vector<double,DIM> p2)
{
    double r = (p1 - p2).norm();
    if (r < float_epsilon) return 0;
    if constexpr (DIM == 2)
    {
        return -1/(2*pi)*log(r);
    }else if constexpr (DIM == 3)
    {
        return 1/(4*pi*r);
    }else
    {
        return 1/((DIM -2)*unit_ball_surface_area<DIM-1>()*pow(r,DIM-2));
    }
}

template<int DIM>
double Ball_Green_Func(Eigen::Vector<double,DIM> x,Eigen::Vector<double,DIM> y,Eigen::Vector<double,DIM> O,double r)
{
    if constexpr (DIM == 2)
    {
        return log((r*r - (x - O).dot(y - O))/((x - y).norm()*r)) /(2*pi);
    }
    return 0; ///TODO
}
}
#endif //USTC_CG_2025_FRAMEWORK2D_UTIL_H