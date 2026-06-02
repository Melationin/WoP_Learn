#pragma once

#include <vector>

namespace USTC_CG
{
class Shape
{
   public:
    // Draw Settings
    struct Config
    {
        // Offset to convert canvas position to screen position
        float bias[2] = { 0.f, 0.f };
        // Line color in RGBA format
        unsigned char line_color[4] = { 255, 0, 0, 255 };
        float line_thickness = 2.0f;
    };

   public:
    virtual ~Shape() = default;


    virtual void draw(const Config&) const{};


    virtual void update(float x, float y){};

    virtual void add_control_point(float , float ) {}


    virtual  std::pair<int,int> getCoord(float x,float y,int w,int h) = 0;
    virtual   std::pair<float,float> getPos(int x, int y, int w, int h) = 0;
    virtual bool interior(float x, float y) = 0;


    //正数表示内部，负数表示外面
    virtual std::pair<float,float> distance_to_boundary(float x, float y) = 0;
};
}  // namespace USTC_CG