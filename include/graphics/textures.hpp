#pragma once

#include <algorithm>
#include <limits>
#include <iostream>
#include <vector>
#include <cstddef>

#include <GL/gl3w.h>
#include <glm/glm.hpp>

namespace tomovis {

template <typename T>
inline GLenum data_type();

template <>
inline GLenum data_type<uint8_t>() { return GL_UNSIGNED_BYTE; }

template <>
inline GLenum data_type<uint32_t>() { return GL_UNSIGNED_INT; }

template <>
inline GLenum data_type<float>() { return GL_FLOAT; }

template <typename T>
inline GLint format_type();

template <>
inline GLint format_type<uint8_t>() { return GL_RED; }

template <>
inline GLint format_type<uint32_t>() { return GL_RED; }

template <>
inline GLint format_type<float>() { return GL_R32F; }

template <typename T = unsigned char>
class texture {
   public:
    texture(int x, int y) : x_(x), y_(y) {
        glGenTextures(1, &texture_id_);
        std::vector<T> data(x * y);
        auto lim = std::numeric_limits<T>::max() - 2;
        for (int i = 0; i < x * y; ++i) {
            data[i] = lim * ((i + i / x) % 2);
        }
        fill_texture(data);
    }

    ~texture() {
        if (texture_id_ >= 0) {
            glDeleteTextures(1, &texture_id_);
        }
    }

    texture(texture&& other) {
        x_ = other.x_;
        y_ = other.y_;
        texture_id_ = other.texture_id_;
        other.texture_id_ = -1;
    }

    void set_data(std::vector<T>& data, int x, int y) {
        x_ = x;
        y_ = y;
        assert((int)data.size() == x * y);
        fill_texture(data);
    }

    void fill_texture(std::vector<T>& data) {
        std::cout << "tmin: " << *std::min_element(data.begin(), data.end()) << "\n";
        std::cout << "tmax: " << *std::max_element(data.begin(), data.end()) << "\n";
        glBindTexture(GL_TEXTURE_2D, texture_id_);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0, format_type<T>(), x_, y_, 0, GL_RED,
                     data_type<T>(), data.data());

        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void bind() const {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id_);
    }

    static void unbind() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

   private:
    GLuint texture_id_ = -1;
    int x_ = -1;
    int y_ = -1;
};

template <typename T = unsigned char>
class texture3d {
   public:
    texture3d(int x, int y, int z) : x_(x), y_(y), z_(z) {
        glGenTextures(1, &texture_id_);
        std::vector<T> data(x * y * z);
        auto lim = std::numeric_limits<T>::max() - 2;
        for (int i = 0; i < x * y * z; ++i) {
            data[i] = lim * ((i + i / x + i / (x * y)) % 2);
        }
        fill_texture(data);
    }

    ~texture3d() { glDeleteTextures(1, &texture_id_); }

    texture3d(const texture3d&) = delete;
    texture3d(const texture3d&&) = delete;

    void set_data(int x, int y, int z, std::vector<T>& data) {
        x_ = x;
        y_ = y;
        z_ = z;
        assert((int)data.size() == x * y * z);
        fill_texture(data);
    }

    void fill_texture(std::vector<T>& data) {
        glBindTexture(GL_TEXTURE_3D, texture_id_);

        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, x_, y_, z_, 0, GL_RED,
                     data_type<T>(), data.data());

        glGenerateMipmap(GL_TEXTURE_3D);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void bind() const {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_3D, texture_id_);
    }

    void unbind() const {
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_3D, 0);
    }

   private:
    GLuint texture_id_;
    int x_ = -1;
    int y_ = -1;
    int z_ = -1;
};

}  // namespace tomovis
