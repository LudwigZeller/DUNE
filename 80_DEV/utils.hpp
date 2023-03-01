//
// Created by Ludwig Zeller on 03.08.2022.
//
#pragma once
#ifndef DEPTHCAMERA_UTILS_HPP
#define DEPTHCAMERA_UTILS_HPP

#include "config.hpp"

#include <cassert>
#include <iomanip>
#include <fstream>
#include <iostream>
#include "glad/glad.h"
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include <vector>
#include <opencv2/opencv.hpp>
#include "dependencies/stb_easy_font.h"
#include "Window.hpp"
#include "librealsense2/hpp/rs_frame.hpp"
#include <time.h>

enum log_type { info, warn, err };
inline std::ostream &clog(log_type &&lt)
{
    time_t sys_time;
    struct tm *local_time;
    time( &sys_time );
    local_time = localtime(&sys_time);

    int sys_hour = local_time->tm_hour;
    int sys_minu = local_time->tm_min;
    int sys_seco = local_time->tm_sec;

    switch(lt)
    {
    case info:
        return std::cout << "\033[37m[INFO @ " << sys_hour << 'h' << sys_minu << 'm' << sys_seco << "s] ";
    case warn:
        return std::cout << "\033[33m[WARNING @ " << sys_hour << 'h' << sys_minu << 'm' << sys_seco << "s] ";
    case err:
        return std::cerr << "\033[31m[ERROR @ " << sys_hour << 'h' << sys_minu << 'm' << sys_seco << "s] ";
    }
    return std::cout;
} 

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
        clog(err) << message << "\n" << std::endl; \
    assert(assertion)

void while_timer();


struct Quadrangle {
    cv::Point tl;
    cv::Point bl;
    cv::Point br;
    cv::Point tr;
};
Quadrangle check_bounds(const rs2::frame &color_frame, std::vector<cv::Vec3f> &circles);
void draw_quadrangle(cv::Mat& matrix, const Quadrangle& quad);


// https://gist.github.com/insaneyilin/038a022f2ece61c923315306ddcea081
inline GLuint matToTexture(const cv::Mat &mat, GLenum minFilter, GLenum magFilter, GLenum wrapFilter) {
    // Generate a number for our textureID's unique handle
    GLuint textureID;
    glGenTextures(1, &textureID);

    // Bind to our texture handle
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Catch silly-mistake texture interpolation method for magnification
    if (magFilter == GL_LINEAR_MIPMAP_LINEAR ||
        magFilter == GL_LINEAR_MIPMAP_NEAREST ||
        magFilter == GL_NEAREST_MIPMAP_LINEAR ||
        magFilter == GL_NEAREST_MIPMAP_NEAREST) {
        clog(err) << "You can't use MIPMAPs for magnification - setting filter to GL_LINEAR" << std::endl;
        magFilter = GL_LINEAR;
    }

    // Set texture interpolation methods for minification and magnification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    // Set texture clamping method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapFilter);

    // Set incoming texture format to:
    // GL_BGR       for CV_CAP_OPENNI_BGR_IMAGE,
    // GL_LUMINANCE for CV_CAP_OPENNI_DISPARITY_MAP,
    // Work out other mappings as required ( there's a list in comments in main() )
    GLenum inputColourFormat = GL_BGR;
    if (mat.channels() == 1) {
        inputColourFormat = GL_LUMINANCE;
    }

    // Create the texture
    glTexImage2D(GL_TEXTURE_2D,     // Type of texture
                 0,                 // Pyramid level (for mip-mapping) - 0 is the top level
                 GL_RGB,            // Internal colour format to convert to
                 mat.cols,          // Image width  i.e. 640 for Kinect in standard mode
                 mat.rows,          // Image height i.e. 480 for Kinect in standard mode
                 0,                 // Border width in pixels (can either be 1 or 0)
                 inputColourFormat, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
                 GL_UNSIGNED_BYTE,  // Image data type
                 mat.ptr());        // The actual image data itself

    // If we're using mipmaps then generate them. Note: This requires OpenGL 3.0 or higher
    if (minFilter == GL_LINEAR_MIPMAP_LINEAR ||
        minFilter == GL_LINEAR_MIPMAP_NEAREST ||
        minFilter == GL_NEAREST_MIPMAP_LINEAR ||
        minFilter == GL_NEAREST_MIPMAP_NEAREST) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    return textureID;
}


// https://gist.github.com/insaneyilin/038a022f2ece61c923315306ddcea081
inline void draw_frame(const cv::Size &size, const cv::Mat &frame, const cv::Size &xy = {0,0}) {
    // Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);     // Operate on model-view matrix

    glEnable(GL_TEXTURE_2D);
    GLuint image_tex = matToTexture(frame, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP);


    /* Draw a quad */
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);     glVertex2i((int) xy.width,                      (int) xy.height);
    glTexCoord2i(0, 1);     glVertex2i((int) xy.width,                      (int) size.height + (int) xy.height);
    glTexCoord2i(1, 1);     glVertex2i((int) size.width + (int) xy.width,   (int) size.height + (int) xy.height);
    glTexCoord2i(1, 0);     glVertex2i((int) size.width + (int) xy.width,   (int) xy.height);
    glEnd();

    glDeleteTextures(1, &image_tex);
    glDisable(GL_TEXTURE_2D);
}

inline void draw_text(int x, int y, const char *text) {
    std::vector<char> buffer;
    buffer.resize(60000); // ~300 chars
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, &(buffer[0]));
    glDrawArrays(GL_QUADS,
                 0,
                 4
                 * stb_easy_font_print((float) x,
                                       (float) (y - 7),
                                       (char *) text,
                                       nullptr,
                                       &(buffer[0]),
                                       int(sizeof(char) * buffer.size())));
    glDisableClientState(GL_VERTEX_ARRAY);
}

inline void draw_text_debug(int x, int y, const char *text) {
#if DEBUG_ON_SCREEN
    std::vector<char> buffer;
    buffer.resize(60000); // ~300 chars
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, &(buffer[0]));
    glDrawArrays(GL_QUADS,
                 0,
                 4
                 * stb_easy_font_print((float) x,
                                       (float) (y - 7),
                                       (char *) text,
                                       nullptr,
                                       &(buffer[0]),
                                       int(sizeof(char) * buffer.size())));
    glDisableClientState(GL_VERTEX_ARRAY);
#endif
}

cv::Mat frame_to_mat(const rs2::frame &f);

// Converts depth frame to a matrix of doubles with distances in meters
cv::Mat depth_frame_to_meters(const rs2::depth_frame &f);

struct float2 {
    float x, y;
};

struct rect {
    float x, y;
    float w, h;
    [[nodiscard]] rect adjust_ratio(float2 size) const {
        auto H = static_cast<float>(h), W = static_cast<float>(h) * size.x / size.y;
        if (W > w) {
            auto scale = w / W;
            W *= scale;
            H *= scale;
        }

        return {x + (w - W) / 2, y + (h - H) / 2, W, H};
    }
};

inline void set_viewport(const rect &r) {
    glViewport((int) r.x, (int) r.y, (int) r.w, (int) r.h);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glOrtho(0, r.w, r.h, 0, -1, +1);
}

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

ShaderProgramSource parseShader(const std::string &path);

unsigned int createShader(const ShaderProgramSource &source);

static unsigned int compileShader(unsigned int type, const std::string &source);

/**
 * @brief Warning! This function is only useable for RGB Matrices!
*/
cv::Mat assetToMat(unsigned int width, unsigned int height, const char *data);
/**
 * @brief Warning! This function is only useable for Z16-Depth Matrices!
*/
cv::Mat assetToMatDepth(unsigned int width, unsigned int height, const short *data);

static const char CHHEX_Translate[17] = "0123456789ABCDEF";

inline void saveMatAsFile(const cv::Mat &cln, const std::string name, const std::string path)
{
    uchar *dat = (uchar*) cln.ptr<uchar[3]>();
    cv::Size s = cln.size();
    std::ofstream dlm_out(path);
    clog(info) << "Creating asset: " << path << std::endl;
    
    dlm_out <<
        "/**\n * Auto-generated resource \"" << path << "\" originating from OpenCV matrix"         \
        "\n * Asset \"" << name << "\" created for DUNE Project\n * @author David Schoosleitner"                       \
        "\n * (c) David Schoosleitner 2022\n */\n\n#pragma once\n\n/***** DATA *****/\n" << std::endl;
    dlm_out << "//! Matrix Width\n#define " << name << "_WIDTH (" << s.width << "u)\n" << std::endl;
    dlm_out << "//! Matrix Height\n#define " << name << "_HEIGHT (" << s.height << "u)\n" << std::endl;
    dlm_out << "//! Matrix Data: Hex format 0x[BBGGRR]0x[....\n#define " << name << "_DATA ( \\" << std::endl;

    std::string tmpdat;

    {
        std::stringstream dlm_tmp1;

        std::string row = "";
        std::string set_full = "";
        std::string set = "";
        uchar g = 0;
        uchar l = 255;
        int count = 0;

        for(int i = 0; i < s.height; i++)
        {
            dlm_tmp1 << "  \"";
            row = "";
            g = 255;
            count = 0;
            for(int j = 0; j < s.width; j++)
            {
                for(int k = 0; k < 3; k++)
                {
                    l = g;
                    g = (dat[3*(j + i * s.width) + k]);
                    if(g == l && count < 26)
                    {
                        set = (char)(count + 'a');
                        set += CHHEX_Translate[g >> 4];
                        set += CHHEX_Translate[g & 0x0F];
                        count++;
                    }
                    else
                    {
                        count = 0;
                        row += set;
                        set = CHHEX_Translate[g >> 4];
                        set += CHHEX_Translate[g & 0x0F];
                    }
                }
            }
            dlm_tmp1 << row << "\" \\" << std::endl;

        }
        tmpdat = dlm_tmp1.str();
    }

    dlm_out << tmpdat << std::flush;

    dlm_out << ")\n/// " << name << "\n" << std::endl;
    dlm_out.close();
    clog(info) << "Done " << name << std::endl;
}

inline void saveMatDepth(const cv::Mat &cln, const std::string name, const std::string path)
{
    short *dat = (short*) cln.ptr<short>();
    cv::Size s = cln.size();
    std::ofstream dlm_out(path);
    clog(info) << "Creating depth asset: " << path << std::endl;
    
    dlm_out <<
        "/**\n * Auto-generated depth resource \"" << path << "\" originating from rs depth matrix"         \
        "\n * Asset \"" << name << "\" created for DUNE Project\n * @author David Schoosleitner"                       \
        "\n * (c) David Schoosleitner 2022\n */\n\n#pragma once\n\n/***** DATA *****/\n" << std::endl;
    dlm_out << "//! Matrix Width\n#define " << name << "_WIDTH (" << s.width << "u)\n" << std::endl;
    dlm_out << "//! Matrix Height\n#define " << name << "_HEIGHT (" << s.height << "u)\n" << std::endl;
    dlm_out << "//! Matrix Data: Hex format 0x[BBGGRR]0x[....\n#define " << name << "_DATA (short[]{ \\" << std::endl;

    for(int i = 0; i < s.height; i++)
    {

        dlm_out << "  ";

        for(int j = 0; j < s.width; j++)
        {
            dlm_out << dat[j + i * s.width] << ", ";
        }

        dlm_out << "\t\\" << std::endl;

    }

    dlm_out << "})\n/// " << name << "\n" << std::endl;
    dlm_out.close();
    clog(info) << "Done " << name << std::endl;
}

void capture(const cv::Mat &cln);
void captureDepth(const cv::Mat &cln);

inline void meta_translate(cv::Mat &m, cv::Point2i translate)
{
    bool tx_sm_0 = 0 > translate.x;
    bool ty_sm_0 = 0 > translate.y;

    int x_area_cp = STREAM_WIDTH + (tx_sm_0 ? translate.x : (-translate.x));
    int y_area_cp = STREAM_HEIGHT + (ty_sm_0 ? translate.y : (-translate.y));

    cv::Rect src{
        !tx_sm_0 ? 0 : -translate.x,
        !ty_sm_0 ? 0 : -translate.y,
        x_area_cp, y_area_cp
    };

    cv::Rect dst{
        !tx_sm_0 ? translate.x : 0,
        !ty_sm_0 ? translate.y : 0,
        x_area_cp, y_area_cp
    };

    cv::Mat tmp = std::move(m);
    m = cv::Mat::zeros(tmp.size(), CV_8U);
    cv::copyTo(tmp(src), m(dst), cv::Mat());
}

inline bool file_exists(const std::string path)
{
    std::ifstream f(path);
    return f.good();
}

#define macro_depthAssetToMatrix(matrix, width, height, data) { \
    short cursed[] = data;              \
    matrix = assetToMatDepth(width, height, cursed); \
}

#define macro_matrixToMeters(matrix) { \
    matrix.convertTo(matrix, CV_64F); \
    matrix *= 0.001; \
} 

inline void save_depth_image(std::string path, cv::Mat mat)
{
    if(file_exists(path + ".res"))
    {
        int check = 1;
        while(file_exists(path + "_(" + std::to_string(check) + ").res")) check++;
        path = path + "_(" + std::to_string(check) + ")";
    }

    std::ofstream outp{path + ".res", std::ios_base::openmode::_S_bin};
    std::vector<uchar> dat{12UL + (unsigned long)((mat.cols * mat.rows) >> 1) + (unsigned long)((mat.cols * mat.rows) & 1), std::allocator<char>{}};

    memcpy(dat.data(), "DUNE", 4);
    memcpy(dat.data() + 4, &mat.cols, 4);
    memcpy(dat.data() + 8, &mat.rows, 4);
 
    uchar buf = 0;
    uchar *place = dat.data() + 12;
    int ct = 0;

    for(int y = 0; y < mat.rows; y++)
    {
        for(int x = 0; x < mat.cols; x++)
        {
            buf |= mat.at<uchar>(y,x) << (ct * 4);
            if(++ct > 1)
            {
                ct = 0;
                *(place++) = buf;
                buf = 0;
            }
        }
    }
    if(buf)
        *place = buf;

    outp.write((const char*) dat.data(), dat.size());
    outp.close();
}

inline cv::Mat load_depth_image(const std::string &path)
{
    if(!file_exists(path))
    {
        return {};
    }

    std::ifstream inp{path, std::ios_base::openmode::_S_bin | std::ios_base::openmode::_S_ate};
    std::ifstream::pos_type _filesize = inp.tellg() + (std::ifstream::pos_type) 1;
    inp.seekg(0, std::ios::beg);

    std::vector<char> dat{(std::vector<char>::size_type) _filesize, std::allocator<char>{}};
    inp.read(dat.data(), _filesize);
    inp.close();

    if(memcmp(dat.data(), "DUNE", 4) != 0)
        return {};

    cv::Size size{};
    memcpy(&size.width, dat.data() + 4, 4);
    memcpy(&size.height, dat.data() + 8, 4);

    cv::Mat mat{size, CV_8U};
    uchar *ptr = mat.ptr<uchar>();

    for(int i = 0; i < size.width * size.height; i++)
    {
        int idx = i >> 1;
        int ct = i & 1;
        ptr[i] = 0x0F & (dat[idx] >> ct);
    }

    return mat;
}

#endif //DEPTHCAMERA_UTILS_HPP
