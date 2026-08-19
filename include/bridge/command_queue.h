#pragma once
#include <atomic>
#include <array>
#include <string>
#include <cstring>

namespace hse {

struct Command {
    enum Type {
        CMD_NONE = 0,
        CMD_ADD_PRIMITIVE,
        CMD_REMOVE_PRIMITIVE,
        CMD_SET_POSITION,
        CMD_SET_ROTATION,
        CMD_SET_SCALE,
        CMD_SET_COLOR,
        CMD_SET_CAMERA,
        CMD_SET_CLEAR_COLOR,
        CMD_SCENE_STATE,
        CMD_OBSERVE_SCENE,
        CMD_OBSERVE_OBJECT,
        CMD_OBSERVE_CAMERA,
        CMD_OBSERVE_HEALTH,
        CMD_CAPTURE_FRAME,
        CMD_SAVE_PROJECT,
        CMD_LOAD_PROJECT,
        CMD_REPARENT,
        CMD_ORBIT_START,
        CMD_ORBIT_STOP,
        CMD_ORBIT_SET,
        CMD_SET_ROTATION_SPEED,
        CMD_OBSERVE_COLLISIONS,
        CMD_HEALTH
    };

    Type type = CMD_NONE;
    uint64_t seq = 0;
    int index = -1;
    uint64_t objectID = 0;
    float px = 0, py = 0, pz = 0;
    float rx = 0, ry = 0, rz = 0;
    float sx = 1, sy = 1, sz = 1;
    float cr = 1, cg = 1, cb = 1;
    int primType = 0;
    float fov = 60;
    char name[64] = {};
    uint64_t sinceRevision = 0;
    uint64_t parentID = 0;
    float orbitRadius = 5.0f;
    float orbitSpeed = 1.0f;
};

static constexpr int RING_SIZE = 256;

class CommandQueue {
public:
    bool push(const Command& cmd) {
        size_t write = m_write.load(std::memory_order_relaxed);
        size_t next = (write + 1) % RING_SIZE;
        if (next == m_read.load(std::memory_order_acquire)) {
            return false;
        }
        m_ring[write] = cmd;
        m_write.store(next, std::memory_order_release);
        return true;
    }

    bool pop(Command& cmd) {
        size_t read = m_read.load(std::memory_order_relaxed);
        if (read == m_write.load(std::memory_order_acquire)) {
            return false;
        }
        cmd = m_ring[read];
        m_read.store((read + 1) % RING_SIZE, std::memory_order_release);
        return true;
    }

    bool empty() const {
        return m_read.load(std::memory_order_acquire) == m_write.load(std::memory_order_acquire);
    }

    void clear() {
        m_read.store(0, std::memory_order_relaxed);
        m_write.store(0, std::memory_order_relaxed);
    }

private:
    std::array<Command, RING_SIZE> m_ring{};
    std::atomic<size_t> m_read{0};
    std::atomic<size_t> m_write{0};
};

} // namespace hse
