//
// Created by zhdds on 2026/3/20.
//
#include <cmath>
#include <shapes/rect.h>
#include <shapes/polygon.h>
#include <shapes/line.h>
#include <solvers/stableWos.h>
#include <solvers/HC_solver.h>
#include <iostream>

#include "util.h"


#include <vector>

#include <cmath>
#include <iostream>

#include "PDEWindows.h"
#include "solvers/WoP.h"


using namespace USTC_CG;

Polygon make_test_polygon()
{
    std::vector<std::pair<float, float>> pts;

    const int N = 360;  // 至少64个点，这里给96个
    const float PI = 3.14159265358979323846f;

    for (int i = 0; i < N; ++i)
    {
        float t = 2.0f * PI * i / N;

        // 强烈起伏的半径函数：
        // 保证始终 > 0，因此按角排序后得到的是简单星形多边形，不会自交
        float r =
            1.3f
            + 0.23f * std::sin(3.0f * t + 0.20f)
            + 0.22f * std::sin(7.0f * t + 1.10f)
            + 0.1f * std::cos(11.0f * t + 0.60f)
            + 0.1f * std::sin(17.0f * t + 2.00f)
            + 0.08f * std::cos(29.0f * t + 0.35f);

        // 再加一点角度扰动，让轮廓更“扭曲”
        float theta =
            t
            + 0.08f * std::sin(5.0f * t + 0.40f)
            + 0.05f * std::sin(13.0f * t + 1.70f);

        float x = r * std::cos(theta);
        float y = r * std::sin(theta);

        pts.emplace_back(x, y);
    }

    return Polygon(pts);
}


//return l2 error and linf error
std::pair<double,double> computeError(std::vector<float> ref,std::vector<float> sol,int count)
{
    double l2_error = 0.0f;
    double linf_error = 0.0f;

    for (int i = 0; i < ref.size(); i++)
    {
        double err = std::abs(sol[i] - ref[i]);

        l2_error += err * err;
        linf_error = std::max(linf_error, err);
    }

    l2_error = std::sqrt(l2_error / count);
    return {l2_error,linf_error};
}

template<class Func>
class TestScene final : public Scene {
public:

    Point p1{-0.5,0.5},p2{0.5,0.5},p3{0.5,-0.5},p4{-0.5,-0.5};
    Func func;

    explicit TestScene(Func&& func):func(func)
    {
    }

    R_DC distance_to_boundaryAndDC(Point p) override
    {
        float x = p.x();
        float y = p.y();

        float min = 1000;
        float p_min = 0;
        if ( abs(1-y) < min)
        {
            min = abs(1-y);
            p_min = func(x,1.0);
        }
        if ( abs(-1-y) < min)
        {
            min = abs(-1-y);
            p_min = func(x,-1.0);
        }

        return {min,p_min};


    };

    double distanceToMandelbrotBoundary(double cx, double cy) {
        double zx = 0.0, zy = 0.0;   // z = z^2 + c 的当前值
        double dzx = 0.0, dzy = 0.0; // dz/dc，初始为 0

        const int maxIter = 1000;
        const double escapeRadius2 = 4.0; // |z|^2 > 4 即逃逸

        for (int iter = 0; iter < maxIter; ++iter) {
            // 更新导数: dz = 2*z*dz + 1
            // 复数乘法: (zx + i*zy) * (dzx + i*dzy) = (zx*dzx - zy*dzy) + i*(zx*dzy + zy*dzx)
            double new_dzx = 2.0 * (zx * dzx - zy * dzy) + 1.0;
            double new_dzy = 2.0 * (zx * dzy + zy * dzx);
            dzx = new_dzx;
            dzy = new_dzy;

            // 更新 z = z*z + c
            double new_zx = zx * zx - zy * zy + cx;
            double new_zy = 2.0 * zx * zy + cy;
            zx = new_zx;
            zy = new_zy;

            double z2 = zx * zx + zy * zy;
            if (z2 > escapeRadius2) {
                // 逃逸，计算距离估计
                double modz = std::sqrt(z2);
                double moddz = std::sqrt(dzx * dzx + dzy * dzy);
                // 避免除以零
                if (moddz < 1e-12)
                    return 1e10; // 很大，表示距离非常远（实际上不会发生）
                // 距离估计公式
                return modz * std::log(modz) / moddz;
            }
        }
        // 10 次迭代内未逃逸，视为集合内部点
        return 0.0;
    }

    bool isInMandelbrot(double cx, double cy) {
        double zx = 0.0;  // z 的实部
        double zy = 0.0;  // z 的虚部

        for (int i = 0; i < 100; ++i) {
            // 计算 z = z*z + c
            double new_zx = zx * zx - zy * zy + cx;
            double new_zy = 2.0 * zx * zy + cy;
            zx = new_zx;
            zy = new_zy;

            // 检查模平方是否超过 2^2 = 4 (即 |z| > 2)
            if (zx * zx + zy * zy > 4.0) {
                return false;  // 发散，不在集合内
            }
        }
        return true;  // 10次迭代后仍未发散，认为在集合内
    }

    Point walkUncheck(Point p, float ang, float r) override
    {
       Point p2 = p + Point{std::cos(ang),std::sin(ang)}*r;
        if (p2.x() > 1) p2.x() -= 1;
        if (p2.x() < -1) p2.x() += 1;
        return p2;
    };

    double distance_to_boundary(Point p) override
    {
        return distance_to_boundaryAndDC(p).r;
    };
};

int main1()
{
    // ===== 1. 初始化 GLFW =====

    int W = 256, H = 256;
    std::vector<float> u_ref(W * H);
    std::vector<float> u_sol(W * H);

    auto b = [](float x,float y) {
        return sin(pi * x) *cosh(pi*y)/cosh(pi);
    };
    auto rect = std::make_shared<TestScene<decltype(b)>>(std::move(b));

    auto& ref = b;

    auto bG = [](float x,float y) {
        return 2*(sqrt(x*x + y*y));
    };
    auto f = [](float x,float y) {
        if ((x)*(x) + (y*y) < 1*1 && (x)*(x) + (y*y) > 0.8*0.8 )
        {
            return 10.0f;
        }
        return  0.0f;
    };
    //auto rect = std::make_shared<Polygon>(make_test_polygon());



    /*
boundary: count = 17057599
distance_to_boundary_WoS: count = 169647571
distance_to_boundary_WoP: count = 0
search: count = 67569
search_HC: count = 0
distance_to_boundary: count = 170108035
     */
    //WoP_solver solver2{10,1e-4,0.9,rect,100};
    HC_solver solver{50,1e-4,0.9,rect,100,1000};

    //WosStableSolver2D solver{rect,1000,1e-5};
    //HC 边缘计算次数：359349

    /*
    l2 error: 0.00569922
linf error: 0.0220187
boundary: count = 39096801
distance_to_boundary_WoS: count = 487410670
distance_to_boundary_WoP: count = 0
search: count = 0
search_HC: count = 65025
distance_to_boundary: count = 487412162
     */

    //HC_solver solver{1,1e-4,0.9,rect,100,20000};

    //WosStableSolver2D solver(rect,20,1e-5);
    //solver.set_source_function(f);

   // std::cout << omp_get_max_threads();

    std::vector<std::tuple<float,float,int>> poi;
    std::vector<std::pair<float,float>> poi2;

    int extend = 1;

    for (int y = 1 + extend; y < H - extend; y++)
    {
        for (int x = 1 + extend; x < W - extend; x++)
        {
            float fx = (x+0.5f)/W*2-1;
            float fy = (y+0.5f)/H*2-1;

            u_ref[y * W + x] = ref(fx,fy);
            poi.emplace_back(fx ,fy,y * W + x);
            poi2.emplace_back(fx ,fy );
        }
        //std::cout<<"r"<<std::endl;
    }


    std::shuffle(poi.begin(),poi.end(),std::minstd_rand{std::random_device{}()});


    auto start = std::chrono::high_resolution_clock::now();

    //auto temp = solver.solve(0,0);



    //std::cout<<"finish solve"<<std::endl;
    //return 0;


    solver.precompute(poi2);
    for (auto& [x,y,i] : poi)
    {
        u_sol[i] = solver.solve(x,y);
    }
    std::cout<<"time: "<<std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count()<<" ms\n";

    //return 0;
    auto err = computeError(u_ref,u_sol,poi.size());
    std::cout<<"l2 error: "<<err.first<<std::endl;
    std::cout<<"linf error: "<<err.second<<std::endl;
    //return 0;
    PDEWindows windows{};
     windows.init(W,H);

    windows.set_reference(u_ref);
     windows.set_solution(u_sol);

     windows.run();


}


/*WoP
 *
 *
 18s
l2 error: 0.0267016
linf error: 0.511648
distance_to_boundary_WoP_solution: count = 78301
boundary: count = 1258158
distance_to_boundary_WoS: count = 12355258
distance_to_boundary_WoP: count = 1235778
search: count = 25338
search_HC: count = 0
distance_to_boundary: count = 0



HC

耗时: 20210 ms
l2 error: 0.0365668
linf error: 0.408593
boundary: count = 1222481
distance_to_boundary_WoS: count = 15149202
distance_to_boundary_WoP: count = 0
search: count = 0
search_HC: count = 22380
distance_to_boundary: count = 0
*/
