#include <iostream>
#include <string>
#include "VLLMClient.hpp"
#include "SQLiteDB.hpp"
#include "Visualizer.hpp"
#include "CellData.hpp"

// 打印查询结果
void print_result(const std::vector<CellData>& data) {
    if (data.empty()) {
        std::cout << "\n❌ 未查询到符合条件的单元" << std::endl;
        return;
    }

    // 打印表头
    std::cout << "\n==================== 查询结果 ====================" << std::endl;
    printf("%-15s %-10s %-8s %-12s %-12s %-8s %-10s\n",
           "单元名称", "功能类型", "驱动强度", "动态功耗(uW)", "漏电功耗(nW)", "面积(um²)", "延时(ps)");
    std::cout << "-------------------------------------------------------------------------------------------------" << std::endl;

    // 打印每一行数据
    for (const auto& cell : data) {
        printf("%-15s %-10s %-8s %-12.4f %-12.4f %-8.4f %-10.4f\n",
               cell.cell_name.c_str(),
               cell.function_type.c_str(),
               cell.drive_strength.c_str(),
               cell.dynamic_power,
               cell.leakage_power,
               cell.area,
               cell.intrinsic_delay);
    }
    std::cout << "===================================================" << std::endl;
}

int main() {
    std::cout << "=============================================" << std::endl;
    std::cout << "  标准单元库参数速查与对比助手" << std::endl;
    std::cout << "=============================================" << std::endl;

    // -------------------------- 1. 初始化组件 --------------------------
    // 数据库初始化（路径不要错！）
    SQLiteDB db("./data/standard_cells.db");
    if (!db.is_open()) {
        std::cerr << "数据库初始化失败，程序退出" << std::endl;
        return -1;
    }

    // vLLM客户端初始化（租卡后改成你的云服务器IP:端口）
    VLLMClient client("http://127.0.0.1:8000/v1/chat/completions");

    // -------------------------- 2. 主交互循环 --------------------------
    std::string user_input;
    while (true) {
        std::cout << "\n请输入查询语句（输入exit退出）：" << std::endl;
        std::cout << "> ";
        std::getline(std::cin, user_input);

        // 退出程序
        if (user_input == "exit" || user_input == "quit") {
            std::cout << "程序退出" << std::endl;
            break;
        }

        // 空输入跳过
        if (user_input.empty()) {
            continue;
        }

        // -------------------------- 3. 核心流程 --------------------------
        // 步骤1：自然语言转SQL
        std::cout << "\n[步骤1] 正在生成SQL语句..." << std::endl;
        std::string sql = client.get_sql_from_nl(user_input);
        if (sql.empty()) {
            std::cerr << "生成SQL失败，请重新输入" << std::endl;
            continue;
        }
        std::cout << "生成的SQL：" << sql << std::endl;

        // 步骤2：执行SQL查询
        std::cout << "\n[步骤2] 正在执行查询..." << std::endl;
        std::vector<CellData> result = db.exec_query(sql);
        if (result.empty()) {
            continue;
        }

        // 步骤3：打印结果
        print_result(result);

        // 步骤4：生成可视化图表
        std::cout << "\n[步骤3] 正在生成可视化图表..." << std::endl;
        Visualizer::plot_power_compare(result);
        Visualizer::plot_delay_compare(result);
        Visualizer::plot_area_compare(result);
    }

    return 0;
}