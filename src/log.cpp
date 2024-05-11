
#include "log.h"

mutex Logger::mtx;

Logger::Logger()
{
}

void Logger::flush_buffer()
{
    for (auto& log : buffer)
    {
        logfile << log << endl;
    }
    buffer.clear();
}

void Logger::period_flush()
{
    while (true)
    {
        this_thread::sleep_for(flushInterval);
        flush_buffer();
    }
}

void Logger::log(LogLevel level, const std::string& file, const int& line, const string& function, const char* format, ...) {
    lock_guard<mutex> lck(file_mtx);
    auto now = chrono::system_clock::now();
    auto now_ms = chrono::time_point_cast<chrono::milliseconds>(now);
    auto epoch = now_ms.time_since_epoch();
    auto value = chrono::duration_cast<chrono::milliseconds>(epoch);
    long duration = value.count();
    time_t t = duration/1000;
    tm localTime = *std::localtime(&t);
    stringstream ss;
    ss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << '.' << std::setw(3) << std::setfill('0') << duration % 1000;
    string time_stamp = ss.str();

    string levelStr;
    switch (level) {
    case LogLevel::INFO:
        levelStr = "INFO";
        break;
    case LogLevel::WARNING:
        levelStr = "WARNING";
        break;
    case LogLevel::ERROR0:
        levelStr = "ERROR";
        break;
    }
    va_list args;
    va_start(args, format);
    char message[1024];
    vsprintf(message, format, args);
    va_end(args);


    ostringstream oss;
    oss << time_stamp << " [" << levelStr << "] " << file << ":" << line << " (" << function << "): " << message << std::endl;
    string logMessage = oss.str();
    cout << logMessage;
    if (buffer.size() >= maxBufferSize)
    {
        flush_buffer();
    }
    buffer.emplace_back(logMessage);
    //logfile << logMessage<<endl; // 输出到日志文件


}

Logger* Logger::get_instance()
{
    lock_guard<std::mutex> lck(mtx);
    static Logger ins;
    return &ins;
}

void Logger::init()
{
    logfile.open(PATH, ios_base::app);
    buffer.reserve(maxBufferSize);
    if(!logfile.is_open())
    {
        cout<<"logfile open faild"<<endl;
        exit(1);
    }
    // 创建一个新的线程并启动period_flush()函数
    std::thread flush_thread(&Logger::period_flush, this);
    flush_thread.detach();  // 将线程与Logger实例解绑，使其在后台运行
}
