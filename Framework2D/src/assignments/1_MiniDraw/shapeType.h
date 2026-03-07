//
// Created by zhdds on 2026/3/7.
//

#ifndef USTC_CG_2025_FRAMEWORK2D_SHAPETYPE_H
#define USTC_CG_2025_FRAMEWORK2D_SHAPETYPE_H
namespace USTC_CG
{
enum ShapeType
{
    kDefault = 0,
    kLine = 1,
    kRect = 2,
    kEllipse = 3,
    kPolygon = 4,
    kFreehand = 5,
    kShape ,
    kSelector,
    kShapeConfigSetter,
    kShapeMover,

};

inline bool isShape(ShapeType type)
{
    switch (type)
    {
        case kLine:
        case kRect:
        case kEllipse:
        case kPolygon:
        case kFreehand:
            return true;
        default:
            return false;
    }
}

inline bool isSelector(ShapeType type)
{
    switch (type)
    {
        case kSelector:
        case kShapeConfigSetter:
            return true;
        default:
            return false;
    }
}
}
#endif //USTC_CG_2025_FRAMEWORK2D_SHAPETYPE_H