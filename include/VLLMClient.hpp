#ifndef VLLM_CLIENT_HPP
#define VLLM_CLIENT_HPP

#include <string>

// // ！！！本地调试时打开MOCK_MODE，租卡后注释掉这行即可！！！
// #define MOCK_MODE 1

class VLLMClient {
public:
    // 构造函数：传入云服务的API地址
    explicit VLLMClient(const std::string& api_url);
    ~VLLMClient();

    // 核心方法：传入自然语言，返回SQL语句
    std::string get_sql_from_nl(const std::string& user_input);

private:
    std::string api_url_;
    // 真实HTTP请求实现（MOCK_MODE关闭时启用）
    std::string send_http_request(const std::string& post_data);
    // Mock实现（本地调试用，直接返回固定SQL）
    std::string mock_get_sql(const std::string& user_input);
};

#endif // VLLM_CLIENT_HPP