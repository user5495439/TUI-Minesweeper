#pragma once

#include "../xy.h"
#include <cstddef>
#include <string>
#include <termios.h>
#include <unistd.h>

namespace core::input
{
    class MouseAndKeyboardInput
    {
    public:
        struct MouseInput
        {
        public:
            XY mousePos;
            int mouseState;
            char mouseButton;
        };

        static void autoInitialize();
        static void rawMode(bool enable = true);
        static void mouseEventTracking(bool enable = true);
        static void hideCursor(bool enable = true);
        static void altScreenBuffer(bool enable = true);
        static void enableExitHandling();
        static void enableWakeupPipe() { pipe(wakeupPipe); };
        static ssize_t stdinRead(std::string& output);
        static void interruptStdinRead();
        static bool filterMouseInput(const std::string& input, MouseInput* output);
        static bool filterKbInput(const std::string& input, std::string* output);
    
    private:
        inline static constexpr char escapeEnableAnyEventMTracking[] = "\x1b[?1003h";
        inline static constexpr char escapeEnableSGRmode[] = "\x1b[?1006h";
        inline static constexpr char escapeDisableAnyEventMTracking[] = "\x1b[?1003l";
        inline static constexpr char escapeDisableSGRmode[] = "\x1b[?1006l";

        inline static constexpr char escapeEnableHideCursor[] = "\x1b[?25l";
        inline static constexpr char escapeDisableHideCursor[] = "\x1b[?25h";

        inline static constexpr char escapeEnableAltScreenBuffer[] = "\x1b[?1049h";
        inline static constexpr char escapeDisableAltScreenBuffer[] = "\x1b[?1049l";

        static inline termios orig{};

        inline static int wakeupPipe[2]{};

        inline static bool autoInitialized = false;

        inline static bool rawModeEnabled = false;
        inline static bool mouseEventTrackingEnabled = false;
        inline static bool hideCursorEnabled = false;
        inline static bool altScreenBufferEnabled = false;

        static void cleanUp();
        static void signalHandler(int sig);
        static void stdoutWrite(const char* str);
        static void stderrWrite(const char* str);

        // i could've also coded this myself but ChatGPT helped me out here
        static constexpr size_t signalSafeStrlen(const char* str)
        {
            size_t length = 0;

            while (str[length] != '\0')
                ++length;

            return length;
        }

        ~MouseAndKeyboardInput();
    };
}