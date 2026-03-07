//
// Created by zhdds on 2026/3/7.
//

#include "shape_setter.h"

namespace USTC_CG {
void ShapeSetter::update()
{
    Selector::update();
    for (const auto& it: selected_shapes_)
    {
        if (auto sp = it.lock()){
            sp->setConfig(config_);
        }
    }
}
} // USTC_CG