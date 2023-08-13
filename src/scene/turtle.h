#ifndef TURTLE_H
#define TURTLE_H

#pragma once

#include "la.h"

// A Turtle class that tracks the position, orientation
// Recall that a turtle is simply a state marker for drawing graphic elements, such as simple lines.

class Turtle
{
public:

    glm::vec3 pos;
    glm::vec3 dir;

    Turtle(glm::vec3 newPos);
    Turtle();
    Turtle(const Turtle& t);
};

#endif // TURTLE_H
