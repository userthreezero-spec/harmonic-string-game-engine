#include "bridge/pipe_transport.h"
#include <iostream>

#ifdef _WIN32

namespace hse {

PipeServer::PipeServer(const std::string& pipeName) : m_pipeName(pipeName) {}

PipeServer::~PipeServer() {
    stop();
}

bool PipeServer::start() {
    m_pipe = CreateNamedPipeA(
        m_pipeName.c_str(),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        1,
        4096,
        4096,
        0,
        nullptr);

    if (m_pipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create named pipe: " << m_pipeName << std::endl;
        return false;
    }

    m_running = true;
    std::cout << "Pipe server started: " << m_pipeName << std::endl;
    return true;
}

bool PipeServer::waitClient(int timeoutMs) {
    if (m_pipe == INVALID_HANDLE_VALUE) return false;

    BOOL ok = ConnectNamedPipe(m_pipe, nullptr);
    if (ok || GetLastError() == ERROR_PIPE_CONNECTED) {
        m_clientConnected = true;
        std::cout << "Client connected" << std::endl;
        return true;
    }

    if (GetLastError() == ERROR_IO_PENDING) {
        DWORD bytesTransferred = 0;
        WaitForSingleObject(m_pipe, timeoutMs);
        if (GetOverlappedResult(m_pipe, nullptr, &bytesTransferred, FALSE)) {
            m_clientConnected = true;
            std::cout << "Client connected" << std::endl;
            return true;
        }
    }

    std::cerr << "Wait for client failed: " << GetLastError() << std::endl;
    return false;
}

bool PipeServer::readLine(std::string& line) {
    if (!m_clientConnected) return false;

    line.clear();
    char buf[1];
    DWORD bytesRead;

    while (true) {
        BOOL ok = ReadFile(m_pipe, buf, 1, &bytesRead, nullptr);
        if (!ok || bytesRead == 0) {
            DWORD err = GetLastError();
            if (err == ERROR_BROKEN_PIPE || err == ERROR_NO_DATA) {
                m_clientConnected = false;
                std::cout << "Client disconnected" << std::endl;
                return false;
            }
            return false;
        }
        if (buf[0] == '\n') return true;
        if (buf[0] != '\r') line += buf[0];
    }
}

bool PipeServer::writeLine(const std::string& line) {
    if (!m_clientConnected) return false;

    std::string data = line + "\n";
    DWORD bytesWritten;
    BOOL ok = WriteFile(m_pipe, data.c_str(), static_cast<DWORD>(data.size()), &bytesWritten, nullptr);
    return ok && bytesWritten == data.size();
}

bool PipeServer::isClientConnected() const {
    return m_clientConnected;
}

void PipeServer::disconnectClient() {
    if (m_clientConnected) {
        DisconnectNamedPipe(m_pipe);
        m_clientConnected = false;
    }
}

void PipeServer::stop() {
    m_running = false;
    disconnectClient();
    if (m_pipe != INVALID_HANDLE_VALUE) {
        CloseHandle(m_pipe);
        m_pipe = INVALID_HANDLE_VALUE;
    }
}

} // namespace hse

#else
// Non-Windows stub
namespace hse {
PipeServer::PipeServer(const std::string&) {}
PipeServer::~PipeServer() {}
bool PipeServer::start() { return false; }
bool PipeServer::waitClient(int) { return false; }
bool PipeServer::readLine(std::string&) { return false; }
bool PipeServer::writeLine(const std::string&) { return false; }
bool PipeServer::isClientConnected() const { return false; }
void PipeServer::disconnectClient() {}
void PipeServer::stop() {}
}
#endif
