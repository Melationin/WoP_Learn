//
// Created by zhdds on 2026/3/20.
//

#include "rect.h"
#include <algorithm>

#include "util.h"

namespace USTC_CG {
//static Counter distance_to_boundary_count{"distance_to_boundary"};
std::pair<float, float> Rect::distance_to_boundary(float x, float y)
{

    //std::this_thread::sleep_for(std::chrono::duration<long long, std::micro>(100)); // 模拟耗时操作

    //distance_to_boundary_count++;
    if (x1 <= x && x <= x2 && y1 <= y && y <= y2)
    {
        float dx = std::min(x - x1, x2 - x);
        float dy = std::min(y - y1, y2 - y);
        if (dx < dy)
        {
            return {x < (x1 + x2) / 2 ? x1 : x2, y};
        }
        else
        {
            return {x, y < (y1 + y2) / 2 ? y1 : y2};
        }
    }
    else
    {
        float dx = std::max({x1 - x, 0.0f, x - x2});
        float dy = std::max({y1 - y, 0.0f, y - y2});
        if (dx > dy)
        {
            return {x < (x1 + x2) / 2 ? x1 : x2, y};
        }
        else
        {
            return {x, y < (y1 + y2) / 2 ? y1 : y2};
        }
    }

}

} // USTC_Cg