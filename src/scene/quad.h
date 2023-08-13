#ifndef QUAD_H
#define QUAD_H

#pragma once

#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

#include "drawable.h"

class Quad : public Drawable {
public:
    Quad(OpenGLContext* context);
    void createVBOdata() override;
};


#endif // QUAD_H


