//
// Created by Ludwig Zeller on 08.08.2022.
//

#pragma once
#ifndef DEPTHCAMERA_TEXTURE_HPP
#define DEPTHCAMERA_TEXTURE_HPP

#include <librealsense2/rs.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "utils.hpp"

class Texture {
public:
    void render(const rs2::frame &, const rect&, float = 1.0f);
    void upload(const rs2::video_frame &);
    void show(const rect &, float d);
    [[nodiscard]] unsigned int get_gl_handle() const { return _gl_handle; }
private:
    unsigned int _gl_handle = 0;
    rs2_stream _stream_type = RS2_STREAM_ANY;
    int _stream_index{};
};


#endif //DEPTHCAMERA_TEXTURE_HPP
