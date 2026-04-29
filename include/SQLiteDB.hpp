#ifndef SQLITE_DB_HPP
#define SQLITE_DB_HPP

#include <string>
#include <vector>
#include "CellData.hpp"

class SQLiteDB {
public:
    // 构造函数：传入数据库文件路径
    explicit SQLiteDB(const std::string& db_path);
    ~SQLiteDB();

    // 核心方法：执行SQL查询，返回单元数据列表
    std::vector<CellData> exec_query(const std::string& sql);

    // 检查数据库是否正常打开
    bool is_open() const;

private:
    struct sqlite3* db_ = nullptr;
    // SQLite回调函数，把查询结果转为CellData
    static int query_callback(void* data, int argc, char** argv, char** az_col_name);
};

#endif // SQLITE_DB_HPP