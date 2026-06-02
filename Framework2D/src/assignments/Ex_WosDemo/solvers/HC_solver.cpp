//
// Created by zhdds on 2026/3/20.
//

#include "HC_solver.h"
#include "util/thread_pool.h"
namespace USTC_CG {

void HC_solver::precompute(std::vector<std::pair<float, float>> poi)
{
}
static ThreadPool pool{1};
float HC_solver::solve(float x, float y)
{
    std::vector<int> results;
    rtree_.search({x,y},results,a_rec_);

    float w_sum  = 0;

    for (auto i:  results)
    {
        Sphere<10>& it = spheres_[i];
        float r = it.r;
        float r_norm = (Eigen::Vector2f(x,y) - it.pos ).norm();
        w_sum += getW(r_norm/r);
    }
    if (w_sum < w_min_)
    {
        auto r  = shape->distance_to_boundary({x,y});
        spheres_.emplace_back(x,y,r);
        Sphere<10>& now_sphere = spheres_[spheres_.size()-1];
        rtree_.insert(spheres_.size()-1);
        int n = N(r);
        float d_theta = 2 * pi / n;
        int n_threads = pool.numThreads();
        int k = (n + n_threads - 1) / n_threads;
        //k/=3;
        //std::cout<<"n = "<<n<<" k = "<<k<<"\n";
        std::vector<std::future<void>> futures;
        futures.reserve(n/k+1);
        std::vector<Sphere<10>> temp_spheres(n/k+1,now_sphere);
        for (int t = 0; t*k < n; t++)
        {
            futures.push_back(pool.enqueue([t,k,n,d_theta,x,y,r,this,&temp_spheres]() {
                Sphere<10>& temp_sphere = temp_spheres[t];
                for (int i = k*t;i <k*(t+1) && i<n; i++)
                {
                    float theta = i * d_theta;
                    auto p = shape ->walkUncheck({x,y},theta,r);
                    float sample = WosStableSolver2D::solve(p.x(),p.y());
                    temp_sphere.w_sum += 1;
                    temp_sphere.n++;
                    temp_sphere.fourier_coefficients[0].first += sample;
                    const float c1 = std::cos(theta);
                    const float s1 = std::sin(theta);
                    float cn = c1 ;
                    float sn = s1 ;
                    for (int l = 1; l <= 10; l++)
                    {
                        temp_sphere.fourier_coefficients[l].first +=  (sample) * cn;
                        temp_sphere.fourier_coefficients[l].second +=  (sample) * sn;
                        float cn_1  =  cn * c1 - sn * s1;
                        float sn_1 = cn * s1 + sn * c1;
                        cn = cn_1;
                        sn = sn_1;
                    }
                }
            }));
        }
        for (int i = 0; i < futures.size(); i++)
        {
            futures[i].get();
            Sphere<10>& temp_sphere = temp_spheres[i];
            for (int l = 0; l <= 10; l++)
            {
                now_sphere.fourier_coefficients[l].first += temp_sphere.fourier_coefficients[l].first;
                now_sphere.fourier_coefficients[l].second += temp_sphere.fourier_coefficients[l].second;
            }
            now_sphere.w_sum += temp_sphere.w_sum;
            now_sphere.n += temp_sphere.n;
        }
        for (auto& [ a, b]: now_sphere.fourier_coefficients)
        {
            a /= now_sphere.w_sum;
            b /= now_sphere.w_sum;
        }
        results.push_back(spheres_.size()-1);
        w_sum += getW(0);
    }
    float res = 0;
    float source = 0;
    for (auto i:  results)
    {
        Sphere<10>& it = spheres_[i];
        float r = it.r;
        Eigen::Vector2f dv = Eigen::Vector2f(x,y) - it.pos ;
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



Eigen::Vector2f HC_solver::get_gradient(float x, float y)
{
    std::vector<int> results;

    rtree_.search({x,y},results,a_rec_);

    float w_sum  = 0;

    for (auto i:  results)
    {
        Sphere<10>& it = spheres_[i];
        float r = it.r;
        float r_norm = (Eigen::Vector2f(x,y) - it.pos ).norm();
        w_sum += getW(r_norm/r);
    }
    if (w_sum < w_min_)
    {
        auto r  = shape->distance_to_boundary({x,y});
        if (r > 1)
        {
            r = 0.5;
        }
        spheres_.emplace_back(x,y,r);
        Sphere<10>& now_sphere = spheres_[spheres_.size()-1];
        rtree_.insert(spheres_.size()-1);
        int n = N(r);
        float d_theta = 2 * pi / n;
        float theta = 0;
        for (int i = 0; i < n; i++)
        {
            float sample = WosStableSolver2D::solve(x + r*cos(theta) , y + r*sin(theta));
            auto& sp = now_sphere;
            sp.w_sum += 1;
            sp.n++;
            sp.fourier_coefficients[0].first += sample;
            const float c1 = std::cos(theta);
            const float s1 = std::sin(theta);

            float cn = c1 ;
            float sn = s1 ;

            for (int l = 1; l <= 10; l++)
            {
                sp.fourier_coefficients[l].first +=  (sample) * cn;
                sp.fourier_coefficients[l].second +=  (sample) * sn;
                float cn_1  =  cn * c1 - sn * s1;
                float sn_1 = cn * s1 + sn * c1;
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
    Eigen::Vector2f res = {0,0};
    Eigen::Vector2f source = {0,0};
    for (auto i:  results)
    {
        Sphere<10>& it = spheres_[i];
        float r = it.r;
        Eigen::Vector2f dv = Eigen::Vector2f(x,y) - it.pos ;
        float r_norm = dv.norm();
        float weight= getW(r_norm/r);
        res += weight * it.get_gradient(atan2(dv.y(),dv.x()),r_norm);
        if (has_source)
        {
            source += weight * compute_source_gradient(x,y,it.pos,r);
        }
    }
    res /= w_sum;
    source /= w_sum;
    return res - source;
}

float HC_solver::compute_source(float x, float y,Eigen::Vector2f O,float r)
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


Eigen::Vector2f HC_solver::compute_source_gradient(float x, float y,Eigen::Vector2f O,float r)
{

    Eigen::Vector2f source = {0,0};
    for (int k = 0; k < K2_; k++)
    {
        const float r2 =sqrt(randFloat()) * r;
        if (r2 < float_epsilon) continue;
        const float arg = randFloat()*2*pi;
        Eigen::Vector2f v2 = shape->walkUncheck(O,r2,arg);
        source += shape->source(v2)*Ball_Green_Grad<2>({x,y},v2,O,r);
    }
    return source*r*r*pi/K2_;
}
} // USTC_CG