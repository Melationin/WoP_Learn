//
// Created by zhdds on 2026/3/22.
//

#ifndef USTC_CG_2025_FRAMEWORK2D_WOP_H
#define USTC_CG_2025_FRAMEWORK2D_WOP_H
#include "HC_solver.h"


namespace USTC_CG {

struct  Probe : public Sphere<10> {
    std::vector<std::tuple<int,float,float>> neighbors;
    std::array<int,180> neighbor_i{}; // 存储每个角度范围内的邻居
    std::bitset<180> reach_b{0};

    using Sphere<10>::Sphere;

    static inline float angleDistance(float a, float b) {
        float d = std::fabs(a - b);
        return (d <= 180.0f) ? d : (360.0f - d);
    }

    template<typename Func>
    void findNeighbors(float theta,Func&& func)
    {
        for (auto [i,phi,alpha] : neighbors)
        {
            if (angleDistance(phi,theta)<=alpha )
            {
                func(i);
            }
        }
    }
};


class WoP_solver : public WosStableSolver2D{

    //行走系数
    float a_walk = 0.6;
    float a_rec = 0.9;
    int N_min = 12;

    double w_min_ = 1;

    //半径相对于epsilon的最小比例
    float k = 10;

    int K2_ = 100;

    float lambda = 4000;

    std::vector<Probe> probes_;
    RTree<int, float, 2> probes_rtree_;
    std::vector<std::tuple<float,float,float>> poi_;

    struct WalkData
    {
        float x;
        float y;
        //记录每次行走的球的索引和角度和pdf
        std::vector<std::tuple<Probe&,float,float>> walked_spheres{};
    };

    int N(float r)
    {
        return std::max(N_min,int(std::ceil(lambda * r)));
    }

    float getW(float r_norm)
    {
        r_norm = 1 - r_norm;
        return (3-2*r_norm)*r_norm*r_norm;
    }

public:


    WoP_solver(int N, float epsilon, float k_re,std::shared_ptr<Scene> shape,float lambda = 4000)
            : WosStableSolver2D( std::move(shape),N, epsilon),a_rec(k_re),lambda(lambda)
    {
        w_min_ = 1;

    }

    void precompute(std::vector<std::pair<float, float>> poi) override;

    float solve(float, float) override;

    float compute_source(float x, float y,Eigen::Vector2f O,float r)
    {

        float source = 0;
        for (int k = 0; k < K2_; k++)
        {
            const float r2 =sqrt(randFloat()) * r;
            if (r2 < float_epsilon) continue;
            const float arg = randFloat()*2*pi;
            Eigen::Vector2f v2 = shape->walkUncheck(O,r2,arg);
            source += shape->source(v2)*Ball_Green_Func<2>({x,y},v2,O,r);
        }
        return  source*r*r*pi/K2_;
    }

    void addProbes(std::vector<std::pair<float, float>> poi,std::vector<std::pair<float,float>>* poi_out = nullptr);

    void initProbes()
    {
        //poi_rtree.RemoveAll();
    }


    void loadProbe(Probe& sp);

    //返回 一次采样后的位置和角度和pdf
    std::tuple<Vector2f,float,float> sample_poisson_kernel(const Vector2f &x, const Vector2f &c, float R) {
        Vector2f d = x - c;
        float r = d.norm();

        if (r < float_epsilon)
        {
            float theta = 2 * pi *randFloat();
            Vector2f z = c + R * Vector2f(std::cos(theta), std::sin(theta));
            float pdf = 1.0 / (2 * pi);
            return {z, theta, pdf};
        }


        const float rho = r / R;
        const float phi = std::atan2(d.y(), d.x());

        const float u = randFloat();

        const float t = std::tan(pi * (u - 0.5));
        const float factor = (1 - rho) / (1 + rho);
        float theta = phi + 2 * std::atan(factor * t);
        Vector2f z = c + R * Vector2f(std::cos(theta), std::sin(theta));
        float denom = 1 + rho * rho - 2 * rho * std::cos(theta - phi);
        float pdf = (1 - rho * rho) / (2 * pi * denom);
        return {z, theta, pdf};
    }
};
} // USTC_CG

#endif //USTC_CG_2025_FRAMEWORK2D_WOP_H