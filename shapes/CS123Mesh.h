#ifndef CS123MESH_H
#define CS123MESH_H

#include "CS123Primitive.h"
#include "CS123Shape.h"


class CS123Mesh : public CS123Primitive {
public:
    explicit CS123Mesh(const glm::mat4 &transform = glm::mat4());

    ~CS123Mesh() override;

    CS123Primitive& draw() override;

protected:
    std::vector<CS123Shape> m_shapes;

    static void createUnitPolygon(std::vector<CS123Shape> &shapes, int sides, int stack);
};


#endif //CS123MESH_H
