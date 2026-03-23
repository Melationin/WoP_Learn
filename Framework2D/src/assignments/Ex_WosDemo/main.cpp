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





using namespace USTC_CG;
int main()
{


    int W = 256, H = 256;



    std::vector<float> u_ref(W * H);
    std::vector<float> u_sol(W * H);


    auto b = [](double x,double y) {
        //return x* y* sin(x)* cos(y);
        return x*x-y*y +x*y;
    };
    auto f = [](double x,double y) {
        return 2 *( y *cos(x)* cos(y) - x *sin(x)* sin(y) - x* y* sin(x)* cos(y) );
    };
    auto rect = std::make_shared<Rect>(-2,2,-2,2);
    WoP_solver solver{1,1e-4,0.9,rect};

   //HC_solver solver{1,1e-4,0.9,rect,100};

    //WosStableSolver2D solver(rect,20,1e-5);
    solver.set_boundary_condition(b);
    //solver.set_source_function(f);

    //std::cout << omp_get_max_threads();

    std::vector<std::tuple<double,double,int>> poi;
    std::vector<std::pair<double,double>> poi2;
    for (int y = 20; y < H-20; y++)
    {
        for (int x = 20; x < W-20; x++)
        {
            auto [fx,fy] = rect->getPos(x,y,W,H);

            u_ref[y * W + x] = b(fx,fy);

            poi.push_back({fx ,fy,y * W + x});
            poi2.emplace_back(fx ,fy );
        }
        //std::cout<<"r"<<std::endl;
    }


    TIME_TEST_BEGIN("solve")
    solver.precompute(poi2);
    for (auto [fx,fy,index]:poi)
    {
        u_sol[index] = solver.solve(fx,fy);
    }
    TIME_TEST_END

    return 0;
}
