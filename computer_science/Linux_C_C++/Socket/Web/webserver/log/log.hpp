#ifndef LOG_HPP
#define LOG_HPP


#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <mutex>
#include <ctime>
#include <cmath>


namespace web {
std::mutex log_mutex;
class log {
public:
    log();
    enum log_level { DEBUG, INFO, WARNING, ERROR, FATAL };

    static log_level debug;
    static log_level info;
    static log_level warning;
    static log_level error;
    static log_level fatal;
    void write(log_level level, std::string msg);
private:
    std::ofstream log_file;
    std::string file_path;
};

log::log_level log::debug = log::log_level::DEBUG;
log::log_level log::info = log::log_level::INFO;
log::log_level log::warning = log::log_level::WARNING;
log::log_level log::error = log::log_level::ERROR;
log::log_level log::fatal = log::log_level::FATAL;

log::log() {
    file_path = "../info.log";
}

void log::write(log_level level, std::string msg) {
    std::lock_guard<std::mutex> lock(log_mutex);
    log_file.open(file_path, std::ios::out | std::ios::app);

    if (log_file.is_open()) {
        auto now = std::chrono::system_clock::now();
        auto now_in_milliseconds = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
        time_t now_c = std::chrono::system_clock::to_time_t(std::chrono::time_point_cast<std::chrono::system_clock::duration>(now_in_milliseconds));
        auto ms = now_in_milliseconds.time_since_epoch() - std::chrono::duration_cast<std::chrono::seconds>(now_in_milliseconds.time_since_epoch());


        tm timeinfo;
#ifdef _MSC_VER
        localtime_s(&timeinfo, &now_c);
#else
        localtime_r(&now_c, &timeinfo);
#endif


        log_file << timeinfo.tm_year + 1900 << "-"
             << timeinfo.tm_mon + 1 << "-"
             << timeinfo.tm_mday << " "
             << timeinfo.tm_hour << ":"
             << timeinfo.tm_min << ":"
             << timeinfo.tm_sec << "."
             << ms.count() % 1000 << " ";

        if (level == DEBUG)
            log_file << "[DEBUG]: ";
        if (level == WARNING)
            log_file << "[WARNING]: ";
        if (level == ERROR)
            log_file << "[ERROR]: ";
        if (level == FATAL)
            log_file << "[FATAL]: ";
        if (level == INFO)
            log_file << "[INFO]: ";
        log_file << msg << std::endl;
        log_file.close();
    }
}

}
#endif
