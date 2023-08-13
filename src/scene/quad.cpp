#include "quad.h"

Quad::Quad(OpenGLContext* context) : Drawable(context) {

}

void Quad::createVBOdata() {

    GLuint idx[6]{0,1,2,0,2,3};
    m_countOpaque = 6;

    // define coordinates in normalized screen space
    glm::vec4 pos[4] = {
        glm::vec4(-1.f, -1.f, 1.f, 1.f),
        glm::vec4(1.f, -1.f, 1.f, 1.f),
        glm::vec4(1.f, 1.f, 1.f, 1.f),
        glm::vec4(-1.f, 1.f, 1.f, 1.f)
    };

    glm::vec2 uv[4] = {
        glm::vec2(0.f, 0.f),
        glm::vec2(1.f, 0.f),
        glm::vec2(1.f, 1.f),
        glm::vec2(0.f, 1.f)
    };

    generateOpaqueIdx();
    bindOpaqueIdx();
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), idx, GL_STATIC_DRAW);

    generatePos();
    bindPos();
    mp_context->glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec4), pos, GL_STATIC_DRAW);

    generateUV();
    bindUV();
    mp_context->glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), uv, GL_STATIC_DRAW);

}
