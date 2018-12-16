#ifndef PHYSICSSCENE_H
#define PHYSICSSCENE_H

#include <memory>

#include "PhysicsData.h"

namespace CS123 { namespace PHYSICS {

class PhysicsObject;

class PhysicsScene {
public:
    PhysicsScene();

    virtual ~PhysicsScene();

    PhysicsGlobalData getGlobal() { return m_data; }
    void setGlobal(const PhysicsGlobalData &data) { m_data = data; }

    void simulate(int msecLapsed = 0);

    std::shared_ptr<PhysicsObject> createObject(const PhysicsObjectData &data);

    void clearObjects();

private:
    PhysicsGlobalData m_data;

    std::vector<std::shared_ptr<PhysicsObject>> m_object_ptrs;

    void simulate_sphere(int msecLapsed);

    void simulate_box(int msecLapsed);
};

}}

#endif //PHYSICSSCENE_H
