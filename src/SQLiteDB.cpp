#include "SQLiteDB.hpp"
#include <iostream>
#include <sqlite3.h>

SQLiteDB::SQLiteDB(const std::string& db_path) {
    int rc = sqlite3_open(db_path.c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::cerr << "打开数据库失败：" << sqlite3_errmsg(db_) << std::endl;
        sqlite3_close(db_);
        db_ = nullptr;
    } else {
        std::cout << "✅ 数据库打开成功" << std::endl;
    }
}

SQLiteDB::~SQLiteDB() {
    if (db_) {
        sqlite3_close(db_);
    }
}

bool SQLiteDB::is_open() const {
    return db_ != nullptr;
}

int SQLiteDB::query_callback(void* data, int argc, char** argv, char** az_col_name) {
    auto* result_list = static_cast<std::vector<CellData>*>(data);
    CellData cell;

    for (int i = 0; i < argc; i++) {
        std::string col_name = az_col_name[i];
        char* value = argv[i];
        if (value == nullptr) continue;

        if (col_name == "id") cell.id = std::stoi(value);
        else if (col_name == "cell_name") cell.cell_name = value;
        else if (col_name == "function_type") cell.function_type = value;
        else if (col_name == "drive_strength") cell.drive_strength = value;
        else if (col_name == "drive_strength_num") cell.drive_strength_num = std::stoi(value);
        else if (col_name == "leakage_power") cell.leakage_power = std::stod(value);
        else if (col_name == "dynamic_power") cell.dynamic_power = std::stod(value);
        else if (col_name == "area") cell.area = std::stod(value);
        else if (col_name == "intrinsic_delay") cell.intrinsic_delay = std::stod(value);
    }

    result_list->push_back(cell);
    return 0;
}

std::vector<CellData> SQLiteDB::exec_query(const std::string& sql) {
    std::vector<CellData> result;
    if (!db_) {
        std::cerr << "数据库未打开" << std::endl;
        return result;
    }

    char* err_msg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), query_callback, &result, &err_msg);

    if (rc != SQLITE_OK) {
        std::cerr << "执行SQL失败：" << err_msg << std::endl;
        sqlite3_free(err_msg);
    } else {
        std::cout << "✅ SQL执行成功，共查询到 " << result.size() << " 条数据" << std::endl;
    }

    return result;
}