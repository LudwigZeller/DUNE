//
// Created by Ludwig Zeller on 03.08.2022.
//
#include "utils.hpp"

/**
 * Syncs the thread with the defined FPS
 */
void while_timer() {
    // I am sorry for the static variable
    // But like this the main loop is readable -
    // and I can use the "chrono" namespace in this scope
    using namespace std::chrono;
    static auto timestamp = steady_clock::now();
    static std::string str{};
    static unsigned int ct = 0;
    static unsigned int sum = 0;
    static unsigned int average = 20;

    auto elapsed = duration_cast<milliseconds>(steady_clock::now() - timestamp);
    if (elapsed < milliseconds((int) (1000.0f / FPS))) {
//        std::cout << "Sleeping for " << (milliseconds(1000 / FPS) - elapsed).count() << "ms" << std::endl;
        std::this_thread::sleep_for(milliseconds((int) (1000.0f / FPS)) - elapsed);
    } else {
        str = "";
        str += "- The thread is running " +
               (elapsed < milliseconds((int) (1000.0f / FPS)) ? "0" : std::to_string(
                       (elapsed - milliseconds((int) (1000.0f / FPS))).count())) +
               "ms behind";

        clog(err) << "The thread is running " << (elapsed - milliseconds((int) (1000.0f / FPS))).count() << "ms behind"
                  << std::endl;
        draw_text_debug(200, 20, str.c_str());
    }
#if DEBUG_ON_SCREEN
    str = "";
    ct++;
    sum += elapsed.count();

    if (ct >= CYCLE_COUNTER_AVERAGE) {
        average = sum / ct;
        sum = 0;
        ct = 0;
    }

    str += "Average over last " + std::to_string(CYCLE_COUNTER_AVERAGE) + " cycles: " + std::to_string(average) + "ms";
    draw_text_debug(10, 20, str.c_str());
#endif
    timestamp = steady_clock::now();

}

Quadrangle check_bounds(const rs2::frame &color_frame, std::vector<cv::Vec3f> &circles) {
    cv::Mat color_matrix_temp{};
    cv::Mat color_matrix = frame_to_mat(color_frame);
    auto size = color_matrix.size();
    color_matrix_temp = color_matrix.clone().reshape(1);
    cv::resize(color_matrix_temp, color_matrix_temp, size);
    cv::medianBlur(color_matrix_temp, color_matrix_temp, 5);
    cv::HoughCircles(color_matrix_temp, circles, cv::HOUGH_GRADIENT, 1,
                     (float) color_matrix.rows / 16.0f,
                     100, 30, 0, 0);
    return {{50,                     50},
            {50,                     color_matrix.rows - 50},
            {color_matrix.cols - 50, color_matrix.rows - 50},
            {color_matrix.cols - 50, 50}};
}

void draw_quadrangle(cv::Mat &matrix, const Quadrangle &quad) {
    cv::line(matrix, quad.tl, quad.bl, {255, 255, 255}, 2);
    cv::line(matrix, quad.bl, quad.br, {255, 255, 255}, 2);
    cv::line(matrix, quad.br, quad.tr, {255, 255, 255}, 2);
    cv::line(matrix, quad.tr, quad.tl, {255, 255, 255}, 2);
}

cv::Mat frame_to_mat(const rs2::frame &f) {
    using namespace cv;
    using namespace rs2;

    auto vf = f.as<video_frame>();
    const int w = vf.get_width();
    const int h = vf.get_height();

    if (f.get_profile().format() == RS2_FORMAT_BGR8) {
        return {Size(w, h), CV_8UC3, (void *) f.get_data(), Mat::AUTO_STEP};
    } else if (f.get_profile().format() == RS2_FORMAT_RGB8) {
        auto r_rgb = Mat(Size(w, h), CV_8UC3, (void *) f.get_data(), Mat::AUTO_STEP);
        Mat r_bgr;
        cvtColor(r_rgb, r_bgr, COLOR_RGB2BGR);
        return r_bgr;
    } else if (f.get_profile().format() == RS2_FORMAT_Z16) {
        return {Size(w, h), CV_16UC1, (void *) f.get_data(), Mat::AUTO_STEP};
    } else if (f.get_profile().format() == RS2_FORMAT_Y8) {
        return {Size(w, h), CV_8UC1, (void *) f.get_data(), Mat::AUTO_STEP};
    } else if (f.get_profile().format() == RS2_FORMAT_DISPARITY32) {
        return {Size(w, h), CV_32FC1, (void *) f.get_data(), Mat::AUTO_STEP};
    }
    throw std::runtime_error("Frame format is not supported!");
}

cv::Mat depth_frame_to_meters(const rs2::depth_frame &f) {
    cv::Mat dm = frame_to_mat(f);
    dm.convertTo(dm, CV_64F);
    dm = dm * f.get_units();
    return dm;
}

ShaderProgramSource parseShader(const std::string &path) {
    std::ifstream stream(path);

    enum class ShaderType {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    std::string line;
    std::stringstream stringstream[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        } else {
            if (type != ShaderType::NONE)
                stringstream[(int) type] << line << '\n';
        }
    }

    clog(info) << "Successfully loaded Shaders from: " << path.c_str() << std::endl;

    return {stringstream[(int) ShaderType::VERTEX].str(),
            stringstream[(int) ShaderType::FRAGMENT].str()};
}

unsigned int createShader(const ShaderProgramSource &source) {
    unsigned int program = glCreateProgram();
    unsigned int vs = compileShader(GL_VERTEX_SHADER, source.VertexSource);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, source.FragmentSource);
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);


    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

static unsigned int compileShader(unsigned int type, const std::string &source) {
    unsigned int id = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char *message = (char *) alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        clog(err) << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!"
                  << std::endl;
        clog(err) << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    clog(info) << "Successfully compiled shader of type: " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
              << std::endl;

    return id;
}

cv::Mat assetToMat(unsigned int width, unsigned int height, const char *data)
{
    // Allocated data init
    cv::Mat tmp = cv::Mat(cv::Size(width, height), CV_8UC3);
    uchar *dat = (uchar*) tmp.ptr<uchar[3]>();
    int x = 0,      //< X coordinate
        y = 0,      //< Y coordinate
        c = 0,      //< Repeat Counter
        n = 0;      //< R/G/B coordinate
    uchar dh = 0u,  //< High nibble
        dl = 0u;    //< Low nibble

    std::string last_color = "";

    unsigned long long until = strlen(data);

    #define _g dat[n + 3 * (x + y * width)]
    #define _h2i(val) ((val >= 'A' && val <= 'F') ? (val - 'A' + 10) : (val - '0'))
    #define _c2d(high, low) ((_h2i(high) << 4) | _h2i(low))

    for(unsigned long long i = 0ULL; i < until; i += 2)
    {
        if(data[i] >= 'a' && data[i] <= 'z')
        {
            last_color = "-";
            c = data[i] - 'a' + 2;
            dh = data[i+1];
            dl = data[i+2];
            i++;

            for(; c > 0; c--)
            {
                _g = _c2d(dh, dl);
                if(++n > 2)
                {
                    n = 0;
                    x++;
                }
            }
        }
        else
        {
            dh = data[i];
            dl = data[i+1];
            _g = _c2d(dh, dl);

            if(++n > 2)
            {
                n = 0;
                x++;
            }
        }

        if(x >= width)
        {
            x = 0;
            y++;
        }
    }

    return {tmp};
}

void capture(const cv::Mat &cln)
{
    static int check = 0;
    while(file_exists(std::string("capture-") + std::to_string(check) + ".dres")) check++;
    saveMatAsFile(cln, std::string("CAPTURE_") + std::to_string(check), std::string("capture-") + std::to_string(check) + ".dres");
}

void captureDepth(const cv::Mat &cln)
{
    static int check = 0;
    while(file_exists(std::string("capture-depth-") + std::to_string(check) + ".dres")) check++;
    saveMatDepth(cln, std::string("CAPTURE_DEPTH_") + std::to_string(check), std::string("capture-depth-") + std::to_string(check) + ".dres");
}
