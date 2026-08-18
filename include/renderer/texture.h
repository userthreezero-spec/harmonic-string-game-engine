#pragma once
#include <string>

namespace hse {

class Texture {
public:
    Texture(const std::string& path);
    ~Texture();

    void bind(unsigned int slot = 0) const;
    void unbind() const;

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    unsigned int getID() const { return m_rendererID; }

private:
    unsigned int m_rendererID;
    int m_width, m_height, m_channels;
};

} // namespace hse
