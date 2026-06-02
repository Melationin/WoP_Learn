//
// Created by zhdds on 2026/3/22.
//

#include "WoP.h"
#include <RTree.h>
namespace USTC_CG {

constexpr int optMethod =1;

static Counter searchCount{"search"};
static Counter distance_to_boundary_WoP{"distance_to_boundary_WoP"};
void WoP_solver::precompute(std::vector<std::pair<float, float>> poi)
{

    float temp_lambda = lambda;

    //lambda = lambda/500;



    addProbes(std::move(poi));
    std::cout<<"finish1"<<std::endl;
    WalkData walkData{};
    for (int i = 0;i<probes_.size();i++)
    {

        auto& sphere = probes_[i];
        int n = N(sphere.r);
        while (sphere.n < n)
        {
            float theta = 2 * pi * randFloat();

            walkData.x = sphere.pos.x() + sphere.r * std::cos(theta);
            walkData.y = sphere.pos.y() + sphere.r * std::sin(theta);
            walkData.walked_spheres.clear();
            walkData.walked_spheres.emplace_back(sphere,theta,1/(2*pi));
            //暂时不做源项的
            float solution = 0;

            int n2 = 0;

            sphere.n++;
            while (true)
            {
                n2++;
                auto& now_sp = std::get<0>( walkData.walked_spheres.back());
                float phi =  std::get<1>( walkData.walked_spheres.back());
                if (phi < 0) phi += 2*pi;
                int i = phi / (2*pi)*180.0;
                int m_index = now_sp.neighbor_i[i];

                float m_r = std::numeric_limits<float>::max();

                if (m_index == -1 || n2 > 40)
                {
                    if ( ((now_sp.reach_b[i]) && optMethod == 0)||(now_sp.can_reach_boundary&&optMethod == 1) ||(optMethod == -1))
                    {
                        distance_to_boundary_WoP++;
                        auto [r,d] = shape->distance_to_boundaryAndDC({walkData.x,walkData.y});
                        if (r < epsilon_)
                        {
                            static Counter distance_to_boundary_count3{"distance_to_boundary_WoP_solution"};
                            distance_to_boundary_count3++;
                            solution = d;
                            break;
                        }

                    }
                    solution = USTC_CG::WosStableSolver2D::solve(walkData.x,walkData.y);
                    break;
                }else
                {
                    auto &sp_next = probes_[m_index];
                    auto [next_p,theta2,pdf] = sample_poisson_kernel({walkData.x,walkData.y},sp_next.pos,sp_next.r);
                    walkData.walked_spheres.emplace_back(sp_next,theta2,pdf);
                    walkData.x = next_p.x();
                    walkData.y = next_p.y();
                }

            }

            for (auto [sp,theta2,pdf]: walkData.walked_spheres)
            {
                //std::cout<<walkData.walked_spheres.size()<<std::endl;
                const float w = 1.0 / (2*pi* pdf);
                sp.w_sum += w;
                sp.fourier_coefficients[0].first += w*solution;
                //const float a_0 = sp.fourier_coefficients[0].first / sp.w_sum;
                const float c1 = std::cos(theta2);
                const float s1 = std::sin(theta2);
                float cn = c1 *w;
                float sn = s1 *w;
                for (int l = 1; l <= 10; l++)
                {
                    sp.fourier_coefficients[l].first +=  (solution  ) * cn;
                    sp.fourier_coefficients[l].second +=  (solution ) * sn;
                    sp.fourier_coefficients2[l].first +=  cn;
                    sp.fourier_coefficients2[l].second +=   sn;
                    float cn_1  =  cn * c1 - sn * s1;
                    float sn_1 = cn * s1 + sn * c1;
                    cn = cn_1;
                    sn = sn_1;
                }
            }

        }
    }

    for (auto &it : probes_)
    {
        it.fourier_coefficients[0].first /= it.w_sum;
        //std::cout<<w.su
        float a_0 = it.fourier_coefficients[0].first ;
        for (int l = 1;l<= 10;l++)
        {
            auto& [ a, b] = it.fourier_coefficients[l];
            auto& [ a2, b2] = it.fourier_coefficients2[l];
            a =(a - a_0*a2) / it.w_sum;
            b =(b - a_0*b2) / it.w_sum;
        }
    }


}



float WoP_solver::solve(float x, float y)
{
    std::vector<int> results;
    searchCount++;
    const float min[2] = {x,y};
    const float max[2] = {x,y};
    probes_rtree_.Search(min,max,[&results,this, x, y](int t) {
        auto& sphere = probes_[t];
        if ( (sphere.pos - Eigen::Vector2f{x,y}).norm() <= sphere.r * a_rec)
        {
            results.push_back(t);
        }
        return true;
    });

    float w_sum  = 0;

    for (auto i:  results)
    {
        Sphere<10>& it =probes_[i];
        float r = it.r;
        float r_norm = (Eigen::Vector2f(x,y) - it.pos ).norm();
        w_sum += getW(r_norm/r);
    }


    float res = 0;
    float source = 0;
    for (auto i:  results)
    {
        Sphere<10>& it = probes_[i];
        float r = it.r;
        Vector2f dv = Vector2f(x,y) - it.pos ;
        float r_norm = dv.norm();
        float weight= getW(r_norm/r);
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

//方法0 缓存边界距离
//方法1 减少大探针球大小


void WoP_solver::addProbes(std::vector<std::pair<float, float>> poi,std::vector<std::pair<float,float>>* poi_out)
{

    std::vector<std::tuple<float,float,float>> poi_rand;
    poi_rand.reserve(poi.size());
    for (auto& it : poi)
    {
        poi_rand.emplace_back(it.first,it.second,0);
    }
    std::shuffle(poi_rand.begin(),poi_rand.end(),std::minstd_rand{std::random_device{}()});

    RTree<int, float, 2> poi_rtree;
    for (int i = 0;i< poi_rand.size();++i)
    {
        auto [x,y,_] = poi_rand[i];
        const float min[2] = {x,y};
        const float max[2] = {x,y};
        poi_rtree.Insert(min,max,i);
    }

    std::vector<int> probes_now;
    //先算出所有的球。后面会计算每个球的邻居球
    for (auto& [x,y,w] : poi_rand)
    {
        while (w < w_min_)
        {
            auto [r,d]  = shape->distance_to_boundaryAndDC({x,y});

            if (optMethod == 1 && r > epsilon_ * k)
            {
                //r = r - epsilon_;

                probes_.emplace_back(x,y,r,true);
                probes_now.push_back(probes_.size()-1);
                //break;
            }else
            {
                probes_.emplace_back(x,y,r);
                probes_now.push_back(probes_.size()-1);
            }
            const float min[2] = {x - r,y - r};
            const float max[2] = {x + r,y + r};
            probes_rtree_ .Insert(min,max,probes_.size()-1);

            const float min2[2] = {x - r*a_walk,y - r*a_walk};
            const float max2[2] = {x + r*a_walk,y + r*a_walk};

            searchCount++;
            poi_rtree.Search(min2,max2,[&](int t){
                auto& [x2,y2,w2] = poi_rand[t];
                if (w2 >= w_min_) return true;
                if (float r_2 = Eigen::Vector2f{x - x2,y - y2} .norm();r_2< r * this->a_walk)
                {
                    w2 += getW(r_2/r);
                }
                return true;
            });
        }
    }




    for (int sp_index : probes_now)
    {
        auto& sp = probes_[sp_index];
        loadProbe(sp);
    }

}

void WoP_solver::loadProbe(Probe& sp)
{
    const float min[2] ={sp.pos.x() - sp.r, sp.pos.y() - sp.r};
    const float max[2] ={sp.pos.x() + sp.r, sp.pos.y() + sp.r};
    probes_rtree_.Search(min,max,[&](int i){
        auto& sp2 = probes_[i];
        if (float r_2 = (sp.pos - sp2.pos).norm();r_2 < sp.r  + sp2.r * a_walk)
        {
            Vector2f dv = sp2.pos - sp.pos;
            float phi = std::atan2(dv.y(),dv.x());
            float alpha = std::acos((sp.r*sp.r + r_2*r_2 - sp2.r*sp2.r * a_walk*a_walk) / (2*sp.r*r_2));
            if (phi < 0) phi += 2*pi;
            sp.neighbors.emplace_back(i ,phi,alpha);
        }
        return true;
    });

    for (int i = 0;i<180;i++)
    {
        float phi = 2 * pi * i / 180;
        double m_r = std::numeric_limits<double>::max();
        double m_index  = -1;
        Vector2f pos = sp.pos + sp.r  * Vector2f(std::cos(phi),std::sin(phi));
        sp.findNeighbors(phi,[&](int i2) {
                auto &sp2 =probes_[i2];
                if (float r2 = (pos - sp2.pos).squaredNorm();
                r2 < sp.r*sp.r * a_walk*a_walk)
                {
                    if (float r_n =  r2/(sp.r*sp.r);r_n < m_r)
                    {
                        m_r = r_n;
                        m_index = i2;
                    }
                }
                //return true;
            });
        sp.neighbor_i[i] = m_index;

        //方法1 直接缓存边界距离。但高维时占用内存大
        /*
        auto [bx,by]  = shape->distance_to_boundary(pos.x(),pos.y());
        float r  = sqrt((bx-pos.x())*(bx-pos.x()) + (by-pos.y())*(by-pos.y()));
        if (r < epsilon_ + 2*pi*sp.r/180)
        {
            sp.reach_b[i] = true;
        }
        */
    }
}
} // USTC_CG