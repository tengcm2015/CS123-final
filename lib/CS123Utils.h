#ifndef CS123UTILS_H
#define CS123UTILS_H

#include "BGRA.h"
#include "CommonData.h"
#include "glm/glm.hpp"

#include <vector>

namespace CS123 { namespace UTILS {

template <typename Enumeration>
auto as_integer(Enumeration const value)
-> typename std::underlying_type< Enumeration >::type
{
    return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

unsigned char REAL2byte(float f);
float byte2REAL(unsigned char b);

BGRA REAL2byte(const RGBAf &c);
RGBAf byte2REAL(const BGRA &c);

std::vector<BGRA> REAL2byte(const std::vector<RGBAf> &rgbfVec);
std::vector<RGBAf> byte2REAL(const std::vector<BGRA> &bgraVec);

unsigned char toGray(const BGRA &pixel);
float toGray(const RGBAf &pixel);

RGBAf adjustContrast(const RGBAf &pixel, float factor);

BGRA  mixColor(const BGRA&  a, const BGRA&  b, float percent);
RGBAf mixColor(const RGBAf& a, const RGBAf& b, float percent);

}}

#endif // CS123UTILS_H
