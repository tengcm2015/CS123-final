#ifndef PHYSICSDATA_H
#define PHYSICSDATA_H

#include <vector>
#include <string>

#include "lib/CommonData.h"

namespace CS123 { namespace PHYSICS {

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
    GEOMETRY_PLANE,
    GEOMETRY_MESH
};

// Global physics properties
struct PhysicsGlobalData {
    float damping = 0.0f;
    glm::vec3 gravity {0.0f, -0.001f, 0.0f};
};

// Data for physics materials
struct PhysicsMaterial {
    float density = 1.0f;
    float restitution = 1.0f;
    float staticFriction = 0.0f;
    float dynamicFriction = 0.0f;
};

struct PhysicsObjectData {
    PhysicsFlag flag = PhysicsFlag::FLAG_DYNAMIC;
    GeometryType type = GeometryType::GEOMETRY_SPHERE;
    PhysicsMaterial material;
    std::vector<CS123Transformation> transformations;
};

}}

#endif
