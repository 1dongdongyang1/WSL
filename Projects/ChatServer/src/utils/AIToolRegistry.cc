#include "utils/AIToolRegistry.h"
#include <curl/curl.h>

AIToolRegistry::AIToolRegistry() {
    registerTool("get_weather", getWeather);
    registerTool("get_time", getTime);
}

void AIToolRegistry::registerTool(const std::string& name, ToolFunc func) {
    tools_[name] = func;
}

bool AIToolRegistry::hasTool(const std::string& name) const {
    return tools_.find(name) != tools_.end();
}

json AIToolRegistry::callTool(const std::string& name, const json& args) const {
    auto it = tools_.find(name);
    if (it != tools_.end()) {
        return it->second(args);
    }
    throw std::runtime_error("Tool not found: " + name);
}

json AIToolRegistry::getWeather(const json& args) {
    if(!args.contains("city")) {
        return json{{"error", "Missing 'city' argument"}};
    }
    std::string city = args["city"].get<std::string>();
    std::string encodedCity;

    char* curlEncoded = curl_easy_escape(nullptr, city.c_str(), city.length());
    if(curlEncoded) {
        encodedCity = std::string(curlEncoded);
        curl_free(curlEncoded);
    } else {
        return json{{"error", "Failed to encode city name"}};
    }

    std::string url = "https://wttr.in/" + encodedCity + "?format=3&lang=zh";
    std::string responseData;

    CURL* curl = curl_easy_init();
    if (!curl) {
        return json{ {"error", "Failed to initialize CURL"} };
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void* contents, size_t size, size_t nmemb, void* userp) -> size_t {
        size_t totalSize = size * nmemb;
        std::string* str = static_cast<std::string*>(userp);
        str->append(static_cast<char*>(contents), totalSize);
        return totalSize;
    });
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        return json{ {"error", "Failed to fetch weather data"} };
    }

    return json{ {"city", city}, { "weather", responseData } };
}

json AIToolRegistry::getTime(const json& args) {
    (void)args;
    std::time_t t = std::time(nullptr);
    std::tm* tm_ptr = std::localtime(&t);
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_ptr);
    return json{ {"time", std::string(buffer)} };
}
