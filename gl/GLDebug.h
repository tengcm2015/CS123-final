#ifndef GLDEBUG_H
#define GLDEBUG_H

#include <string>
#include "GL/glew.h"

namespace CS123 { namespace GL {

    void checkGLError(const std::string &errmsg = "");
    void printGLErrorCodeInEnglish(GLenum err);

    void checkFramebufferStatus();
    void printFramebufferErrorCodeInEnglish(GLenum err);

    void checkShaderCompilationStatus(GLuint shaderID);
    void checkShaderLinkStatus(GLuint programID);

}}

#endif // GLDEBUG_H
