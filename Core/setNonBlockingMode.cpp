#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

void setNonBlockingMode(bool enable) {
    static bool isNonBlocking = false;
    static struct termios orig_termios;

    if (enable && !isNonBlocking) {
        tcgetattr(STDIN_FILENO, &orig_termios); // Save original terminal settings
        struct termios new_termios = orig_termios;
        new_termios.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
        tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK); // Set stdin to non-blocking
        isNonBlocking = true;
    } else if (!enable && isNonBlocking) {
        tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios); // Restore terminal settings
        fcntl(STDIN_FILENO, F_SETFL, 0); // Restore blocking mode
        isNonBlocking = false;
    }
}