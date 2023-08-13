#pragma once
#include "entity.h"
#include "camera.h"
#include "terrain.h"

class Player : public Entity {
private:
    glm::vec3 m_velocity, m_acceleration;
    //Camera m_camera;
    const Terrain &mcr_terrain;
    boolean inFlight;
    boolean isGrounded;
    std::vector<glm::vec3> boundingBox;

    float playerSpeed; //STATIC VARIABLES, different from velocity
    float flightSpeed;
    float jumpSpeed;
    float playerWeight;
    float maxVelocity;
    bool jump;
    bool swim;

    void processInputs(InputBundle &inputs);
    void computePhysics(float DT, const Terrain &terrain);
    bool checkWading();

public:
    // Readonly public reference to our camera
    // for easy access from MyGL
    const Camera& mcr_camera;
    Camera m_camera;
    float phi;

    BlockType swimmingIn;

    Player(glm::vec3 pos, const Terrain &terrain);
    virtual ~Player() override;

    void setCameraWiDThHeight(unsigned int w, unsigned int h);

    glm::vec4 getCameraPos();

    void tick(float DT, InputBundle &input) override;

    bool checkSubmerged();

    // Player overrides all of Entity's movement
    // functions so that it transforms its camera
    // by the same amount as it transforms itself.
    void moveAlongVector(glm::vec3 dir) override;
    void moveForwardLocal(float amount) override;
    void moveRightLocal(float amount) override;
    void moveUpLocal(float amount) override;
    void moveForwardGlobal(float amount) override;
    void moveRightGlobal(float amount) override;
    void moveUpGlobal(float amount) override;
    void rotateOnForwardLocal(float degrees) override;
    void rotateOnRightLocal(float degrees) override;
    void rotateOnUpLocal(float degrees) override;
    void rotateOnForwardGlobal(float degrees) override;
    void rotateOnRightGlobal(float degrees) override;
    void rotateOnUpGlobal(float degrees) override;
    void toggleFlight();
    void toggleGrounded();

    void setBoundingBox();
    bool gridMarch(glm::vec3 rayOrigin, glm::vec3 rayDirection,
                   const Terrain &terrain, float *out_dist,
                   glm::ivec3 *out_blockHit);

    bool gridMarchBlockBefore(glm::vec3 rayOrigin, glm::vec3 rayDirection,
                   const Terrain &terrain, glm::ivec3 *out_blockHit);

    bool removeBlock(Terrain *terrain);
    bool addBlock(Terrain *terrain);

    glm::vec3 getPosition();

    float getNearestBlockDist(Terrain *terrain);


    Camera getCamera();
    // For sending the Player's data to the GUI
    // for display
    QString posAsQString() const;
    QString velAsQString() const;
    QString accAsQString() const;
    QString lookAsQString() const;
};
