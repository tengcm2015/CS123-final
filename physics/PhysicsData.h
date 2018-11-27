#ifndef PHYSICSDATA_H
#define PHYSICSDATA_H

#include <vector>
#include <string>

#include "lib/CommonData.h"

namespace CS123 { namespace PHYSICS {

// standard gravity in m/(s^2)
const float STD_G = 9.80665;

enum class PhysicsFlag {
    FLAG_DISABLED,
    FLAG_DYNAMIC,
    FLAG_STATIC,
    FLAG_KINEMATIC
};

enum class GeometryType {
    GEOMETRY_CUBE,
    GEOMETRY_CONE,
    GEOMETRY_CYLINDER,
    GEOMETRY_TORUS,
    GEOMETRY_SPHERE,
    GEOMETRY_MESH
};

// Global physics properties
struct PhysicsGlobalData {
    float MPU; // meter per unit
    float damping;
    glm::vec3 gravity;
};

// Data for physics materials
struct PhysicsMaterial {
    float density;
    float restitution;
    float staticFriction;
    float dynamicFriction;
};

struct PhysicsObjectData {
    PhysicsFlag flag;
    GeometryType type;
    PhysicsMaterial material;
    std::vector<CS123::UTIL::Transformation> transformations;
};

}}

#endif
