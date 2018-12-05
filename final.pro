# -------------------------------------------------
# Project created by QtCreator 2010-08-22T14:12:19
# -------------------------------------------------
QT += opengl xml
TARGET = final
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++14
CONFIG += c++14

unix:!macx {
    LIBS += -lGLU
}
macx {
    QMAKE_CFLAGS_X86_64 += -mmacosx-version-min=10.7
    QMAKE_CXXFLAGS_X86_64 = $$QMAKE_CFLAGS_X86_64
    CONFIG += c++11
}
win32 {
    DEFINES += GLEW_STATIC
    LIBS += -lopengl32 -lglu32
}

SOURCES += main.cpp \
    glew-1.10.0/src/glew.c \
    ui/mainwindow.cpp \
    ui/SceneBuilder.cpp \
    ui/view.cpp \
    lib/BGRA.cpp \
    lib/ResourceLoader.cpp \
    shapes/CS123Shape.cpp \
    shapes/CS123Mesh.cpp \
    shapes/CS123Cube.cpp \
    shapes/CS123Cone.cpp \
    shapes/CS123Cylinder.cpp \
    shapes/CS123Torus.cpp \
    shapes/CS123Sphere.cpp \
    shapes/CS123Plane.cpp \
    scene/Scene.cpp \
    scene/SceneCamera.cpp \
    scene/SceneObject.cpp \
    scene/ScenePrimitive.cpp \
    gl/GLDebug.cpp \
    gl/shaders/Shader.cpp \
    gl/shaders/CS123Shader.cpp \
    gl/textures/Texture.cpp \
    gl/textures/Texture2D.cpp \
    gl/textures/TextureParameters.cpp \
    gl/textures/TextureParametersBuilder.cpp \
    gl/textures/RenderBuffer.cpp \
    gl/textures/DepthBuffer.cpp \
    gl/OpenGLShape.cpp \
    gl/datatype/VBOAttribMarker.cpp \
    gl/datatype/VBO.cpp \
    gl/datatype/IBO.cpp \
    gl/datatype/VAO.cpp \
    gl/datatype/FBO.cpp \
    physics/PhysicsObject.cpp \
    physics/PhysicsScene.cpp \


HEADERS += ui_mainwindow.h \
    glew-1.10.0/include/GL/glew.h \
    ui/mainwindow.h \
    ui/SceneBuilder.h \
    ui/view.h \
    shapes/CS123Shape.h \
    shapes/CS123Mesh.h \
    shapes/CS123Cube.h \
    shapes/CS123Cone.h \
    shapes/CS123Cylinder.h \
    shapes/CS123Torus.h \
    shapes/CS123Sphere.h \
    shapes/CS123Plane.cpp \
    shapes/CS123Primitive.h \
    lib/BGRA.h \
    lib/ResourceLoader.h \
    lib/CommonData.h \
    scene/Scene.h \
    scene/SceneCamera.h \
    scene/SceneData.h \
    scene/SceneObject.h \
    scene/ScenePrimitive.h \
    gl/GLDebug.h \
    gl/shaders/Shader.h \
    gl/shaders/CS123Shader.h \
    gl/shaders/ShaderAttribLocations.h \
    gl/textures/Texture.h \
    gl/textures/Texture2D.h \
    gl/textures/TextureParameters.h \
    gl/textures/TextureParametersBuilder.h \
    gl/textures/RenderBuffer.h \
    gl/textures/DepthBuffer.h \
    gl/OpenGLShape.h \
    gl/datatype/VBOAttribMarker.h \
    gl/datatype/VBO.h \
    gl/datatype/IBO.h \
    gl/datatype/VAO.h \
    gl/datatype/FBO.h \
    physics/PhysicsData.h \
    physics/PhysicsObject.h \
    physics/PhysicsScene.h \


FORMS += ui/mainwindow.ui
INCLUDEPATH += glm ui glew-1.10.0/include
DEPENDPATH += glm ui glew-1.10.0/include

DEFINES += _USE_MATH_DEFINES
DEFINES += TIXML_USE_STL
DEFINES += GLM_SWIZZLE GLM_FORCE_RADIANS
OTHER_FILES += \
    shaders/phong.frag \
    shaders/phong.vert \

# Don't add the -pg flag unless you know what you are doing. It makes QThreadPool freeze on Mac OS X
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_CXXFLAGS_WARN_ON -= -Wall
QMAKE_CXXFLAGS_WARN_ON += -Waddress -Warray-bounds -Wc++0x-compat -Wchar-subscripts -Wformat\
                          -Wmain -Wmissing-braces -Wparentheses -Wreorder -Wreturn-type \
                          -Wsequence-point -Wsign-compare -Wstrict-overflow=1 -Wswitch \
                          -Wtrigraphs -Wuninitialized -Wunused-label -Wunused-variable \
                          -Wvolatile-register-var -Wno-extra

QMAKE_CXXFLAGS += -g

# QMAKE_CXX_FLAGS_WARN_ON += -Wunknown-pragmas -Wunused-function -Wmain

macx {
    QMAKE_CXXFLAGS_WARN_ON -= -Warray-bounds -Wc++0x-compat
}

RESOURCES += \
    resources.qrc
