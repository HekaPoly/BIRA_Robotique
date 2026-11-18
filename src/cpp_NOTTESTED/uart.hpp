#ifndef UART_HPP
#define UART_HPP

#include <string>

class UART {
public:
    UART();
    ~UART();

    bool openPort(const std::string& port);
    bool sendData(const std::string& data);
    std::string readData();
    void closePort();

private:
    int fd;
};

#endif