#include <iostream>
#include <string>
#include <curl/curl.h> // Thư viện quan trọng nhất

// Hàm callback để hứng dữ liệu trả về từ Server (Ollama)
// Vì C++ không tự biết cách lưu data tải về, mình phải định nghĩa hàm này
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main() {
    // 1. Khởi tạo CURL
    CURL* curl;
    CURLcode res;
    std::string readBuffer; // Biến này sẽ chứa câu trả lời của AI

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl) {
        // =================================================================
        // THAY LINK NGROK CỦA BẠN VÀO ĐÂY
        // Ví dụ: "https://abc12345.ngrok-free.app/api/generate"
        // =================================================================
        std::string url = "https://unslaked-unblocked-josette.ngrok-free.dev/api/generate";

        // 2. Chuẩn bị dữ liệu JSON để gửi đi (Hard-code cho phần POC)
        // Lưu ý: "model" phải đúng tên model đang chạy trên Kaggle (gpt-oss:20b)
        std::string json_data = "{"
                                "\"model\": \"gpt-oss:20b\","
                                "\"prompt\": \"Hello, tell me a short joke within 1 sentence.\","
                                "\"stream\": false" 
                                "}";

        // 3. Cấu hình HTTP Request
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);             // Báo là method POST
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str()); // Đính kèm cục JSON
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);  // Đính kèm Header
        
        // Cấu hình hàm hứng kết quả trả về
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        // 4. Thực hiện gửi đi!
        std::cout << "Dang gui tin hieu toi Ollama (qua Ngrok)..." << std::endl;
        res = curl_easy_perform(curl);

        // 5. Kiểm tra kết quả
        if(res != CURLE_OK) {
            std::cerr << "Loi ket noi: " << curl_easy_strerror(res) << std::endl;
        } else {
            // In kết quả thô (Raw JSON) ra màn hình
            std::cout << "--- KET QUA TU AI ---" << std::endl;
            std::cout << readBuffer << std::endl;
            std::cout << "---------------------" << std::endl;
            std::cout << "POC THANH CONG! (+0.1 diem)" << std::endl;
        }

        // 6. Dọn dẹp bộ nhớ
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return 0;
}