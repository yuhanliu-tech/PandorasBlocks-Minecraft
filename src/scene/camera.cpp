#include "camera.h"
#include "glm_includes.h"
#include <QApplication>
#include <QKeyEvent>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
//#include <math.h>

Camera::Camera(glm::vec3 pos)
    : Camera(400, 400, pos)
{}

Camera::Camera(unsigned int w, unsigned int h, glm::vec3 pos)
    : Entity(pos), m_fovy(45), m_wiDTh(w), m_height(h),
      m_near_clip(0.1f), m_far_clip(1000.f), m_aspect(w / static_cast<float>(h)), theta(glm::radians(0.f)), phi(glm::radians(0.f))
{}

Camera::Camera(const Camera &c)
    : Entity(c),
      m_fovy(c.m_fovy),
      m_wiDTh(c.m_wiDTh),
      m_height(c.m_height),
      m_near_clip(c.m_near_clip),
      m_far_clip(c.m_far_clip),
      m_aspect(c.m_aspect)
{
    theta = glm::radians(0.f);
    phi = glm::radians(0.f);
}

void Camera::RecomputeAttributes() {
    glm::mat4 transform = glm::mat4();
    transform = glm::rotate(transform, phi, glm::vec3(0, 1, 0));
    transform = glm::rotate(transform, theta, glm::vec3(0, 0, 1));
    m_forward = glm::vec3(transform[0]);
    m_up = glm::vec3(transform[1]);
    m_right = glm::vec3(transform[2]);
}
void Camera::setRotation(glm::mat4 &rot) {
    m_right = glm::normalize(glm::vec3(rot[0]));
    m_up = glm::normalize(glm::vec3(rot[1]));
    m_forward =glm::normalize(glm::vec3(rot[2]));
}
void Camera::changeTheta(float amt) {
    theta += glm::radians(amt);
    theta = glm::clamp(theta, (float) -M_PI * 0.5f,  (float) M_PI * 0.5f);
    RecomputeAttributes();
}
void Camera::changePhi(float amt) {
    phi += glm::radians(-amt);
    phi =fmod(phi + 2 * M_PI, 2 * M_PI);
    RecomputeAttributes();
}
glm::vec3 Camera::getForward() {
    return m_forward;
}
glm::vec3 Camera::getRight() {
    return m_right;
}
glm::vec3 Camera::getUp() {
    return m_up;
}
void Camera::setWiDThHeight(unsigned int w, unsigned int h) {
    m_wiDTh = w;
    m_height = h;
    m_aspect = w / static_cast<float>(h);
}
float Camera::getPhi() {
    return phi;
}

void Camera::tick(float DT, InputBundle &input) {
    float sx = 2 * input.mouseX / static_cast<float>(m_wiDTh) - 1;
    float sy = 1 - 2 * input.mouseY / static_cast<float>(m_height);
    float vLen = glm::tan(glm::radians(m_fovy) / 2) * 100;
    float rLen = m_aspect * vLen;
    changeTheta(glm::atan(sy) * vLen);
    changePhi(glm::atan(sx) * rLen);
}

glm::mat4 Camera::getViewProj() const {
    return glm::perspective(glm::radians(m_fovy), m_aspect, m_near_clip, m_far_clip) * glm::lookAt(m_position, m_position + m_forward, m_up);
}
void Camera::matchCamToPlayer(glm::vec3 playerPos) {
    this->m_position = playerPos + glm::vec3(0.f, 1.5f, 0.f);
}

float Camera::getNearClip() {
    return m_near_clip;
};
float Camera::getFarClip() {
    return m_far_clip;
};

float Camera::getFOVY() {
    return m_fovy;
}


