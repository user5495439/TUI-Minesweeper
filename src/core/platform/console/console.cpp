#include "console.h"
#include <sys/ioctl.h>
#include <unistd.h>

namespace core::platform::console
{
    XY Console::windowSizes()
    {
        // ChatGPT coded these two lines below because i didn't know how to code that myself
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

        return 
        {
            w.ws_col,   // note to myself: number of columns
            w.ws_row    // note to myself: number of rows
        };
    }
}