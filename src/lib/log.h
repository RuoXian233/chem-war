#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <format>
#include <chrono>
#include "utils.hpp"

#define GET_SOURCE_INFO() \
    engine::SourceInfo { __FILE__, __FUNCTION__, __LINE__ }

static const char *LOG_LEVEL_STRING[] = { 
    "DEBUG", "INFO", "WARNING", "ERROR", "FATAL" 
};

#define PREFER_LOGGER_REF
#define DEFAULT_LOGGER_NAME logger

#ifdef PREFER_LOGGER_REF
    #define _LOG_IMPL(logger, level, message) (logger).Log(level, message, GET_SOURCE_INFO())
    #define _LOG_IMPL_F(logger, level, fmt, ...) (logger).Log(level, std::format(fmt, __VA_ARGS__), GET_SOURCE_INFO())
#else
    #define _LOG_IMPL(logger, level, message) (logger)->Log(level, message, GET_SOURCE_INFO())
    #define _LOG_IMPL_F(logger, level, fmt, ...) (logger)->Log(level, std::format(fmt, __VA_ARGS__), GET_SOURCE_INFO())
#endif

#define LOG(level, message) _LOG_IMPL(DEFAULT_LOGGER_NAME, level, message)
#define LOG_F(level, fmt, ...) _LOG_IMPL_F(DEFAULT_LOGGER_NAME, level, fmt, __VA_ARGS__)
#define INFO(message) LOG(engine::Logger::Level::Info, message)
#define INFO_F(fmt, ...) LOG_F(engine::Logger::Level::Info, fmt, __VA_ARGS__)
#define DEBUG(message) LOG(engine::Logger::Level::Debug, message)
#define DEBUG_F(fmt, ...) LOG_F(engine::Logger::Level::Debug, fmt, __VA_ARGS__)
#define WARNING(message) LOG(engine::Logger::Level::Warning, message)
#define WARNING_F(fmt, ...) LOG_F(engine::Logger::Level::Warning, fmt, __VA_ARGS__)
#define ERROR(message) LOG(engine::Logger::Level::Error, message)
#define ERROR_F(fmt, ...) LOG_F(engine::Logger::Level::Error, fmt, __VA_ARGS__)
#define FATAL(message) LOG(engine::Logger::Level::Fatal, message)
#define FATAL_F(fmt, ...) LOG_F(engine::Logger::Level::Fatal, fmt, __VA_ARGS__)


#define RESET "\033[0m"
#define BLACK "\033[30m"  /* Black */
#define RED "\033[31m"    /* Red */
#define GREEN "\033[32m"  /* Green */
#define YELLOW "\033[33m" /* Yellow */
#define BLUE "\033[34m"   /* Blue */
#define PURPLE "\033[35m" /* Purple */
#define CYAN "\033[36m"   /* Cyan */
#define WHITE "\033[37m"  /* White */
#define BOLD_WHITE "\033[1;37m"
#define BOLD_RED "\033[1;31m"
#define BOLD_GREEN "\033[1;32m"
#define BOLD_YELLOW "\033[1;33m"
#define BOLD_BLUE "\033[1;34m"
#define BOLD_PURPLE "\033[1;35m"
#define BOLD_CYAN "\033[1;36m"

static const char *DEFAULT_MSG_COLOR[] = { CYAN, RESET, GREEN, RED, BOLD_RED };


namespace engine {
    struct SourceInfo {
        std::string file;
        std::string func;
        int line;
    };

    class Logger final {
    public:

        enum class Level {
            Debug = 0,
            Info = 1,
            Warning = 2,
            Error = 3,
            Fatal = 4
        };

        inline friend bool operator>=(Level l1, Level l2) {
            return static_cast<int>(l1) >= static_cast<int>(l2);
        }

        inline static std::string ParseFilename(const std::string &filePath) {
            return StringSplit(filePath, "/").back();
        }

        inline friend bool operator<=(Level l1, Level l2) {
            return static_cast<int>(l1) <= static_cast<int>(l2);
        }

        static inline std::string GetLevelString(Level level) {
            return LOG_LEVEL_STRING[static_cast<int>(level)];
        }

        static inline std::string GetColor(Level level) {
            return DEFAULT_MSG_COLOR[static_cast<int>(level)];
        }

        Logger(const std::string &moduleName);
        void AttachFile(const std::string &filePath, Level level);
        void RedirectOutput(std::ostream *stream);
        void RedirectError(std::ostream *stream);
        void SetDisplayLevel(Level level);
        void StartParagraph(Level level);
        void EndParagraph();
        void Log(Level level, const std::string &message, const SourceInfo &sourceInfo);
        static std::string GetFormattedTimeStamp();

        ~Logger();
        void Write(Level level, const std::string &message);

    private:
        int termWidth = -1;
        std::ostream *outputStream = &std::cout;
        std::ostream *errorStream = &std::cerr;
        const std::string TAB = "    ";
        std::string SEP = "          ";
        Level currentParagraphLevel;

        std::fstream fileStream;
        bool isInParagraph = false;
        std::string moduleName;
        Level displayLevel = Level::Info;
    };
}
