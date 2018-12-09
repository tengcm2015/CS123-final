#include "PhysicsScene.h"
#include "PhysicsObject.h"

#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate
#include <cstdlib>
#include <iostream>

using namespace std;

namespace CS123 { namespace PHYSICS {

#define EPSILON 0.1f
#define RADIUS 0.3f
#define MIN_STEP_MSEC 10

PhysicsScene::PhysicsScene()
{
}

PhysicsScene::~PhysicsScene()
{
}

float distance(glm::vec3 pos1, glm::vec3 pos2){
    float xDelta = pos1.x - pos2.x;
    float yDelta = pos1.y - pos2.y;
    float zDelta = pos1.z - pos2.z;
    return sqrt(xDelta * xDelta + yDelta * yDelta + zDelta * zDelta);
}

void PhysicsScene::simulate(int msecLapsed) {

    if (msecLapsed > MIN_STEP_MSEC) {
        this->simulate(MIN_STEP_MSEC);
        this->simulate(msecLapsed - MIN_STEP_MSEC);
        return;
    }

    // TODO: update objects' translation matrices based on physics

    for (auto &op: m_object_ptrs) {
        if (op->getFlag() != PhysicsFlag::FLAG_DYNAMIC)
            continue;

        // Collisions with the walls
        glm::vec3 oldLinearVelocity = op->getLinearVelocity();
        glm::vec3 newLinearVelocity = oldLinearVelocity;
        glm::vec3 curPos = op->getPosition().xyz();

        if ( curPos.x + RADIUS > 1 && oldLinearVelocity.x > 0){
            newLinearVelocity.x = -newLinearVelocity.x;
         }

        if ( curPos.x - RADIUS < -1  && oldLinearVelocity.x < 0){
            newLinearVelocity.x = -newLinearVelocity.x;
         }

        if ( curPos.y + RADIUS > 1  && oldLinearVelocity.y > 0){
            newLinearVelocity.y = -newLinearVelocity.y;
         }

        if ( curPos.y - RADIUS < -1 && oldLinearVelocity.y < 0){
            newLinearVelocity.y = -newLinearVelocity.y;
         }
        if ( curPos.z + RADIUS > 1 && oldLinearVelocity.z > 0){
            newLinearVelocity.z = -newLinearVelocity.z;
         }

        if ( curPos.z - RADIUS < -1 && oldLinearVelocity.z < 0){
            newLinearVelocity.z = -newLinearVelocity.z;
         }

        newLinearVelocity += m_data.gravity / m_data.MPU / 1000.0f;
        curPos = curPos + newLinearVelocity * (float) msecLapsed;

        op->m_physicsTransform = glm::translate(curPos);
        op->setLinearVelocity(newLinearVelocity);

    }

    // Collisions with other balls
    // Perfectly elastic collisions between the balls
    for(auto &op1: m_object_ptrs){

        if (op1->getFlag() != PhysicsFlag::FLAG_DYNAMIC){
            continue;
        }

        for(auto &op2: m_object_ptrs){
            if (op2->getFlag() != PhysicsFlag::FLAG_DYNAMIC){
                continue;
            }

            glm::vec3 curPos1 = op1->getPosition().xyz();
            glm::vec3 curPos2 = op2->getPosition().xyz();

            if  (curPos1 == curPos2){
                    continue;
            }

            if (distance(curPos1, curPos2) > 2 * RADIUS ){
                    continue;

            } else{

                    glm::vec3 collision = curPos1 - curPos2;

                    float dist = distance(curPos1, curPos2);

                    if (dist == 0){
                        collision = glm::vec3(1,0,0);
                    } else if (dist > 1.0){
                        return;
                    }

                    collision = collision/dist;


                    float op1_i = glm::dot(op1->getLinearVelocity(), collision);
                    float op2_i = glm::dot(op2->getLinearVelocity(), collision);

                    float op1_f = op2_i;
                    float op2_f = op1_i;

                    glm::vec3 op1_newLinearVelocity = op1->getLinearVelocity() ;
                    op1_newLinearVelocity += collision*( op1_f - op1_i);

                    glm::vec3 op2_newLinearVelocity = op2->getLinearVelocity() ;
                    op2_newLinearVelocity += collision*( op2_f - op2_i);

                    glm::vec3 curPos = op1->getPosition().xyz();
                    curPos = curPos + op1_newLinearVelocity * (float) msecLapsed;
                    op1->m_physicsTransform = glm::translate(curPos);
                    op1->setLinearVelocity(op1_newLinearVelocity);

                    curPos = op2->getPosition().xyz();
                    curPos = curPos + op2_newLinearVelocity * (float) msecLapsed;
                    op2->m_physicsTransform = glm::translate(curPos);
                    op2->setLinearVelocity(op2_newLinearVelocity);

            }


        }

    }
}


std::shared_ptr<PhysicsObject> PhysicsScene::createObject(const PhysicsObjectData &data) {
    auto obj_ptr = std::make_shared<PhysicsObject>(data);
    m_object_ptrs.push_back(obj_ptr);
    return obj_ptr;
}

}}
