//
// Created by zhdds on 2026/3/20.
//

#ifndef USTC_CG_2025_FRAMEWORK2D_UTIL_H
#define USTC_CG_2025_FRAMEWORK2D_UTIL_H
#include <cmath>
#include <Eigen/Dense>
#include <iostream>
#include <random>

namespace USTC_CG {
using ::std::exp;
using ::std::sqrt;
using ::std::sin;
using ::std::cos;
using ::std::pow;
using ::std::numbers::pi;
using ::std::log;
using ::std::size_t;
constexpr float float_epsilon = 1e-9f;

template<int DIM>
consteval float unit_ball_volume() {
    float half_n =  DIM / 2.0;
    float numerator = std::pow(pi, half_n);
    float denominator = std::tgamma(half_n + 1.0);
    return numerator / denominator;
}
template<int DIM>
consteval float unit_ball_surface_area() {
    return DIM * unit_ball_volume<DIM>();
}

template<int DIM>
float Green_Func(Eigen::Vector<float,DIM> p1,Eigen::Vector<float,DIM> p2)
{
    float r = (p1 - p2).norm();
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
float Ball_Green_Func(Eigen::Vector<float,DIM> x,Eigen::Vector<float,DIM> y,Eigen::Vector<float,DIM> O,float r)
{
    if constexpr (DIM == 2)
    {
        return log((r*r - (x - O).dot(y - O))/((x - y).norm()*r)) /(2*pi);
    }
    return 0; ///TODO
}
template<int DIM>
inline Eigen::Vector2f Ball_Green_Grad(    const Eigen::Vector2f& x,
    const Eigen::Vector2f& xi,
    const Eigen::Vector2f& O,
    float R){static_assert(false); return {0,0};};

template<>
inline Eigen::Vector2f Ball_Green_Grad<2>(
    const Eigen::Vector2f& x,
    const Eigen::Vector2f& xi,
    const Eigen::Vector2f& O,
    float R)
{
    constexpr float inv_2pi =
        0.15915494309189535f;

    Eigen::Vector2f d = xi - O;

    float d2 = d.squaredNorm();

    Eigen::Vector2f xi_star =
        O + (R * R / d2) * d;

    Eigen::Vector2f a = x - xi;
    Eigen::Vector2f b = x - xi_star;

    float a2 = a.squaredNorm();
    float b2 = b.squaredNorm();

    return inv_2pi * (
        b / b2
        - a / a2
    );
}
inline uint32_t xorshift32() {
    thread_local uint32_t seed = std::random_device{}();
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
    return seed;
}

inline float randFloat() {
    return (xorshift32() >> 8) * (1.0f / (1u << 24));
}

class Counter {
    const char * name_;
    int count_ = 0;
    std::ostream& out_;
public:
    explicit Counter(const char* name,std::ostream& out = std::cout):name_(name),out_(out){}
    ~Counter()
    {
        out_<<name_<<": count = "<<count_<<"\n";
    }
    void increment()
    {
        count_++;
    }

    void operator++()
    {
        increment();
    }

    void operator++(int)
    {
        increment();
    };
};

void time_test(const char* name,auto func)
{

    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout <<"["<<name <<"]time: " << duration.count() << " ms\n";

}

#define TIME_TEST_BEGIN(name) time_test(name,[&](){

#define TIME_TEST_END });


}
#endif //USTC_CG_2025_FRAMEWORK2D_UTIL_H