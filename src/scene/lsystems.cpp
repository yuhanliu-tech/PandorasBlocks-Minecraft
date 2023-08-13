#include "lsystems.h"

L_Tree::L_Tree(QString a, glm::vec3 pos, int i, int d)
    :treeDude(Turtle(pos)), axiom(a), iterations(i), dist(d)
{

    drawRules['F'] = &L_Tree::forward;
    drawRules['+'] = &L_Tree::turnCCWyz;
    drawRules['-'] = &L_Tree::turnCWyz;
    drawRules['*'] = &L_Tree::turnCWyx;
    drawRules['/'] = &L_Tree::turnCCWyx;
    drawRules[']'] = &L_Tree::load;
    drawRules['['] = &L_Tree::save;
    drawRules['X'] = &L_Tree::x;
    drawRules['Y'] = &L_Tree::x;

}

void L_Tree::x() {
    // do nothing if x is not expanded
}

void L_Tree::forward() {
    glm::vec3 move;
    move.x = treeDude.dir.x * dist;
    move.y = treeDude.dir.y * dist;
    move.z = treeDude.dir.z * dist;
    treeDude.pos += move;
}

void L_Tree::turnCCWyx() {
    float angle = 3.14159265359f / 6.f;
    treeDude.dir = glm::vec3(
                glm::sin(angle) * treeDude.dir.x - glm::cos(angle) * treeDude.dir.y,
                glm::cos(angle) * treeDude.dir.x + glm::sin(angle) * treeDude.dir.y,
                treeDude.dir.z);
}

void L_Tree::turnCWyx() {
    float angle = -3.14159265359f / 6.f;
    treeDude.dir = glm::vec3(
                glm::sin(angle) * treeDude.dir.x + glm::cos(angle) * treeDude.dir.y,
                glm::cos(angle) * treeDude.dir.x - glm::sin(angle) * treeDude.dir.y,
                treeDude.dir.z);
}

void L_Tree::turnCCWyz() {
    float angle = 3.14159265359f / 6.f;
    treeDude.dir = glm::vec3(
                treeDude.dir.x,
                glm::sin(angle) * treeDude.dir.z + glm::cos(angle) * treeDude.dir.y,
                glm::cos(angle) * treeDude.dir.z - glm::sin(angle) * treeDude.dir.y);
}

void L_Tree::turnCWyz() {
    float angle = -3.14159265359f / 6.f;
    treeDude.dir = glm::vec3(
                treeDude.dir.x,
                glm::sin(angle) * treeDude.dir.z + glm::cos(angle) * treeDude.dir.y,
                glm::cos(angle) * treeDude.dir.z - glm::sin(angle) * treeDude.dir.y);
}

void L_Tree::save() {
    turtles.push(treeDude);
}

void L_Tree::load() {
    treeDude = Turtle(turtles.pop());
}

void L_Tree::generateTree() {
    QString expanded = expandTree(axiom);
    drawTree(expanded);
}

QString L_Tree::expandTree(QString a) {

    QString outStr = "";

    for (int i = 0; i < iterations; i++) {
        QString outStr = "";
        for (int c = 0; c < a.length(); c++) {
            if (a[c] == 'X') {
                outStr += "F[+X][-X][*X][/X]";
            } else if (a[c] == 'Y') {
                outStr += "[-Y][+Y][/Y][*Y]F";
            } else {
                outStr += a[c];
            }
        }
        a = outStr;
    }

    return a;

}

void L_Tree::drawTree(QString expanded) {

    branches.push_back(treeDude.pos);

    for (int c = 0; c < expanded.length(); c++) {
        auto fPtr = drawRules[expanded[c]];
        (this->*fPtr)();
        branches.push_back(treeDude.pos);
    }

}



