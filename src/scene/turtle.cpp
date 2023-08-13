#include "turtle.h"
#define GLM_FORCE_RADIANS

// A Turtle class that tracks the position,
// orientation, and (possibly)
// "recursion depth" of your drawing turtle.

// Recall that a turtle is simply a state marker for drawing graphic elements, such as simple lines.

Turtle::Turtle()
    :pos(glm::vec3(0.f, 0.f, 0.f)), dir(glm::vec3(0.f, 1.f, 0.f))
{}

Turtle::Turtle(glm::vec3 newPos)
    :pos(newPos), dir(glm::vec3(0.f, 1.f, 0.f))
{}

Turtle::Turtle(const Turtle& t)
    :pos(t.pos), dir(t.dir)
{}

//void Turtle::turn(float rot) {
//    float angle = 3.14159265359f * 0.5f / rot;
//    orientation = glm::vec2(
//                glm::cos(angle) * orientation.x - glm::sin(angle) * orientation.y,
//                glm::sin(angle) * orientation.x + glm::cos(angle) * orientation.y);
//}

//void Turtle::move() {
//    pos += (orientation * dist);
//}
