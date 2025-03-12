#pragma once

#include <cstring>
#include <fstream>
#include <map>
#include <string>
#include <string_view>
#include <algorithm>



inline std::map<std::string, std::string> mapping { 
    {".html", "text/html; charset=utf-8"}, 
    {".htm", "text/html; charset=utf-8"}, 
    {".jpg", "image/jpeg"}, 
    {".jepg", "image/jpeg"}, 
    {".gif", "image/gif"}, 
    {".png", "image/png"}, 
    {".css", "text/css"}, 
    {".au", "audio/basic"}, 
    {".wav", "audio/wav"}, 
    {".avi", "video/x-msvideo"}, 
    {".mov", "video/quicktime"}, 
    {".ico", "image/vnd.microsoft.icon"}, 
    {".mp3", "audio/mpeg"}, 
    {".mp4", "video/mp4"}, 
    {".doc", "application/msword"}, 
    {".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"}, 
    {".js", "application/javascript"}, 
    {".json", "application/json"}, 
    {".jsonp", "application/jsonp"}, 
    {".md", "application/x-genesis-rom"}, 
    {".pdf", "application/pdf"}, 
    {".xbl", "application/xml"}, 
    {".xml", "application/xml"}, 
    {".ttf", "application/x-font-ttf"}, 
    {".woff2", "font/woff2"}, 
    {".webp", "image/webp"}, 
};


inline std::string file_get_content(std::string const &path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
        // throw std::system_error(errno, std::generic_category());
    }
    std::string content{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
    return content;
}

inline void file_put_content(std::string const &path, std::string_view content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        throw std::system_error(errno, std::generic_category());
    }
    std::copy(content.begin(), content.end(), std::ostreambuf_iterator<char>(file));
}

inline std::string file_get_style(std::string url) {
    std::reverse(url.begin(), url.end());
    auto pos = url.find('.');
    auto style = url.substr(0, pos + 1);
    std::reverse(style.begin(), style.end());
    return style;
}

inline std::string file_get_webtype(std::string file) {
    auto style = file_get_style(file);
    auto it = mapping.find(style);
    if (it != mapping.end()) return it->second;
    else return "text/plain; charset=utf-8";
}

inline std::string file_get_route(std::string file) {
    return std::string(1, file[0]);
}

inline std::pair<std::string, std::string> extract_post(std::string const & body) {
    std::string key, value;
    auto pos = body.find("&");
    if (pos != std::string::npos) {
        auto left = body.substr(0, pos);
        auto right = body.substr(pos + 1);

        auto pos1 = left.find("=");
        key = left.substr(pos1 + 1);
        auto pos2 = right.find("=");
        value = right.substr(pos2 + 1);
    }
    return { key, value };
}
