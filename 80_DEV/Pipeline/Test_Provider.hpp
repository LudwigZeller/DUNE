//
// Created by eragon on 11/18/22.
//

#ifndef DUNE_PIPELINE_TEST_WORKER_PROVIDER_HPP
#define DUNE_PIPELINE_TEST_WORKER_PROVIDER_HPP

#include "Provider.hpp"
#include <chrono>
#include <thread>
#include <utility>

cv::Mat assetToMat(unsigned int width, unsigned int height, const char *data) {
    // Allocated data init
    cv::Mat tmp = cv::Mat(cv::Size(width, height), CV_8UC3);
    uchar *dat = (uchar *) tmp.ptr<uchar[3]>();
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

    for (unsigned long long i = 0ULL; i < until; i += 2) {
        if (data[i] >= 'a' && data[i] <= 'z') {
            last_color = "-";
            c = data[i] - 'a' + 2;
            dh = data[i + 1];
            dl = data[i + 2];
            i++;

            for (; c > 0; c--) {
                _g = _c2d(dh, dl);
                if (++n > 2) {
                    n = 0;
                    x++;
                }
            }
        } else {
            dh = data[i];
            dl = data[i + 1];
            _g = _c2d(dh, dl);

            if (++n > 2) {
                n = 0;
                x++;
            }
        }

        if (x >= width) {
            x = 0;
            y++;
        }
    }

    return {tmp};
}
#include "../lol/capture-0.dres"

class Test_Provider : public Provider {
public:
    explicit Test_Provider(std::string id) : Provider(std::move(id)) {};

    void work() override {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
        m_work_matrix = std::move(assetToMat(CAPTURE_0_WIDTH,CAPTURE_0_HEIGHT,CAPTURE_0_DATA));
        clog(info) << get_id() <<" provided Matrix!" << std::endl;
    };
};

#endif //DUNE_PIPELINE_TEST_WORKER_PROVIDER_HPP
