//
// Created by zhddsj on 2026/3/20.
//

#ifndef USTC_CG_2025_FRAMEWORK2D_HC_SOLVER_H
#define USTC_CG_2025_FRAMEWORK2D_HC_SOLVER_H
#include <bitset>
#include <iostream>

#include "stableWos.h"

#include <Eigen/Dense>
#include <RTree.h>

namespace USTC_CG {

// L为展开项数
template<int L>
struct Sphere {
    Vector2f pos;
    float r;
    float w_sum = 0; //
    int n = 0;
    bool can_reach_boundary = true; // 是否可以到达边界
    std::array<std::pair<float,float>,L+1> fourier_coefficients; // 存储傅里叶系数，例如 [(a0,b0), (a1,b1), ...]
    std::array<std::pair<float,float>,L+1> fourier_coefficients2;
    //存储球边界上采样的点的角度和对应的值，后面可以用来重建源项,用于优化游走

    Sphere(float x,float y, float r,bool can_reach_boundary = true):pos(x,y),r(r),can_reach_boundary(can_reach_boundary){};

    Sphere(const Eigen::Vector2f& pos, float r):pos(pos),r(r){};

    float get_value(float theta, float r)
    {
        float r2 = r/this->r;
        float r2_pow = 1;
        auto [a,b] = fourier_coefficients[0] ;
        float x = a*r2_pow;
        r2_pow *= r2;
        for (int i = 1;i<L+1;i++)
        {
            auto [a,b] = fourier_coefficients[i];
            x +=  2 * (a * std::cos(i*theta) + b * std::sin(i*theta)) * r2_pow ;
            r2_pow *= r2;
           // std::cout<<"x = "<<x<<"\n";
        }
        return  x;
    }


    Eigen::Vector2f get_gradient(float theta, float r)
    {
        float c = std::cos(theta);
        float s = std::sin(theta);

        float cn = c;
        float sn = s;

        r = r/this->r;

        float rn = 1.0f;

        float ur = 0.0f;
        float ut = 0.0f;

        for (int n = 1; n <= 10; n++)
        {
            auto [a,b] = fourier_coefficients[n];

            float H = a * cn + b * sn;
            float T = -a * sn + b * cn;

            ur += n * rn * H;
            ut += n * rn * T;

            float cn1 = cn * c - sn * s;
            float sn1 = cn * s + sn * c;

            rn *= r;
            cn = cn1;
            sn = sn1;
        }
        ur/=this->r;
        ut/=this->r;
        float ux =
            ur * c
            - (ut) * s;

        float uy =
            ur * s
            + (ut) * c;

        return {2*ux,2*uy};
    }
};
class RTree2D {
public:
    RTree<int, float, 2> tree;
    std::vector<Sphere<10>>& spheres_;
    //Sphere<10> s;
    explicit RTree2D( std::vector<Sphere<10>>& spheres):spheres_(spheres){}
    void insert(int index_,float k = 1) {
        auto& sphere = spheres_[index_];
        float min[2] = {sphere.pos.x() - sphere.r*k, sphere.pos.y() - sphere.r*k};
        float max[2] = {sphere.pos.x() + sphere.r*k, sphere.pos.y() + sphere.r*k};
        tree.Insert( min, max,index_);
    }

    void search(Eigen::Vector2f pos, std::vector<int>& results,float k) {
        float min[2] = {pos.x(), pos.y()};
        float max[2] = {pos.x(), pos.y()};
        tree.Search(min, max, [&results,pos,this,k](int t) {
            auto& sphere = spheres_[t];
            if ( (sphere.pos - pos).norm() <= sphere.r * k)
            {
                results.push_back(t);
            }
            return true;
        });
    }
};



class HC_solver :public WosStableSolver2D{
public:

    HC_solver(int N, float epsilon, float k_re,std::shared_ptr<Scene> shape)
    : WosStableSolver2D(std::move(shape),N,epsilon),a_rec_(k_re),rtree_(spheres_)
    {}
    HC_solver(int N, float epsilon, float k_re,std::shared_ptr<Scene> shape,int K2,float lambda = 4000*9)
       : WosStableSolver2D(std::move(shape),N,epsilon),a_rec_(k_re),rtree_(spheres_),K2_(K2),lambda(lambda)
    {}
    void clear(){spheres_.clear();rtree_.tree.RemoveAll();}

    void precompute(std::vector<std::pair<float, float>> poi) override;

    float solve(float, float) override;

    Eigen::Vector2f get_gradient(float x, float y);
protected:

    Eigen::Vector2f compute_source_gradient(float x, float y,Eigen::Vector2f O,float r);

    float compute_source(float x,float y,Eigen::Vector2f O,float r);

    //重建半径
    float  a_rec_ = 0.9;

    std::vector<Sphere<10>> spheres_;
    std::vector<std::vector<Sphere<10>>> sphere_cache_; //每个线程的
    RTree2D rtree_;

    //权函数
    float getW(float r_norm)
    {
        r_norm = 1 - r_norm;
        return (3-2*r_norm)*r_norm*r_norm;
    }

    //表示每个球边界上要采样多少个点，N_i = lambda * r_i^(d-1) 上取整

    int N_min = 12;

    int N(float r)
    {
        return std::max(N_min,int(std::ceil(lambda * r)));
    }

    //每个点的最小权重
    float w_min_ = 0.5;

    //重建源项采样数量
    int K2_ = 10;
    float lambda = 50;
};
} // USTC_CG

#endif //USTC_CG_2025_FRAMEWORK2D_HC_SOLVER_H