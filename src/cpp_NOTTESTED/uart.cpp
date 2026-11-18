#include "bira_motor_control/uart.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <cerrno>

UART::UART() {
    fd = -1;
}

UART::~UART() {
    closePort();
}

bool UART::openPort(const std::string& port) {
    closePort();

    fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (fd < 0) {
        return false;
    }

    struct termios tty {};

    if (tcgetattr(fd, &tty) != 0) {
        closePort();
        return false;
    }

    cfmakeraw(&tty);

    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CS8;
    tty.c_cflag |= CREAD | CLOCAL;

    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        closePort();
        return false;
    }

    tcflush(fd, TCIOFLUSH);

    return true;
}

bool UART::sendData(const std::string& data) {
    if (fd < 0) {
        return false;
    }

    size_t sent = 0;

    while (sent < data.size()) {
        ssize_t result = write(
            fd,
            data.c_str() + sent,
            data.size() - sent
        );

        if (result < 0 && errno == EINTR) {
            continue;
        }

        if (result <= 0) {
            closePort();
            return false;
        }

        sent += result;
    }

    return true;
}

std::string UART::readData() {
    if (fd < 0) {
        return "";
    }

    char buffer[256];

    ssize_t count = read(fd, buffer, sizeof(buffer));

    if (count > 0) {
        return std::string(buffer, count);
    }

    return "";
}

void UART::closePort() {
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
}