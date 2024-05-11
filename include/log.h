#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <mutex>
#include <thread>
#include <atomic>
#include <cstdarg>
#include<vector>
#include <chrono>
#include <iomanip>
#define PATH "/home/hulong/vscode/override_server/log/server.log"
using namespace std;
#define LOG(format, ...) Logger::get_instance()->log(LogLevel::INFO, __FILE__, __LINE__, __func__,format, ##__VA_ARGS__)
#define LOGERROR(format,...) Logger::get_instance()->log(LogLevel::ERROR0, __FILE__, __LINE__, __func__,format, ##__VA_ARGS__)

enum class LogLevel {
    INFO,
    WARNING,
    ERROR0
};

class Logger {

private:
    mutex file_mtx;
    static mutex mtx;
    ofstream logfile;
    vector<string> buffer;
    const int maxBufferSize = 100; // 最大缓冲区大小
    const std::chrono::seconds flushInterval{ 1 }; // 定期刷新缓冲区的时间间隔
    Logger();
    void flush_buffer();
    void period_flush();
public:
    Logger(const Logger& logger) = delete;
    Logger& operator=(const Logger& logger) = delete;
    void log(LogLevel level, const string& file, const int& line, const string& function, const char* format, ...);
    static Logger* get_instance();
    void init();
};


