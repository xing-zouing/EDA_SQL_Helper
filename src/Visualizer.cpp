#include "Visualizer.hpp"
#include <matplot/matplot.h>
#include <iostream>

namespace plt = matplot;

void Visualizer::plot_power_compare(const std::vector<CellData>& data, const std::string& save_path) {
    if (data.empty()) {
        std::cerr << "无数据，无法绘图" << std::endl;
        return;
    }
    if (data.size() < 2) {
        std::cout << "⚠️ 数据不足2条，跳过功耗对比图绘制" << std::endl;
        return;
    }

    // 准备绘图数据
    std::vector<double> x;
    std::vector<double> dynamic_power;
    std::vector<double> leakage_power;
    std::vector<std::string> labels;

    for (int i = 0; i < data.size(); i++) {
        x.push_back(i);
        labels.push_back(data[i].cell_name);
        dynamic_power.push_back(data[i].dynamic_power);
        leakage_power.push_back(data[i].leakage_power);
    }

    // 绘制分组柱状图
    auto fig = plt::figure();
    fig->size(1200, 600);
    auto p1 = plt::bar(x, dynamic_power);
    p1->face_color({0.2f, 0.5f, 0.9f});
    p1->display_name("动态功耗 (uW/MHz)");

    plt::hold(plt::on);
    auto p2 = plt::bar(x, leakage_power);
    p2->face_color({0.9f, 0.3f, 0.3f});
    p2->display_name("漏电功耗 (nW)");

    // 设置图表样式
    plt::xticks(x);
    plt::xticklabels(labels);
    plt::xlabel("标准单元名称");
    plt::ylabel("功耗值");
    plt::title("标准单元功耗对比");
    plt::legend();
    plt::grid(plt::on);

    // 保存图片
    plt::save(save_path);
    std::cout << "✅ 功耗对比图已保存至：" << save_path << std::endl;
    plt::hold(plt::off);
}

void Visualizer::plot_delay_compare(const std::vector<CellData>& data, const std::string& save_path) {
    if (data.empty()) {
        std::cerr << "无数据，无法绘图" << std::endl;
        return;
    }
    if (data.size() < 2) {
        std::cout << "⚠️ 数据不足2条，跳过功耗对比图绘制" << std::endl;
        return;
    }

    std::vector<double> x;
    std::vector<double> delay;
    std::vector<std::string> labels;

    for (int i = 0; i < data.size(); i++) {
        x.push_back(i);
        labels.push_back(data[i].cell_name);
        delay.push_back(data[i].intrinsic_delay);
    }

    auto fig = plt::figure();
    fig->size(1000, 600);
    auto p = plt::bar(x, delay);
    p->face_color({0.3f, 0.8f, 0.5f});

    plt::xticks(x);
    plt::xticklabels(labels);
    plt::xlabel("标准单元名称");
    plt::ylabel("本征延时 (ps)");
    plt::title("标准单元本征延时对比");
    plt::grid(plt::on);

    plt::save(save_path);
    std::cout << "✅ 延时对比图已保存至：" << save_path << std::endl;
}

void Visualizer::plot_area_compare(const std::vector<CellData>& data, const std::string& save_path) {
    if (data.empty()) {
        std::cerr << "无数据，无法绘图" << std::endl;
        return;
    }
    if (data.size() < 2) {
        std::cout << "⚠️ 数据不足2条，跳过功耗对比图绘制" << std::endl;
        return;
    }

    std::vector<double> x;
    std::vector<double> area;
    std::vector<std::string> labels;

    for (int i = 0; i < data.size(); i++) {
        x.push_back(i);
        labels.push_back(data[i].cell_name);
        area.push_back(data[i].area);
    }

    auto fig = plt::figure();
    fig->size(1000, 600);
    auto p = plt::bar(x, area);
    p->face_color({0.7f, 0.4f, 0.9f});

    plt::xticks(x);
    plt::xticklabels(labels);
    plt::xlabel("标准单元名称");
    plt::ylabel("单元面积 (um²)");
    plt::title("标准单元面积对比");
    plt::grid(plt::on);

    plt::save(save_path);
    std::cout << "✅ 面积对比图已保存至：" << save_path << std::endl;
}