//
// Created by Ludwig Zeller on 03.08.2022.
//
#pragma once
#ifndef DEPTHCAMERA_UTILS_HPP
#define DEPTHCAMERA_UTILS_HPP

#include "constants.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <glad/glad.h>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include <vector>
#include "dependencies/stb_easy_font.h"



/**
 * Macro asserts the condition
 * @param assertion A true or false statement
 * @param message The message to print, if assertion fails
 * @attention Cannot be used as single line.\n
 * For example\n
 * if(condition)\n
 *     assert_log(condition2, "Error");\n
 * wont work
 */
#define assert_log(assertion, message) \
    if(!(assertion)) \
        std::cerr << "Error: " << message << "\n" << std::endl; \
    assert(assertion)

void while_timer();

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

ShaderProgramSource parseShader(const std::string &path);

unsigned int createShader(const ShaderProgramSource &source);

static unsigned int compileShader(unsigned int type, const std::string &source);

inline void draw_text(int x, int y, const char* text)
{
    std::vector<char> buffer;
    buffer.resize(60000); // ~300 chars
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, &(buffer[0]) );
    glDrawArrays( GL_QUADS,
                  0,
                  4
                  * stb_easy_font_print( (float)x,
                                         (float)( y - 7 ),
                                         (char *)text,
                                         nullptr,
                                         &( buffer[0] ),
                                         int( sizeof( char ) * buffer.size() ) ) );
    glDisableClientState(GL_VERTEX_ARRAY);
}

#endif //DEPTHCAMERA_UTILS_HPP
