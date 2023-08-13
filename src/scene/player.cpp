#include "player.h"
#include "glm/gtx/euler_angles.hpp"
#include <QString>
#include <iostream>
#include <QDebug>

Player::Player(glm::vec3 pos, const Terrain &terrain)
    : Entity(pos), m_velocity(0,0,0), m_acceleration(0,0,0),
      m_camera(pos + glm::vec3(0, 1.5f, 0)), mcr_terrain(terrain), swimmingIn(EMPTY),
      inFlight(true), isGrounded(false), mcr_camera(m_camera), playerSpeed(0.5), playerWeight(0.001), maxVelocity(3.0), flightSpeed(2), jumpSpeed(0.02), jump(false), swim(false)
{
    setBoundingBox();
}

Player::~Player()
{}

// check if player is underwater or underlava
bool Player::checkSubmerged() {

        glm::vec3 aboveHead = this->m_position + glm::vec3(0.5f, 1.5f, 0.5f);
        for (int x = 0; x <= 1; x++) {
            for (int z = 0; z <= 1; z++) {
                glm::vec3 blockCoords = glm::vec3(floor(aboveHead.x) + x, floor(aboveHead.y - 0.005f), floor(aboveHead.z) + z);
                if (mcr_terrain.getBlockAt(blockCoords) == WATER) {
                    swimmingIn = WATER;
                    return true;
                }
                if (mcr_terrain.getBlockAt(blockCoords) == LAVA) {
                    swimmingIn = LAVA;
                    return true;
                }
            }
        }
        return false;
}

void Player::toggleFlight() {
    inFlight = !inFlight;
    if (inFlight) {
        isGrounded = false;
    }
}
void Player::setBoundingBox() {
    this->boundingBox.clear();
    for (int i = 0; i < 3; i++) {
        boundingBox.push_back(glm::vec3(m_position.x - 0.4f,
                                        m_position.y + i,
                                        m_position.z - 0.4f));
        boundingBox.push_back(glm::vec3(m_position.x + 0.4f,
                                        m_position.y + i,
                                        m_position.z + 0.4f));
        boundingBox.push_back(glm::vec3(m_position.x + 0.4f,
                                        m_position.y + i,
                                        m_position.z - 0.4f));
        boundingBox.push_back(glm::vec3(m_position.x - 0.4f,
                                        m_position.y + i,
                                        m_position.z + 0.4f));
    }
}
void Player::tick(float DT, InputBundle &input) {
    this->m_camera.tick(DT, input);
    glm::mat4 rot = glm::rotate(glm::mat4(), this->m_camera.getPhi(), glm::vec3(0, 1, 0));
    this->m_right = glm::vec3(rot * glm::vec4(0, 0, 1, 0));
    this->m_forward = glm::vec3(rot * glm::vec4(1, 0, 0, 0));

    processInputs(input);
    computePhysics(DT, mcr_terrain);

    this->m_camera.matchCamToPlayer(this->m_position);
}
// Update the Player's velocity and acceleration based on the
// state of the inputs.
void Player::processInputs(InputBundle &inputs) {
    float relevantSpeed = playerSpeed;
    if(inFlight) {
        relevantSpeed = flightSpeed;
    }
    glm::vec3 localAcc = glm::vec3(0.f);

    // RESET TO ORIGIN when o pressed hehe
    if (inputs.oPressed) {
        this->m_position = glm::vec3(48.f, 140.f, 48.f);
    }

    // FORWARD/BACKWARDS MOVEMENT (w or s)
    if (inputs.wPressed) {
        localAcc.z += relevantSpeed;
    } if (inputs.sPressed) {
        localAcc.z -= relevantSpeed;
    }

    //LEFTWARD AND RIGHTWARD MOVEMENT (a or d)
    if (inputs.dPressed) {
        localAcc.x += relevantSpeed;
    } if (inputs.aPressed) {
        localAcc.x -= relevantSpeed;
    }

    // UPWARD AND DOWNARD MOVEMENT (e or q) - flight only
    if ((inputs.ePressed || inputs.spacePressed) && inFlight) {
        localAcc.y += relevantSpeed;
    } else if (inputs.spacePressed && !inFlight && checkSubmerged()) {
        if (swim == false) {
            swim = true;
        }
        this -> isGrounded = false;
    } else if (inputs.spacePressed && !inFlight && isGrounded) { //Seems to enter this loop
        if(jump == false) {
            jump = true;
        }
        this -> isGrounded = false;
    }

    if (checkSubmerged() && !inputs.spacePressed) {
        swim = false;
    }

    if (inputs.qPressed && inFlight) {
        localAcc.y -= relevantSpeed;
    }
    localAcc *= 0.0002f;
    this->m_acceleration = localAcc.x * m_right + localAcc.y * m_up + localAcc.z * m_forward;
}

// Update the Player's position based on its acceleration
// and velocity, and also perform collision detection.
void Player::computePhysics(float dT, const Terrain &terrain) {
    if (!inFlight && !isGrounded && !swim) {
        this->m_acceleration.y -= playerWeight * 0.5;
    }

    if (!checkSubmerged()) {
        swim = false;
    }

    if (swim) {
        this->m_velocity += glm::vec3(0, jumpSpeed * 0.66, 0);
    } else if(jump) {
        jump = false;
        this->m_velocity += glm::vec3(0, jumpSpeed * 10., 0);
    }

    if(!inFlight && !checkSubmerged()) {
        this->m_velocity.y += this->m_acceleration.y * dT;
    } else {
        this->m_velocity.y = glm::clamp(m_velocity.y + this->m_acceleration.y * dT, -maxVelocity, maxVelocity);
    }

    //clamping logic
    float anticipatedXVelocity = this->m_velocity.x + this->m_acceleration.x * dT;
    float anticipatedZVelocity = this->m_velocity.z + this->m_acceleration.z * dT;
    this->m_velocity.z += this->m_acceleration.z * dT;

    if(std::abs(glm::length(glm::vec2(anticipatedXVelocity, anticipatedZVelocity))) < maxVelocity) { //clamping logic
        this->m_velocity.x += this->m_acceleration.x * dT;
        this->m_velocity.z += this->m_acceleration.z * dT;
    }

    this->m_velocity *= 0.6f;

    if (checkSubmerged() || terrain.getBlockAt(m_position) == WATER || terrain.getBlockAt(m_position) == LAVA) {
        m_velocity *= 0.2f;
    }

    glm::vec3 displacement = this->m_velocity * dT;

    // collision
    setBoundingBox();
    if (!inFlight) {
        float outDist = 0.f;
        glm::ivec3 outBlock = glm::ivec3();
        glm::vec3 currBoxPos;
        isGrounded = false;

        for (int ind = 0; ind < boundingBox.size(); ind++) {
            currBoxPos = boundingBox[ind];
            if (gridMarch(currBoxPos, glm::vec3(glm::sign(displacement.x), 0, 0) * 0.02f, terrain, &outDist, &outBlock)) {
                displacement.x = 0.f;
            }
            if (gridMarch(currBoxPos, glm::vec3(0, 0, glm::sign(displacement.z)) * 0.02f, terrain, &outDist, &outBlock)) {
                displacement.z = 0.f;
            }
            if (gridMarch(currBoxPos, glm::vec3(0, glm::sign(displacement.y), 0) * 0.02f, terrain, &outDist, &outBlock)) {
                displacement.y = 0.f;
                isGrounded = true;
            }
        }

        for (int ind = 0; ind < boundingBox.size(); ind++) {
            if (gridMarch(this->m_position, displacement, terrain, &outDist, &outBlock)) {
                displacement = glm::normalize(displacement) * (outDist - 0.01f);
            }
        }
    }
    this->moveAlongVector(displacement);
}

bool Player::gridMarch(glm::vec3 rayOrigin, glm::vec3 rayDirection,
                       const Terrain &terrain, float *out_dist,
                       glm::ivec3 *out_blockHit) {
    float maxLen = glm::length(rayDirection); // Farthest we search
    glm::ivec3 currCell = glm::ivec3(glm::floor(rayOrigin));
    rayDirection = glm::normalize(rayDirection); // Now all t values represent world dist.

    float curr_t = 0.f;
    while(curr_t < maxLen) {
        float min_t = glm::sqrt(3.f);
        float interfaceAxis = -1; // Track axis for which t is smallest
        for(int i = 0; i < 3; ++i) { // Iterate over the three axes
            if(rayDirection[i] != 0) { // Is ray parallel to axis i?
                float offset = glm::max(0.f, glm::sign(rayDirection[i])); // See slide 5
                // If the player is *exactly* on an interface then
                // they'll never move if they're looking in a negative direction
                if(currCell[i] == rayOrigin[i] && offset == 0.f) {
                    offset = -1.f;
                }
                int nextIntercept = currCell[i] + offset;
                float axis_t = (nextIntercept - rayOrigin[i]) / rayDirection[i];
                axis_t = glm::min(axis_t, maxLen); // Clamp to max len to avoid super out of bounds errors
                if(axis_t < min_t) {
                    min_t = axis_t;
                    interfaceAxis = i;
                }
            }
        }
        if(interfaceAxis == -1) {
            throw std::out_of_range("interfaceAxis was -1 after the for loop in griDMarch!");
        }
        curr_t += min_t; // min_t is declared in slide 7 algorithm
        rayOrigin += rayDirection * min_t;
        glm::ivec3 offset = glm::ivec3(0,0,0);
        // Sets it to 0 if sign is +, -1 if sign is -
        offset[interfaceAxis] = glm::min(0.f, glm::sign(rayDirection[interfaceAxis]));
        // nextcell
        currCell = glm::ivec3(glm::floor(rayOrigin)) + offset;
        // If currCell contains something other than EMPTY, return
        // curr_t
        BlockType cellType = terrain.getBlockAt(currCell.x, currCell.y, currCell.z);
        // don't bump into transparent blocks
        if(cellType != EMPTY && cellType != LAVA && cellType != WATER && cellType != S_GRASS && cellType != LEAVES && cellType != SEA_GRASS
                && cellType != CAVE_PLANT && cellType != CAVE_VINE && cellType != CAVE_PLANT_SMOL
                && cellType != RED_PLANT && cellType != GREEN_PLANT && cellType != YELLOW_PLANT) {
            *out_blockHit = currCell; // return this
            *out_dist = glm::min(maxLen, curr_t);
            return true;
        }
        // update currcel
    }
    *out_dist = glm::min(maxLen, curr_t);
    return false;
}

bool Player::gridMarchBlockBefore(glm::vec3 rayOrigin, glm::vec3 rayDirection,
                                  const Terrain &terrain, glm::ivec3 *out_blockHit) {
    float maxLen = glm::length(rayDirection); // Farthest we search
    glm::ivec3 currCell = glm::ivec3(glm::floor(rayOrigin));
    rayDirection = glm::normalize(rayDirection); // Now all t values represent world dist.

    float curr_t = 0.f;
    while(curr_t < maxLen) {
        float min_t = glm::sqrt(3.f);
        float interfaceAxis = -1; // Track axis for which t is smallest
        for(int i = 0; i < 3; ++i) { // Iterate over the three axes
            if(rayDirection[i] != 0) { // Is ray parallel to axis i?
                float offset = glm::max(0.f, glm::sign(rayDirection[i])); // See slide 5
                // If the player is *exactly* on an interface then
                // they'll never move if they're looking in a negative direction
                if(currCell[i] == rayOrigin[i] && offset == 0.f) {
                    offset = -1.f;
                }
                int nextIntercept = currCell[i] + offset;
                float axis_t = (nextIntercept - rayOrigin[i]) / rayDirection[i];
                axis_t = glm::min(axis_t, maxLen); // Clamp to max len to avoid super out of bounds errors
                if(axis_t < min_t) {
                    min_t = axis_t;
                    interfaceAxis = i;
                }
            }
        }
        if(interfaceAxis == -1) {
            throw std::out_of_range("interfaceAxis was -1 after the for loop in griDMarch!");
        }
        curr_t += min_t; // min_t is declared in slide 7 algorithm
        rayOrigin += rayDirection * min_t;
        glm::ivec3 offset = glm::ivec3(0,0,0);
        // Sets it to 0 if sign is +, -1 if sign is -
        offset[interfaceAxis] = glm::min(0.f, glm::sign(rayDirection[interfaceAxis]));
        if (!(_isnan(rayOrigin.x) || _isnan(rayOrigin.y) || _isnan(rayOrigin.z))) {
            glm::ivec3 nextCell = glm::ivec3(glm::floor(rayOrigin)) + offset;
           // currCell = glm::ivec3(glm::floor(rayOrigin)) + offset;
            // If currCell contains something other than EMPTY, return
            // curr_t
            //std::cout << "rayOrigin: " << rayOrigin.x << " " << rayOrigin.y << " " << rayOrigin.z << std::endl ;
            //std::cout << "currCell: " << currCell.x << " " << currCell.y << " " << currCell.z << std::endl ;
            BlockType cellType = terrain.getBlockAt(nextCell.x, nextCell.y, nextCell.z);
            if(cellType != EMPTY) {
                *out_blockHit = currCell;
                return true;
            }
            currCell = nextCell;
        }
    }
    // *out_dist = glm::min(maxLen, curr_t);
    return false;
}


bool Player::removeBlock(Terrain *terrain) {
    glm::vec3 direction = this->m_camera.getForward() * 3.f;
    glm::ivec3 outBlock = glm::ivec3();
    float outDist = 0.f;
    if (gridMarch((glm::vec3(0.f, 1.5f, 0.f) + this->m_position),
                  direction, *terrain, &outDist, &outBlock)) {

        if (terrain->getBlockAt(outBlock.x, outBlock.y, outBlock.z) != BEDROCK) {

            terrain->setBlockAt(outBlock.x, outBlock.y, outBlock.z, EMPTY);

            Chunk* thisChunk = terrain->getChunkAt(outBlock.x, outBlock.z).get();
           // thisChunk.
            thisChunk->destroyVBOdata();
            thisChunk->createVBOdata();
            terrain->updateChunkNeighbors(outBlock.x, outBlock.z);

            return true;
        } else {
            std::cout << "haha gl trying to break bedrock ya loser" << std::endl;
        }
    }
    return false;
}

bool Player::addBlock(Terrain *terrain) {
    glm::vec3 direction = this->m_camera.getForward() * 3.f;
    glm::ivec3 outBlock = glm::ivec3();
    if (gridMarchBlockBefore((glm::vec3(0.f, 1.5f, 0.f) + this->m_position),
                             direction, *terrain, &outBlock)) {
        terrain->setBlockAt(outBlock.x, outBlock.y, outBlock.z, GRASS);

        Chunk* thisChunk = terrain->getChunkAt(outBlock.x, outBlock.z).get();
        thisChunk->destroyVBOdata();
        thisChunk->createVBOdata();

        return true;
    }
    return false;
}

void Player::toggleGrounded() {
    this->isGrounded = false;
}
Camera Player::getCamera() {
    return this->m_camera;
}

void Player::setCameraWiDThHeight(unsigned int w, unsigned int h) {
    m_camera.setWiDThHeight(w, h);
}

void Player::moveAlongVector(glm::vec3 dir) {
    Entity::moveAlongVector(dir);
    m_camera.matchCamToPlayer(this->m_position);
}
void Player::moveForwardLocal(float amount) {
    Entity::moveForwardLocal(amount);
    m_camera.moveForwardLocal(amount);
}
void Player::moveRightLocal(float amount) {
    Entity::moveRightLocal(amount);
    m_camera.moveRightLocal(amount);
}
void Player::moveUpLocal(float amount) {
    Entity::moveUpLocal(amount);
    m_camera.moveUpLocal(amount);
}
void Player::moveForwardGlobal(float amount) {
    Entity::moveForwardGlobal(amount);
}
void Player::moveRightGlobal(float amount) {
    Entity::moveRightGlobal(amount);
}
void Player::moveUpGlobal(float amount) {
    Entity::moveUpGlobal(amount);
}
void Player::rotateOnForwardLocal(float degrees) {
    Entity::rotateOnForwardLocal(degrees);
    m_camera.rotateOnForwardLocal(degrees);
}
void Player::rotateOnRightLocal(float degrees) {
    Entity::rotateOnRightLocal(degrees);
    m_camera.rotateOnRightLocal(degrees);
}
void Player::rotateOnUpLocal(float degrees) {
    Entity::rotateOnUpLocal(degrees);
    m_camera.rotateOnUpLocal(degrees);
}
void Player::rotateOnForwardGlobal(float degrees) {
    Entity::rotateOnForwardGlobal(degrees);
    m_camera.rotateOnForwardGlobal(degrees);
}
void Player::rotateOnRightGlobal(float degrees) {
    Entity::rotateOnRightGlobal(degrees);
    m_camera.rotateOnRightGlobal(degrees);
}
void Player::rotateOnUpGlobal(float degrees) {
    Entity::rotateOnUpGlobal(degrees);
    m_camera.rotateOnUpGlobal(degrees);
}

float Player::getNearestBlockDist(Terrain *terrain) {
    glm::vec3 direction = this->m_camera.getForward() * 100.f;
    glm::ivec3 outBlock = glm::ivec3();
    float out_Dist = 0.f;
    gridMarch((glm::vec3(0.f, 1.5f, 0.f) + this->m_position),
                  direction, *terrain, &out_Dist, &outBlock);

//    float out_Dist = 0.f;

//    glm::vec3 rayOrigin = glm::vec3(0.f, 1.5f, 0.f) + this->m_position;
//    glm::vec3 rayDirection = direction;
//    glm::ivec3 *out_blockHit;
//    float maxLen = glm::length(rayDirection); // Farthest we search
//    glm::ivec3 currCell = glm::ivec3(glm::floor(rayOrigin));
//    rayDirection = glm::normalize(rayDirection); // Now all t values represent world dist.

//    float curr_t = 0.f;
//    while(curr_t < maxLen) {
//        float min_t = glm::sqrt(3.f);
//        float interfaceAxis = -1; // Track axis for which t is smallest
//        for(int i = 0; i < 3; ++i) { // Iterate over the three axes
//            if(rayDirection[i] != 0) { // Is ray parallel to axis i?
//                float offset = glm::max(0.f, glm::sign(rayDirection[i])); // See slide 5
//                // If the player is *exactly* on an interface then
//                // they'll never move if they're looking in a negative direction
//                if(currCell[i] == rayOrigin[i] && offset == 0.f) {
//                    offset = -1.f;
//                }
//                int nextIntercept = currCell[i] + offset;
//                float axis_t = (nextIntercept - rayOrigin[i]) / rayDirection[i];
//                axis_t = glm::min(axis_t, maxLen); // Clamp to max len to avoid super out of bounds errors
//                if(axis_t < min_t) {
//                    min_t = axis_t;
//                    interfaceAxis = i;
//                }
//            }
//        }
//        if(interfaceAxis == -1) {
//            return 100.f;
//        }
//        curr_t += min_t; // min_t is declared in slide 7 algorithm
//        rayOrigin += rayDirection * min_t;
//        glm::ivec3 offset = glm::ivec3(0,0,0);
//        // Sets it to 0 if sign is +, -1 if sign is -
//        offset[interfaceAxis] = glm::min(0.f, glm::sign(rayDirection[interfaceAxis]));
//        // nextcell
//        currCell = glm::ivec3(glm::floor(rayOrigin)) + offset;
//        // If currCell contains something other than EMPTY, return
//        // curr_t
//        BlockType cellType = terrain -> getBlockAt(currCell.x, currCell.y, currCell.z);
//        // don't bump into transparent blocks
//        if(cellType != EMPTY && cellType != LAVA && cellType != WATER && cellType != S_GRASS && cellType != LEAVES && cellType != SEA_GRASS
//                && cellType != CAVE_PLANT && cellType != CAVE_VINE && cellType != CAVE_PLANT_SMOL
//                && cellType != RED_PLANT && cellType != GREEN_PLANT && cellType != YELLOW_PLANT) {
//            *out_blockHit = currCell; // return this
//            out_Dist = glm::min(maxLen, curr_t);
//            return true;
//        }
//        // update currcel
//    }
//    out_Dist = glm::min(maxLen, curr_t);

    return out_Dist;
}

glm::vec4 Player::getCameraPos (){
    return glm::vec4(m_position + glm::vec3(0, 1.5f, 0), 1.f);
}

glm::vec3 Player::getPosition() {
    return this->m_position;
}
QString Player::posAsQString() const {
    std::string str("( " + std::to_string(m_position.x) + ", " + std::to_string(m_position.y) + ", " + std::to_string(m_position.z) + ")");
    return QString::fromStdString(str);
}
QString Player::velAsQString() const {
    std::string str("( " + std::to_string(m_velocity.x) + ", " + std::to_string(m_velocity.y) + ", " + std::to_string(m_velocity.z) + ")");
    return QString::fromStdString(str);
}
QString Player::accAsQString() const {
    std::string str("( " + std::to_string(m_acceleration.x) + ", " + std::to_string(m_acceleration.y) + ", " + std::to_string(m_acceleration.z) + ")");
    return QString::fromStdString(str);
}
QString Player::lookAsQString() const {
    std::string str("( " + std::to_string(m_forward.x) + ", " + std::to_string(m_forward.y) + ", " + std::to_string(m_forward.z) + ")");
    return QString::fromStdString(str);
}

