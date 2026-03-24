//
// Created by zhddsj on 2026/3/20.
//

#ifndef USTC_CG_2025_FRAMEWORK2D_HC_SOLVER_H
#define USTC_CG_2025_FRAMEWORK2D_HC_SOLVER_H
#include <iostream>

#include "stableWos.h"

#include <Eigen/Dense>
#include <RTree.h>

namespace USTC_CG {

// L为展开项数
template<int L>
struct Sphere {
    Eigen::Vector2d pos;
    double r;
    double w_sum = 0; //
    int n = 0;
    std::array<std::pair<double,double>,L+1> fourier_coefficients; // 存储傅里叶系数，例如 [(a0,b0), (a1,b1), ...]

    //存储球边界上采样的点的角度和对应的值，后面可以用来重建源项,用于优化游走
    std::vector<int> neighbors;
    RTree<int,double,2> neighbor_tree; // 存储邻接球的rtree


    Sphere(double x,double y, double r):pos(x,y),r(r){};

    Sphere(const Eigen::Vector2d& pos, double r):pos(pos),r(r){};

    double get_value(double theta, double r)
    {
        double r2 = r/this->r;
        double r2_pow = 1;
        auto [a,b] = fourier_coefficients[0] ;
        double x = a*r2_pow;
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
};
class RTree2D {
public:
    RTree<int, double, 2> tree;
    std::vector<Sphere<10>>& spheres_;

    explicit RTree2D( std::vector<Sphere<10>>& spheres):spheres_(spheres){}
    void insert(int index_,double k = 1) {
        auto& sphere = spheres_[index_];
        double min[2] = {sphere.pos.x() - sphere.r*k, sphere.pos.y() - sphere.r*k};
        double max[2] = {sphere.pos.x() + sphere.r*k, sphere.pos.y() + sphere.r*k};
        tree.Insert( min, max,index_);
    }

    void search(Eigen::Vector2d pos, std::vector<int>& results,double k) {
        double min[2] = {pos.x(), pos.y()};
        double max[2] = {pos.x(), pos.y()};
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

    HC_solver(int N, double epsilon, double k_re,std::shared_ptr<Shape> shape)
    : WosStableSolver2D(std::move(shape),N,epsilon),a_rec_(k_re),rtree_(spheres_)
    {}
    HC_solver(int N, double epsilon, double k_re,std::shared_ptr<Shape> shape,int K2,double lambda = 4000*9)
       : WosStableSolver2D(std::move(shape),N,epsilon),a_rec_(k_re),rtree_(spheres_),K2_(K2),lambda(lambda)
    {}
    void clear(){spheres_.clear();rtree_.tree.RemoveAll();}

    void precompute(std::vector<std::pair<double, double>> poi) override;

    double solve(double, double) override;


protected:

    double compute_source(double x,double y,Eigen::Vector2d O,double r);

    //重建半径
    double  a_rec_ = 0.9;

    std::vector<Sphere<10>> spheres_;
    RTree2D rtree_;

    //权函数
    double getW(double r_norm)
    {
        r_norm = 1 - r_norm;
        return (3-2*r_norm)*r_norm*r_norm;
    }

    //表示每个球边界上要采样多少个点，N_i = lambda * r_i^(d-1) 上取整


    int N_min = 12;

    int N(double r)
    {
        return std::max(N_min,int(std::ceil(lambda * r)));
    }

    //每个点的最小权重
    double w_min_ = 0.5;

    //重建源项采样数量
    int K2_ = 100;
    double lambda = 4000;
};
} // USTC_CG

#endif //USTC_CG_2025_FRAMEWORK2D_HC_SOLVER_H