//
// Created by zhdds on 2026/6/2.
//

#ifndef USTC_CG_2025_FRAMEWORK2D_SCENE_H
#define USTC_CG_2025_FRAMEWORK2D_SCENE_H
#include <concepts>
#include <utility>
#include <type_traits>
#include <concepts>
#include <utility>


namespace USTC_CG {

using Point = Eigen::Vector2f;


class Scene {
public:
    virtual ~Scene() = default;
    virtual  double distance_to_boundary(Point p)= 0;

    //狄利克雷
    struct R_DC{float r;float d;};
    virtual R_DC distance_to_boundaryAndDC(Point p) = 0;

    virtual float source(Point p){return 0;};

    virtual Point walkUncheck(Point p,float ang,float r) = 0;
};
}
#endif //USTC_CG_2025_FRAMEWORK2D_SCENE_H