#ifndef CELL_DATA_HPP
#define CELL_DATA_HPP

#include <string>

// 标准单元数据结构体，和数据库字段一一对应
struct CellData {
    int id;
    std::string cell_name;
    std::string function_type;
    std::string drive_strength;
    int drive_strength_num;
    double leakage_power;
    double dynamic_power;
    double area;
    double intrinsic_delay;
};

#endif // CELL_DATA_HPP