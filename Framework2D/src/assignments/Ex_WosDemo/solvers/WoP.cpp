//
// Created by zhdds on 2026/3/22.
//

#include "WoP.h"
#include <RTree.h>
namespace USTC_CG {
static Counter searchCount{"search"};

void WoP_solver::precompute(std::vector<std::pair<double, double>> poi)
{
    computeProbes(std::move(poi));


    std::cout<<"finish1";
    WalkData walkData{};
    for (int i = 0;i<spheres_.size();i++)
    {

        auto& sphere = spheres_[i];
        int n = N(sphere.r);
        while (sphere.n < n)
        {
            double theta = 2 * pi * dist_(rand_);

            walkData.x = sphere.pos.x() + sphere.r * std::cos(theta);
            walkData.y = sphere.pos.y() + sphere.r * std::sin(theta);
            walkData.walked_spheres.clear();
            walkData.walked_spheres.emplace_back(i,theta,1/(2*pi));
            //暂时不做源项的
            double solution = 0;

            int n = 0;

            while (true)
            {
                n++;
                auto [bx,by] = shape->distance_to_boundary(walkData.x,walkData.y);
                double r = (bx - walkData.x)*(bx-walkData.x) + (by - walkData.y)*(by - walkData.y);
                if (r < epsilon_*epsilon_)
                {
                    solution = boundary(bx,by);
                    break;
                }

                double min[2] = {walkData.x,walkData.y};
                double max[2] = {walkData.x,walkData.y};
                //找到合适的球
                double m_r = std::numeric_limits<double>::max();
                int m_index = -1;
                //searchCount++;
                auto& now_sp = spheres_[ std::get<0>( walkData.walked_spheres.back())];
                now_sp.neighbor_tree.Search(min,max,[&](int i2) {
                    auto &sp = spheres_[i2];
                    if (double r2 = (Vector2d(walkData.x,walkData.y) - sp.pos).squaredNorm();
                    r2 < sp.r*sp.r * a_walk*a_walk)
                    {
                        if (double r_n =  r2/(sp.r*sp.r);r_n < m_r)
                        {
                            m_r = r_n;
                            m_index = i2;
                        }
                    }
                    return true;
                });

                if (m_index == -1)
                {
                    solution = USTC_CG::WosStableSolver2D::solve(walkData.x,walkData.y);
                    break;
                }else
                {
                    auto &sp_next = spheres_[m_index];

                    auto [next_p,theta2,pdf] = sample_poisson_kernel({walkData.x,walkData.y},sp_next.pos,sp_next.r);
                    walkData.walked_spheres.emplace_back(m_index,theta2,pdf);
                    walkData.x = next_p.x();
                    walkData.y = next_p.y();

                }
            }
            for (auto [index,theta2,pdf]: walkData.walked_spheres)
            {
                auto& sp = spheres_[index];
                const double w = 1.0 / (2*pi* pdf);
                sp.w_sum += w;
                sp.n++;
                sp.fourier_coefficients[0].first += w * solution;
                const double a_0 = sp.fourier_coefficients[0].first / sp.w_sum;
                const double c1 = std::cos(theta2);
                const double s1 = std::sin(theta2);

                double cn = c1 * w;
                double sn = s1 * w;

                for (int l = 1; l <= 10; l++)
                {
                    sp.fourier_coefficients[l].first +=  (solution - a_0) * cn;
                    sp.fourier_coefficients[l].second +=  (solution - a_0) * sn;
                    double cn_1  =  cn * c1 - sn * s1;
                    double sn_1 = cn * s1 + sn * c1;
                    cn = cn_1;
                    sn = sn_1;
                }
            }

        }
    }
    std::cout<<"finish2";
    for (auto &it : spheres_)
    {
        for (auto& [ a, b]: it.fourier_coefficients)
        {
            a /= it.w_sum;
            b /= it.w_sum;
        }
    }

}

double WoP_solver::solve(double x, double y)
{
    std::vector<int> results;
    searchCount++;
    rtree_.search({x,y},results,a_rec_);

    double w_sum  = 0;

    for (auto i:  results)
    {
        Sphere<10>& it = spheres_[i];
        double r = it.r;
        double r_norm = (Eigen::Vector2d(x,y) - it.pos ).norm();
        w_sum += getW(r_norm/r);
    }


    double res = 0;
    double source = 0;
    for (auto i:  results)
    {
        Sphere<10>& it = spheres_[i];
        double r = it.r;
        Vector2d dv = Vector2d(x,y) - it.pos ;
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

void WoP_solver::computeProbes(std::vector<std::pair<double, double>> poi)
{
    std::vector<std::tuple<double,double,double>> poi_rand;
    poi_rand.reserve(poi.size());
    for (auto& it : poi)
    {
        poi_rand.emplace_back(it.first,it.second,0);
    }
    std::shuffle(poi_rand.begin(),poi_rand.end(),std::minstd_rand{std::random_device{}()});

    RTree<int, double, 2> poi_rtree;

    for (int i = 0;i< poi_rand.size();++i)
    {
        auto [x,y,_] = poi_rand[i];
        const double min[2] = {x,y};
        const double max[2] = {x,y};
        poi_rtree.Insert(min,max,i);
    }

    //先算出所有的球。后面会计算每个球的领接球
    for (auto& [x,y,w] : poi_rand)
    {
        while (w < w_min_)
        {
            auto [bx,by]  = shape->distance_to_boundary(x,y);
            const double r  = sqrt((bx-x)*(bx-x) + (by-y)*(by-y));
            spheres_.emplace_back(x,y,r);
            rtree_ .insert(spheres_.size()-1,a_rec_);

            const double min[2] = {x - r*a_walk,y - r*a_walk};
            const double max[2] = {x + r*a_walk,y + r*a_walk};

            searchCount++;
            poi_rtree.Search(min,max,[&](int t){
                auto& [x2,y2,w2] = poi_rand[t];
                if (w >= w_min_) return true;
                if (double r_2 = Eigen::Vector2d{x - x2,y - y2} .norm();r_2< r * this->a_walk)
                {
                    w2 += getW(r_2/r);

                }
                return true;
            });
        }
    }


    for (auto& sp : spheres_)
    {
        const double min[2] ={sp.pos.x() - sp.r, sp.pos.y() - sp.r};
        const double max[2] ={sp.pos.x() + sp.r, sp.pos.y() + sp.r};
        rtree_.tree.Search(min,max,[&](int i){
            auto& sp2 = spheres_[i];
            if (double r_2 = (sp.pos - sp2.pos).norm();r_2 < sp.r  + sp2.r * a_walk)
            {
                //sp.neighbors.push_back(i);
                const double min2[2] ={sp2.pos.x() - sp2.r* a_walk,sp2.pos.y() - sp2.r* a_walk};
                const double max2[2] ={sp2.pos.x() + sp2.r* a_walk,sp2.pos.y() + sp2.r* a_walk};
                sp.neighbor_tree.Insert(min2,max2,i);
            }
            return true;
        });
    }
}
} // USTC_CG