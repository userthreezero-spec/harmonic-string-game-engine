#pragma once
#include <string>
#include <functional>

#ifdef _WIN32
#include <windows.h>
#endif

namespace hse {

class PipeServer {
public:
    PipeServer(const std::string& pipeName);
    ~PipeServer();

    bool start();
    void stop();
    bool waitClient(int timeoutMs = 5000);
    bool readLine(std::string& line);
    bool writeLine(const std::string& line);
    bool isClientConnected() const;
    void disconnectClient();

private:
    std::string m_pipeName;
    bool m_running = false;
    bool m_clientConnected = false;
#ifdef _WIN32
    HANDLE m_pipe = INVALID_HANDLE_VALUE;
#endif
};

} // namespace hse
