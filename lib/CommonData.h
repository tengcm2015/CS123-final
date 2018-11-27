#ifndef COMMONDATA_H
#define COMMONDATA_H

#include "glm/glm.hpp"

namespace CS123 { namespace UTIL {

template <typename Enumeration>
auto as_integer(Enumeration const value)
-> typename std::underlying_type< Enumeration >::type
{
    return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

// Enumeration for types of transformations that can be applied to objects, lights, and cameras.
enum class TransformationType {
    TRANSFORMATION_TRANSLATE, TRANSFORMATION_SCALE, TRANSFORMATION_ROTATE, TRANSFORMATION_MATRIX
};

// Data for transforming a scene object. Aside from the TransformationType, the remaining of the
// data in the struct is mutually exclusive.
struct Transformation {
    TransformationType type;

    glm::vec3 translate; // The translation vector. Only valid if transformation is a translation.
    glm::vec3 scale;     // The scale vector. Only valid if transformation is a scale.
    glm::vec3 rotate;    // The axis of rotation. Only valid if the transformation is a rotation.
    float angle;         // The rotation angle in RADIANS. Only valid if transformation is a
    // rotation.

    glm::mat4x4 matrix;  // The matrix for the transformation. Only valid if the transformation is
    // a custom matrix.
};

}}

#endif //COMMONDATA_H
