#include "input.h"

#include <csignal>
#include <cstdlib>
#include <cstring>
#include <termios.h>
#include <unistd.h>
#include <poll.h>

namespace core::input
{
//public:
    void MouseAndKeyboardInput::autoInitialize()
    {
        if (autoInitialized)
            return;

        enableExitHandling();
        enableWakeupPipe();
        rawMode();
        altScreenBuffer();
        hideCursor();
        mouseEventTracking();

        autoInitialized = true;
    }

    void MouseAndKeyboardInput::rawMode(bool enable)
    {
        rawModeEnabled = enable;

        if (enable)
        {
            // Save original terminal settings
            tcgetattr(STDIN_FILENO, &orig);

            termios raw = orig;

            // Disable canonical mode, echo, and signals
            raw.c_lflag &= ~(ICANON | ECHO | ISIG);

            // Return after 1 byte is available
            raw.c_cc[VMIN] = 1;

            // No timeout
            raw.c_cc[VTIME] = 0;

            tcsetattr(STDIN_FILENO, TCSANOW, &raw);

            return;
        }

        tcsetattr(STDIN_FILENO, TCSANOW, &orig);    // restore original settings
    }

    void MouseAndKeyboardInput::mouseEventTracking(bool enable)
    {
        mouseEventTrackingEnabled = enable;

        if (enable)
        {
            // Enable any-event mouse tracking + SGR mode
            stdoutWrite(escapeEnableAnyEventMTracking); // any-event mouse
            stdoutWrite(escapeEnableSGRmode);   // SGR mode

            return;
        }

        // disable
        stdoutWrite(escapeDisableAnyEventMTracking);
        stdoutWrite(escapeDisableSGRmode);
    }

    void MouseAndKeyboardInput::hideCursor(bool enable)
    {
        hideCursorEnabled = enable;

        if (enable)
        {
            stdoutWrite(escapeEnableHideCursor); // enable hide cursor

            return;
        }

        stdoutWrite(escapeDisableHideCursor); // disable
    }

    void MouseAndKeyboardInput::altScreenBuffer(bool enable)
    {
        altScreenBufferEnabled = enable;

        if (enable)
        {
            stdoutWrite(escapeEnableAltScreenBuffer);   // enable alternative screen buffer

            return;
        }

        stdoutWrite(escapeDisableAltScreenBuffer);   // disable
    }

    void MouseAndKeyboardInput::enableExitHandling()
    {
        std::atexit(cleanUp);

        struct sigaction sa{};
        sa.sa_handler = signalHandler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;

        sigaction(SIGINT, &sa, nullptr);
        sigaction(SIGTERM, &sa, nullptr);
        sigaction(SIGFPE, &sa, nullptr);
    #ifdef SIGSEGV
        sigaction(SIGSEGV, &sa, nullptr);
    #endif
    }
    
    // reads from stdin with wakeup pipe thing support
    // returns the return output of read(), poll() if poll returned an error or -1 if interrupted
    ssize_t MouseAndKeyboardInput::stdinRead(std::string& output)
    {
        pollfd fds[2];

        fds[0].fd = STDIN_FILENO;
        fds[0].events = POLLIN;

        fds[1].fd = wakeupPipe[0];
        fds[1].events = POLLIN;

        int result = poll(fds, 2, -1);

        if (result <= 0) return result;

        if (fds[1].revents & POLLIN)
        {
            char temp[32];
            read(wakeupPipe[0], temp, sizeof(temp));

            errno = EINTR;
            return -1;
        }

        if (fds[0].revents & POLLIN)
        {
            char buffer[256];   // this was 1024 but i think that's a bit too unnecessarily big

            ssize_t bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer));   // sizeof(buffer) / sizeof(buffer[0])

            if (bytesRead <= 0) return bytesRead;

            //std::string string(buffer, buffer + n);
            //output = string;

            output.assign(buffer, bytesRead);

            return bytesRead;
        }

        return 0;
    }

    void MouseAndKeyboardInput::interruptStdinRead()
    {
        char c = 1;
        write(wakeupPipe[1], &c, 1);
    }

    // true if found mouse input, false if not
    bool MouseAndKeyboardInput::filterMouseInput(const std::string& input, MouseInput* output)
    {
        // find ESC[<
        size_t i = input.find("\x1B[<");

        if (i == std::string_view::npos)
            return false;

        // Find final M or m
        char finalChar = input.back();

        if (finalChar != 'M' && finalChar != 'm')
            return false;

        i += 3; // move past ESC[<

        auto readNumber = [&](size_t& pos) -> int {
            int value = 0;
            while (pos < input.size() && input[pos] >= '0' && input[pos] <= '9')
            {
                value = value * 10 + (input[pos] - '0');
                ++pos;
            }
            return value;
        };

        int mouseState = readNumber(i);
        if (i >= input.size() || input[i] != ';') return false;
        ++i;

        int mouseX = readNumber(i);
        if (i >= input.size() || input[i] != ';') return false;
        ++i;

        int mouseY = readNumber(i);
        if (i >= input.size()) return false;

        char mouseButton = finalChar; // 'M' or 'm'

        output->mousePos = {mouseX, mouseY};
        output->mouseState = mouseState;
        output->mouseButton = mouseButton;

        return true;
    }

    // true if keyboard input, false if not
    bool MouseAndKeyboardInput::filterKbInput(const std::string& input, std::string* output)
    {
        if (input.empty()) return false;

        for (const char& c : input)
        {
            if (c == 27)    // 27 = mouse input escape character
                return false;
        }

        *output = input;

        return true;
    }

//private:
    void MouseAndKeyboardInput::cleanUp()
    {
        if (rawModeEnabled) rawMode(false);
        if (mouseEventTrackingEnabled) mouseEventTracking(false);
        if (hideCursorEnabled) hideCursor(false);
        if (altScreenBufferEnabled) altScreenBuffer(false);
    }

    void MouseAndKeyboardInput::signalHandler(int sig)
    {
        cleanUp();

        std::string message = "Exited with signal " + std::to_string(sig) + " (" + strsignal(sig) + ").\n";

        stderrWrite(message.c_str());

        std::_Exit(sig);
    }

    void MouseAndKeyboardInput::stdoutWrite(const char* str)
    {
        write(STDOUT_FILENO, str, signalSafeStrlen(str));
    }

    void MouseAndKeyboardInput::stderrWrite(const char* str)
    {
        write(STDERR_FILENO, str, signalSafeStrlen(str));
    }

    MouseAndKeyboardInput::~MouseAndKeyboardInput()
    {
        cleanUp();
    }
}