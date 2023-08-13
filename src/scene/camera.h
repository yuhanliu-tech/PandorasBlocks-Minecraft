#pragma once
#include "glm_includes.h"
#include "scene/entity.h"

//A perspective projection camera
//Receives its eye position and reference point from the scene XML file
class Camera : public Entity {
private:
    float m_fovy;
    unsigned int m_wiDTh, m_height;  // Screen dimensions
    float m_near_clip;  // Near clip plane distance
    float m_far_clip;  // Far clip plane distance
    float m_aspect;    // Aspect ratio


public:
    float theta, phi;

    Camera(glm::vec3 pos);
    Camera(unsigned int w, unsigned int h, glm::vec3 pos);
    Camera(const Camera &c);
    void setWiDThHeight(unsigned int w, unsigned int h);

    void tick(float DT, InputBundle &input) override;

    glm::mat4 getViewProj() const;

    void RecomputeAttributes();

    void changePhi(float amt);
    void changeTheta(float amt);
    float getPhi();
    void matchCamToPlayer(glm::vec3 playerPos);

    void setRotation(glm::mat4 &rot);
    glm::vec3 getForward();
    glm::vec3 getRight();
    glm::vec3 getUp();

    float getNearClip();
    float getFarClip();
    float getFOVY();



};
