#ifndef FULLSCREENQUAD_H
#define FULLSCREENQUAD_H

#include <memory>
#include "datatype/VAO.h"

namespace CS123 { namespace GL {

class FullScreenQuad {
public:
    FullScreenQuad();

    void draw();

private:
    std::unique_ptr<VAO> m_vao;
};

}}
#endif // FULLSCREENQUAD_H
