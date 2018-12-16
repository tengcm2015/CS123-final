#include "PhysicsScene.h"
#include "PhysicsObject.h"
#include "ui/Settings.h"
#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate
#include <cstdlib>
#include <iostream>
#include <cmath>
#define MIN_STEP_MSEC 50

using namespace std;

namespace CS123 { namespace PHYSICS {



PhysicsScene::PhysicsScene()
{
}

PhysicsScene::~PhysicsScene()
{
}

float distanceNextFrame(glm::vec3 op1_curPos,glm::vec3 op1_vel, float op1_radius, glm::vec3 op2_curPos, glm::vec3 op2_vel, float op2_radius){
    return sqrt(       pow((op1_curPos.x + op1_vel.x - op2_curPos.x - op2_vel.x), 2)
                     + pow((op1_curPos.y + op1_vel.y - op2_curPos.y - op2_vel.y), 2)
                     + pow((op1_curPos.z + op1_vel.z - op2_curPos.z - op2_vel.z), 2)  ) - op1_radius - op2_radius;
}

float calculate_ball_radius(glm::mat4 transform){
    float scaling = glm::length(glm::vec3(transform[0][0], transform[1][0], transform[2][0]));
    return scaling/2.f;
}

void PhysicsScene::simulate_sphere(int msecLapsed){
    for(auto &op1: m_object_ptrs){

            if (op1->getFlag() != PhysicsFlag::FLAG_DYNAMIC)
                continue;

            glm::vec3 curPos1 = op1->getPosition().xyz();
            float op1_radius = calculate_ball_radius(op1->m_physicsTransform);
            float op1_mass = op1->m_data.material.density * 4/3 * M_PI* std::pow(op1_radius, 3);
            glm::vec3 op1_newLinearVelocity = op1->getLinearVelocity();

            curPos1 += op1_newLinearVelocity* (float) msecLapsed;
            for(auto &op2: m_object_ptrs){
                if (op2->getFlag() != PhysicsFlag::FLAG_DYNAMIC){
                    continue;
                }
                glm::vec3 curPos2 = op2->getPosition().xyz();
                float op2_radius = calculate_ball_radius(op2->m_physicsTransform);
                float op2_mass = op2->m_data.material.density * 4/3 * M_PI* std::pow(op2_radius, 3);

                if  ((curPos1 != curPos2)   && glm::distance(curPos1, curPos2) <= op1_radius + op2_radius){
                        glm::vec3 collision = curPos1 - curPos2;
                        float dist = glm::distance(curPos1, curPos2);

                        if (dist == 0){
                            collision = glm::vec3(1,0,0);
                        } else if (dist > 1.0){
                            return;
                        }

                        collision = collision/dist;

                        if ( glm::dot(op1_newLinearVelocity, collision) > 0)
                                continue;

                        curPos1 -= collision*(op1_radius + op2_radius - dist);

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
                        op1->m_physicsTransform = glm::translate(curPos) * glm::scale(glm::vec3(op1_radius*2.f));
                        op1->setLinearVelocity(op1_newLinearVelocity);

                        curPos = op2->getPosition().xyz();
                        curPos = curPos + op2_newLinearVelocity * (float) msecLapsed;
                        op2->m_physicsTransform = glm::translate(curPos) *glm::scale(glm::vec3(op2_radius*2.f));
                        op2->setLinearVelocity(op2_newLinearVelocity);
                }
            }
        }
}

void PhysicsScene::simulate_box(int msecLapsed){
    for (auto &op: m_object_ptrs) {
        if (op->getFlag() != PhysicsFlag::FLAG_DYNAMIC)
            continue;

        float radius = calculate_ball_radius(op->m_physicsTransform);
        float C_R = op->m_data.material.restitution;

        glm::vec3 oldLinearVelocity = op->getLinearVelocity();
        glm::vec3 newLinearVelocity = oldLinearVelocity;
        glm::vec3 curPos = op->getPosition().xyz();

        curPos += oldLinearVelocity * (float) msecLapsed;

        if (curPos.y + radius > 1) {
                curPos.y = 1 - radius ;
        }
        if (curPos.y - radius < -1) {
                curPos.y = radius -1;
        }
        if (curPos.x + radius > 1) {
                curPos.x = 1 - radius;
        }
        if (curPos.x - radius < -1) {
                curPos.x = radius -1;
        }
        if (curPos.z + radius > 1) {
                curPos.z = 1 - radius;
        }
        if (curPos.z - radius < -1) {
                curPos.z = radius -1;
        }

        if ( curPos.x + radius >= 1) {
            newLinearVelocity.x = -C_R * newLinearVelocity.x;
         }

        if ( curPos.x - radius <= -1) {
            newLinearVelocity.x = -C_R *newLinearVelocity.x;
         }

        if ( curPos.y + radius >= 1 ){
            newLinearVelocity.y = -C_R *newLinearVelocity.y;
         }

        if ( curPos.y - radius <= -1 ){
            newLinearVelocity.y = -C_R *newLinearVelocity.y;
         }
        if ( curPos.z + radius >= 1 ){
            newLinearVelocity.z = -C_R *newLinearVelocity.z;
         }

        if ( curPos.z - radius <= -1 ){
            newLinearVelocity.z = -C_R *newLinearVelocity.z;
         }


        newLinearVelocity += m_data.gravity * (float) msecLapsed; // gravity is right unit
        op->m_physicsTransform = glm::translate(curPos) * glm::scale(glm::vec3(radius*2.f));
        op->setLinearVelocity(newLinearVelocity);

    }
}

void PhysicsScene::simulate(int msecLapsed) {

    if (msecLapsed > MIN_STEP_MSEC) {
        this->simulate(MIN_STEP_MSEC);
        this->simulate(msecLapsed - MIN_STEP_MSEC);
        return;
    }

    if ( settings.enableCollision){
        simulate_sphere(msecLapsed);
    }
    simulate_box(msecLapsed);


}


std::shared_ptr<PhysicsObject> PhysicsScene::createObject(const PhysicsObjectData &data) {
    auto obj_ptr = std::make_shared<PhysicsObject>(data);
    m_object_ptrs.push_back(obj_ptr);
    return obj_ptr;
}

void PhysicsScene::clearObjects() {
    m_object_ptrs.clear();
}

}}
