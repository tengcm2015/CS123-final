#include "CS123Utils.h"
#include <algorithm>
#include <cmath>
#include <cstring>

static uint8_t lerp(uint8_t a, uint8_t b, float percent) {
    float fa = static_cast<float>(a) / 255.0f;
    float fb = static_cast<float>(b) / 255.0f;
    return static_cast<uint8_t>((fa + (fb - fa) * percent) * 255.0f + 0.5f);
}

namespace CS123 { namespace UTILS {

unsigned char REAL2byte(float f) {
    auto i = static_cast<int>(std::lroundf(f * 255.0f));
    return (i < 0) ? 0 : (i > 255) ? 255 : i;
}

float byte2REAL(unsigned char b) {
    return static_cast<float>(b) / 255.0f;
}

BGRA REAL2byte(const RGBAf &c) {
    return {REAL2byte(c.r), REAL2byte(c.g), REAL2byte(c.b), 255};
}

RGBAf byte2REAL(const BGRA &c) {
    return {byte2REAL(c.r), byte2REAL(c.g), byte2REAL(c.b), 1.0f};
}

std::vector<BGRA> REAL2byte(const std::vector<RGBAf> &rgbfVec) {
    std::vector<BGRA> bgraVec;
    for (int i = 0; i < rgbfVec.size(); ++i)
        bgraVec.push_back(REAL2byte(rgbfVec[i]));
    return bgraVec;
}

std::vector<RGBAf> byte2REAL(const std::vector<BGRA> &bgraVec) {
    std::vector<RGBAf> rgbfVec;
    for (int i = 0; i < bgraVec.size(); ++i)
        rgbfVec.push_back(byte2REAL(bgraVec[i]));
    return rgbfVec;
}

unsigned char toGray(const BGRA &pixel) {
    return 0.299 * pixel.r + 0.587 * pixel.g + 0.114 * pixel.b;
}

float toGray(const RGBAf &pixel) {
    return 0.299 * pixel.r + 0.587 * pixel.g + 0.114 * pixel.b;
}

RGBAf adjustContrast(const RGBAf &pixel, float factor) {
    BGRA newPix = REAL2byte(factor * (pixel - .5f) + .5f);
    return byte2REAL(newPix);
}

BGRA mixColor(const BGRA &a, const BGRA &b, float percent) {
    return { lerp(a.r, b.r, percent),
             lerp(a.g, b.g, percent),
             lerp(a.b, b.b, percent),
             255 };
}

RGBAf mixColor(const RGBAf &a, const RGBAf &b, float percent) {
    return { a.r + (b.r - a.r) * percent,
             a.g + (b.g - a.g) * percent,
             a.b + (b.b - a.b) * percent,
             1.0f };
}

}}
