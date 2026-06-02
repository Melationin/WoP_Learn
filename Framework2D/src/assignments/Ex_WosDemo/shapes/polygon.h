#pragma once

#include "shape.h"
#include <cmath>
#include <limits>
#include <utility>
#include <vector>

namespace USTC_CG
{
class Polygon : public Shape
{
   public:
    Polygon() = default;

    explicit Polygon(const std::vector<std::pair<float, float>>& pts)
        : points_(pts)
    {
    }

    void add_control_point(float x, float y) override
    {
        points_.emplace_back(x, y);
    }

    // 这里简单设为更新最后一个点；如果没有点则新增
    void update(float x, float y) override
    {
        if (points_.empty())
            points_.emplace_back(x, y);
        else
            points_.back() = { x, y };
    }

    // 将逻辑坐标 [-2,2]^2 映射到屏幕像素坐标 [0,w-1] x [0,h-1]
    std::pair<int, int> getCoord(float x, float y, int w, int h) override
    {
        int px = static_cast<int>((x + 2.0f) / 4.0f * (w - 1));
        int py = static_cast<int>((2.0f - y) / 4.0f * (h - 1));  // y轴翻转
        return { px, py };
    }

    // 将屏幕像素坐标还原到逻辑坐标 [-2,2]^2
    std::pair<float, float> getPos(int x, int y, int w, int h) override
    {
        float fx = -2.0f + 4.0f * static_cast<float>(x) / static_cast<float>(w - 1);
        float fy =  2.0f - 4.0f * static_cast<float>(y) / static_cast<float>(h - 1);
        return { fx, fy };
    }

    // 射线法判断点是否在多边形内部
    bool interior(float x, float y) override
    {
        int n = static_cast<int>(points_.size());
        if (n < 3) return false;

        bool inside = false;
        for (int i = 0, j = n - 1; i < n; j = i++)
        {
            const auto& pi = points_[i];
            const auto& pj = points_[j];

            bool intersect =
                ((pi.second > y) != (pj.second > y)) &&
                (x < (pj.first - pi.first) * (y - pi.second) / (pj.second - pi.second) + pi.first);

            if (intersect) inside = !inside;
        }
        return inside;
    }

    // 返回点(x,y)到多边形边界最近的边界点
    std::pair<float, float> distance_to_boundary(float x, float y) override
    {
        if (points_.empty()) return { x, y };
        if (points_.size() == 1) return points_[0];

        float best_dist2 = std::numeric_limits<float>::max();
        std::pair<float, float> best_point = points_[0];

        int n = static_cast<int>(points_.size());
        for (int i = 0; i < n; ++i)
        {
            const auto& a = points_[i];
            const auto& b = points_[(i + 1) % n];

            auto q = closest_point_on_segment(x, y, a.first, a.second, b.first, b.second);

            float dx = q.first - x;
            float dy = q.second - y;
            float dist2 = dx * dx + dy * dy;

            if (dist2 < best_dist2)
            {
                best_dist2 = dist2;
                best_point = q;
            }
        }

        return best_point;
    }

    const std::vector<std::pair<float, float>>& points() const
    {
        return points_;
    }

   private:
    std::vector<std::pair<float, float>> points_;

    static std::pair<float, float> closest_point_on_segment(
        float px, float py,
        float ax, float ay,
        float bx, float by)
    {
        float abx = bx - ax;
        float aby = by - ay;
        float apx = px - ax;
        float apy = py - ay;

        float ab_len2 = abx * abx + aby * aby;
        if (ab_len2 <= 1e-8f)
            return { ax, ay };

        float t = (apx * abx + apy * aby) / ab_len2;
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;

        return { ax + t * abx, ay + t * aby };
    }
};
}  // namespace USTC_CG