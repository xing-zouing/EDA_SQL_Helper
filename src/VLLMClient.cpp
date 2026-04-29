#include "VLLMClient.hpp"
#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// 回调函数：接收curl返回的响应数据
static size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t new_length = size * nmemb;
    try {
        s->append((char*)contents, new_length);
    } catch (std::bad_alloc& e) {
        return 0;
    }
    return new_length;
}

VLLMClient::VLLMClient(const std::string& api_url) : api_url_(api_url) {
    curl_global_init(CURL_GLOBAL_ALL);
}

VLLMClient::~VLLMClient() {
    curl_global_cleanup();
}

std::string VLLMClient::get_sql_from_nl(const std::string& user_input) {
#ifdef MOCK_MODE
    std::cout << "[MOCK模式] 本地模拟生成SQL，无需调用云服务" << std::endl;
    return mock_get_sql(user_input);
#else
    // 构造请求体（和vLLM OpenAI接口完全兼容）
    json request_body;
    request_body["model"] = "eda_sql";
    request_body["temperature"] = 0.01; // 极低温度，保证输出稳定
    request_body["max_tokens"] = 200;
    // 系统提示词，和微调时完全一致
    std::string system_prompt = R"(你是EDA标准单元库SQL查询助手，只输出可直接执行的SQL语句，不要任何解释、备注、多余内容。
表名：standard_cells
字段说明：
- cell_name: 单元名称
- function_type: 单元功能类型，可选值：INV(反相器), NAND2(二输入与非门), NOR2(二输入或非门), AND2(二输入与门), OR2(二输入或门), DFF(D触发器), BUF(缓冲器)
- drive_strength_num: 驱动强度数字，用于大小比较，对应X1=1,X2=2,X4=4,X8=8,X16=16,X32=32
- leakage_power: 漏电功耗，单位nW
- dynamic_power: 动态功耗，单位uW/MHz
- area: 单元面积，单位um²
- intrinsic_delay: 本征延时，单位ps
严格按照用户需求生成SQL，禁止输出任何SQL之外的内容。)";
    request_body["messages"] = {
        {{"role", "system"}, {"content", system_prompt}},
        {{"role", "user"}, {"content", user_input}}
    };

    std::string post_data = request_body.dump();
    std::string response = send_http_request(post_data);

    // 解析响应，提取SQL
    try {
        json res_json = json::parse(response);
        std::string sql = res_json["choices"][0]["message"]["content"];
        // 清理SQL前后的换行、空格、markdown符号
        size_t start = sql.find("SELECT");
        size_t end = sql.find(";");
        if (start != std::string::npos && end != std::string::npos) {
            sql = sql.substr(start, end - start + 1);
        }
        return sql;
    } catch (const std::exception& e) {
        std::cerr << "解析响应失败：" << e.what() << std::endl;
        return "";
    }
#endif
}

std::string VLLMClient::send_http_request(const std::string& post_data) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (!curl) {
        std::cerr << "curl初始化失败" << std::endl;
        return "";
    }

    // 设置curl选项
    curl_easy_setopt(curl, CURLOPT_URL, api_url_.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // 设置请求头
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // 执行请求
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "请求失败：" << curl_easy_strerror(res) << std::endl;
    }

    // 清理
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return response;
}

// Mock实现：本地调试用，根据用户输入返回对应SQL，模拟模型效果
std::string VLLMClient::mock_get_sql(const std::string& user_input) {
    // 你可以在这里添加更多测试用例，覆盖你常用的查询
    if (user_input.find("反相器") != std::string::npos && user_input.find("驱动强度大于X4") != std::string::npos) {
        return "SELECT * FROM standard_cells WHERE function_type = 'INV' AND drive_strength_num > 4;";
    } else if (user_input.find("X2驱动") != std::string::npos && user_input.find("与非门") != std::string::npos) {
        return "SELECT * FROM standard_cells WHERE function_type = 'NAND2' AND drive_strength_num = 2 ORDER BY area ASC;";
    } else if (user_input.find("D触发器") != std::string::npos) {
        return "SELECT * FROM standard_cells WHERE function_type = 'DFF' ORDER BY intrinsic_delay ASC;";
    } else {
        // 默认返回全量查询
        return "SELECT * FROM standard_cells LIMIT 10;";
    }
}