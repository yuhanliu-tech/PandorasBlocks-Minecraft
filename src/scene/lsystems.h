#ifndef LSYSTEMS_H
#define LSYSTEMS_H

#pragma once

#include "turtle.h"
#include <QStack>
#include <QChar>
#include <QString>
#include <QHash>


class L_Tree
{
public:

    typedef void (L_Tree::*Rule)(void);

    L_Tree(QString a, glm::vec3 pos, int i, int d);

    Turtle treeDude;

    QString axiom;

    int iterations;

    int dist;

    std::vector<glm::vec3> branches;

    //A std::stack or QStack of Turtles,
    //used to save and restore turtle states as you traverse your grammar string during the drawing phase.
    QStack<Turtle> turtles;

    //map from char or QChar to std::string or QString
    //represents what string a given character should expand to during the grammar expansion step
    QHash<QChar, QString> expandRules;

    //map from char or QChar to a function pointer,
    //where the function pointed to performs the drawing operation
    //for the given character.
    QHash<QChar, Rule> drawRules;

    void generateTree();

    QString expandTree(QString a);

    void drawTree(QString expanded);

    void forward();

    void turnCCWyz();
    void turnCWyz();
    void turnCCWyx();
    void turnCWyx();

    void save();
    void load();

    void x();

};

#endif // LSYSTEMS_H
