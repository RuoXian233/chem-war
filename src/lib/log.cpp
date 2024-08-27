#include "log.h"

#ifdef __linux__
    #include <sys/ioctl.h>
    #include <unistd.h>
    #include <termios.h>
#else
    #include <conio.h>
#endif

using namespace engine;


Logger::Logger(const std::string &moduleName) : moduleName(moduleName) {
    #ifdef __linux__
        winsize w;
        ioctl(STDIN_FILENO, TIOCGWINSZ, &w);
        this->termWidth = w.ws_col;
    #else
        ;
    #endif
}

void Logger::AttachFile(const std::string &filePath, Level level) {}

void Logger::RedirectOutput(std::ostream *stream) {
    this->outputStream = stream;
}

void Logger::RedirectError(std::ostream *stream) {
    this->errorStream = stream;
}

void Logger::SetDisplayLevel(Level level) {
    this->displayLevel = level;
}


void Logger::StartParagraph(Level level) {
    if (!this->isInParagraph) {
        this->isInParagraph = true;

        this->Write(level, std::format(
        "[{}{}{} {}{}{}]",
        YELLOW, this->moduleName, RESET,
        BOLD_GREEN, GetFormattedTimeStamp(), RESET 
        ));
        this->currentParagraphLevel = level;
    }
}

void Logger::EndParagraph() {
    if (this->isInParagraph) {
        this->isInParagraph = false;
    
        this->Write(this->currentParagraphLevel, std::format(
            "[{}{}{} {}{}{}]",
            YELLOW, this->moduleName, RESET,
            BOLD_GREEN, GetFormattedTimeStamp(), RESET 
        ));
        this->currentParagraphLevel = Level::Info;
    }
}

void Logger::Log(Level level, const std::string &message, const SourceInfo &sourceInfo) {
    auto fileInfo = std::format(
        "{}<{}:{}>{}",
        BOLD_WHITE, ParseFilename(sourceInfo.file), sourceInfo.line, RESET
    );

    auto rawFileInfo = std::format(
        "<{}:{}>",
        ParseFilename(sourceInfo.file), sourceInfo.line
    );

    if (this->isInParagraph) {
        auto rawText = std::format(
            "{}: [{}] {} {}",
            TAB, sourceInfo.func, GetLevelString(level), message
        );

        auto text = std::format(
            "{}{}{}: {}[{}] {}{}",
            TAB, CYAN, sourceInfo.func, 
            GetColor(level), GetLevelString(level), message, RESET
        );
        std::string sep;
        if (this->termWidth > 0) {
            int sepLength = this->termWidth - rawText.size() - rawFileInfo.size() - 5;
            if (sepLength <= 0) {
                sep = " | ";
            } else {
                sep = std::string(sepLength, ' ');
            }
        } else {
            sep = SEP;
        }

        this->Write(this->currentParagraphLevel, std::format(
            "{}{}{}",
            text, sep, fileInfo
        ));
    } else {
        auto rawText = std::format(
            "{} [{}::{}] [{}] {}",
            GetFormattedTimeStamp(),
            this->moduleName, sourceInfo.func,
            GetLevelString(level), message
        );

        auto text = std::format(
            "{}{}{} {}[{}::{}]{} {}[{}] {}{}",
            BOLD_GREEN, GetFormattedTimeStamp(), RESET,
            YELLOW, this->moduleName, sourceInfo.func, RESET,
            GetColor(level), GetLevelString(level), message, RESET
        );

        std::string sep;
        if (this->termWidth > 0) {
            int sepLength = this->termWidth - rawText.size() - rawFileInfo.size() - 1;
            
            if (sepLength <= 0) {
                sep = " | ";
            } else {
                sep = std::string(sepLength, ' ');
            }
        } else {
            sep = SEP;
        }

        this->Write(level, std::format(
            "{}{}{}",
            text, sep, fileInfo
        ));
    }
}


std::string Logger::GetFormattedTimeStamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    return ss.str();
}

Logger::~Logger() {}

void Logger::Write(Level level, const std::string &message) {
    auto stream = level >= Level::Warning ? this->errorStream : this->outputStream;
    if (this->displayLevel <= level) {
        (*stream) << message << std::endl;
    }
    // TODO: File logging
}
