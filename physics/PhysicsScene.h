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

    void setGlobal(const PhysicsGlobalData &data) { m_data = data; }

    void simulate(int msecLapsed = 0);

    std::shared_ptr<PhysicsObject> createObject(const PhysicsObjectData &data);

private:
    PhysicsGlobalData m_data;

    std::vector<std::shared_ptr<PhysicsObject>> m_object_ptrs;
};

}}

#endif //PHYSICSSCENE_H
