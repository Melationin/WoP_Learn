//
// Created by zhdds on 2026/3/20.
//
#include <cmath>
#include <shapes/rect.h>
#include <solvers/stableWos.h>
#include <solvers/HC_solver.h>
#include <solvers/WoP.h>
#include <iostream>

#include "util.h"


#include <vector>
#include <cmath>
#include <iostream>

#include "PDEWindows.h"


using namespace USTC_CG;
int main()
{
    // ===== 1. 初始化 GLFW =====

    int W = 256, H = 256;



    std::vector<float> u_ref(W * H);
    std::vector<float> u_sol(W * H);

    auto b = [](double x,double y) {
        //return x* y* sin(x)* cos(y);
        static Counter counter{"boundary"};
        counter.increment();
        return x*x-y*y +x*y;
    };
    auto f = [](double x,double y) {
        return 2 *( y *cos(x)* cos(y) - x *sin(x)* sin(y) - x* y* sin(x)* cos(y) );
    };
    auto rect = std::make_shared<Rect>(-2,2,-2,2);

    /*
    boundary: count = 100 9186
distance_to_boundary: count = 1376 9902
search_HC: count = 0
search: count = 662 1521
     */
    WoP_solver solver{1,1e-4,0.9,rect};

    //HC 边缘计算次数：359349

    /*
    boundary: count = 299 2016
distance_to_boundary: count = 3726 0599
search_HC: count = 64516
search: count = 0
     */
    //HC_solver solver{1,1e-4,0.9,rect,100};

    //WosStableSolver2D solver(rect,20,1e-5);
    solver.set_boundary_condition(b);
    //solver.set_source_function(f);

   // std::cout << omp_get_max_threads();

    std::vector<std::tuple<double,double,int>> poi;
    std::vector<std::pair<double,double>> poi2;

    for (int y = 10; y < H-10; y++)
    {
        for (int x = 10; x < W-10; x++)
        {
            auto [fx,fy] = rect->getPos(x,y,W,H);

            u_ref[y * W + x] = b(fx,fy);

            poi.emplace_back(fx ,fy,y * W + x);
            poi2.emplace_back(fx ,fy );
        }
        //std::cout<<"r"<<std::endl;
    }

    auto start = std::chrono::high_resolution_clock::now();

    solver.precompute(poi2);

    std::shuffle(poi.begin(),poi.end(),std::mt19937{std::random_device{}()});
    for (auto [fx,fy,index]:poi)
    {
        u_sol[index] = solver.solve(fx,fy);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "耗时: " << duration.count() << " ms\n";
    PDEWindows windows{W,H};
    windows.init(W,H);

    windows.set_reference(u_ref);
    windows.set_solution(u_sol);

    windows.run();

    return 0;
}
