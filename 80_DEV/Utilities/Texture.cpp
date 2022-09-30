//
// Created by Ludwig Zeller on 08.08.2022.
//

#include "Texture.hpp"

void Texture::render(const rs2::frame &frame, const rect &rect, float alpha) {
    auto vf = frame.as<rs2::video_frame>();
    if (!vf) {
        std::cerr << "Missing video frame!" << std::endl;
        return;
    }
    upload(vf);
    show(rect.adjust_ratio({(float) vf.get_width(), (float) vf.get_height()}), alpha);
}
void Texture::upload(const rs2::video_frame &frame) {
    if (!frame) return;

    if (!_gl_handle)
        glGenTextures(1, &_gl_handle);
    GLenum err = glGetError();

    auto format = frame.get_profile().format();
    auto width = frame.get_width();
    auto height = frame.get_height();
    _stream_type = frame.get_profile().stream_type();
    _stream_index = frame.get_profile().stream_index();

    glBindTexture(GL_TEXTURE_2D, _gl_handle);

    switch (format) {
        case RS2_FORMAT_RGB8:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, frame.get_data());
            break;
        case RS2_FORMAT_RGBA8:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame.get_data());
            break;
        case RS2_FORMAT_Y8:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, frame.get_data());
            break;
        case RS2_FORMAT_Y10BPACK:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_SHORT,
                         frame.get_data());
            break;
        default:
            throw std::runtime_error("The requested format is not supported by this demo!");
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
void Texture::show(const rect &r, float alpha) {
    if (!_gl_handle)
        return;

    set_viewport(r);

    glBindTexture(GL_TEXTURE_2D, _gl_handle);
    glColor4f(1.0f, 1.0f, 1.0f, alpha);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(0, 0);
    glTexCoord2f(0, 1);
    glVertex2f(0, r.h);
    glTexCoord2f(1, 1);
    glVertex2f(r.w, r.h);
    glTexCoord2f(1, 0);
    glVertex2f(r.w, 0);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
//    draw_text(int(0.05f * r.w), int(0.05f * r.h), rs2_stream_to_string(_stream_type));
}
