#ifndef VISUALIZER_HPP
#define VISUALIZER_HPP

#include <vector>
#include "CellData.hpp"

class Visualizer {
public:
    // 绘制功耗对比柱状图
    static void plot_power_compare(const std::vector<CellData>& data, const std::string& save_path = "power_compare.png");

    // 绘制延时对比柱状图
    static void plot_delay_compare(const std::vector<CellData>& data, const std::string& save_path = "delay_compare.png");

    // 绘制面积对比柱状图
    static void plot_area_compare(const std::vector<CellData>& data, const std::string& save_path = "area_compare.png");
};

#endif // VISUALIZER_HPP