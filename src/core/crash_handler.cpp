// This file is subject to the terms and conditions defined in 'LICENSE' in the source code package

#include "core/crash_handler.h"

#include <csignal>
#include <cstdio>
#include <ios>
#include <sstream>

#include "core/filesystem.h"
#include "core/logger.h"

// Logs message to stderr and file
#define CRASH_LOG_MESSAGE(format_, ...)        \
    do {                                       \
        fprintf(file, format_, __VA_ARGS__);   \
        fprintf(stderr, format_, __VA_ARGS__); \
    } while (0)

// Close ofstream and open as FILE*
#define CRASH_OPEN_LOG_FILE()                                                                   \
    hyperion::core::CloseLogFile();                                                             \
    auto* file = fopen(hyperion::core::ResolvePath(hyperion::core::kLogFileName).c_str(), "a"); \
    CRASH_LOG_MESSAGE("\n\nHyperion server crashed%c", '\n');                                   \
    CRASH_LOG_MESSAGE("Below contains debug information related to the crash\n%c", '\n')


constexpr char kCrashContactInfo[] = "https://github.com/Team-Hyperion/Hyperion-server";


void PrintStackTrace(FILE* file);

// ======================================================================
// Windows (MSVC) crash handling

#ifdef _MSC_VER

#include <StackWalker/StackWalker.h>

class HyperionStackWalker final : public StackWalker
{
public:
    explicit HyperionStackWalker(FILE* file) : file(file) {}

protected:
    void OnOutput(const LPCSTR szText) override {
        CRASH_LOG_MESSAGE("%s", szText);
    }

private:
    FILE* file; // Cannot be renamed for macro to work
};

LONG WINAPI TopLevelExceptionHandler(const PEXCEPTION_POINTERS pExceptionInfo) {
    CRASH_OPEN_LOG_FILE();

    std::ostringstream sstr;
    sstr << std::hex << pExceptionInfo->ExceptionRecord->ExceptionCode;

    CRASH_LOG_MESSAGE("%s\n", sstr.str().c_str());

    PrintStackTrace(file);

    return EXCEPTION_CONTINUE_SEARCH;
}

#endif


// ======================================================================
// OSx / Linux

#if defined(__linux__) || defined(__APPLE__)

#include <backward.hpp>

#endif

///
/// \param file Will be closed on function return
void PrintStackTrace(FILE* file) {
    // Log to both file and stderr

    CRASH_LOG_MESSAGE("\nStacktrace: %c", '\n');

    // Print stacktrace
    if constexpr (std::string_view(HYPERION_BUILD_TARGET_PLATFORM) == "Windows") {
#ifdef _MSC_VER

        HyperionStackWalker sw{file};
        sw.ShowCallstack();

#endif
    }
    else if constexpr (std::string_view(HYPERION_BUILD_TARGET_PLATFORM) == "Darwin" ||
                       std::string_view(HYPERION_BUILD_TARGET_PLATFORM) == "Linux") {
#if defined(__linux__) || defined(__APPLE__)

        using namespace backward;
        StackTrace st;

        st.load_here(99); // Limit the number of trace depth
        // st.skip_n_firsts(3);  // Skip some backward internal function from the trace

        // To console
        Printer p;
        p.snippet    = true;
        p.object     = true;
        p.address    = true;
        p.color_mode = ColorMode::automatic;
        p.print(st, stderr);

        // To log file
        p.color_mode = ColorMode::never;
        p.print(st, file);

#endif
    }

    CRASH_LOG_MESSAGE("\n\nPlease report this with the current log at %s\n", kCrashContactInfo);

    fclose(file);
}

[[noreturn]] void SignalCrashHandler(const int signum) {
    const char* name = nullptr;

    switch (signum) {
    case SIGABRT:
        name = "SIGABRT";
        break;
    case SIGSEGV:
        name = "SIGSEGV";
        break;
    case SIGILL:
        name = "SIGILL";
        break;
    case SIGFPE:
        name = "SIGFPE";
        break;

    default:
        break;
    }

    CRASH_OPEN_LOG_FILE();

    if (name) {
        CRASH_LOG_MESSAGE("Signal %d (%s)\n", signum, name);
    }
    else {
        CRASH_LOG_MESSAGE("Signal %d\n", signum);
    }

    PrintStackTrace(file);
    exit(signum);
}

void hyperion::core::RegisterCrashHandler() {
    // Crash handling functions
    signal(SIGABRT, &SignalCrashHandler);
    signal(SIGSEGV, &SignalCrashHandler);
    signal(SIGILL, &SignalCrashHandler);
    signal(SIGFPE, &SignalCrashHandler);

    // Windows SEH
#ifdef _MSC_VER
    SetUnhandledExceptionFilter(TopLevelExceptionHandler);
#endif
}
