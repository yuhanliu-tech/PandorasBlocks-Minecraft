#include "chunk.h"
#include <iostream>
#include <error.h>


Chunk::Chunk(OpenGLContext* context, int x, int z) : Drawable(context), hasVBO(false), hasBlockData(false), isBuffered(false), xCoord(x), zCoord(z), m_blocks(),context(context), m_neighbors{{XPOS, nullptr}, {XNEG, nullptr}, {ZPOS, nullptr}, {ZNEG, nullptr}}
{
    std::fill_n(m_blocks.begin(), 65536, EMPTY);
}

int Chunk::getX() {
    return xCoord;
}
int Chunk::getZ() {
    return zCoord;
}

// Does bounds checking with at()
BlockType Chunk::getBlockAt(unsigned int x, unsigned int y, unsigned int z) const {
    return m_blocks.at(x + 16 * y + 16 * 256 * z);
}

// Exists to get rid of compiler warnings about int -> unsigned int implicit conversion
BlockType Chunk::getBlockAt(int x, int y, int z) const {
    return getBlockAt(static_cast<unsigned int>(x), static_cast<unsigned int>(y), static_cast<unsigned int>(z));
}

// Does bounds checking with at()
void Chunk::setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t) {
    m_blocks.at(x + 16 * y + 16 * 256 * z) = t;
}

// helper function to check if neighbor is empty and if so, update VBOs
void Chunk::updateFaceVBOs(BlockType neighbor, glm::vec3 pos, glm::vec4 nor, int* opaqueFaces,  int* transpFaces, BlockType curr, Direction dir, chunkVBO &chunkvbo) {

    float highestBlockHt = 0;
    if(curr == WATER && nor == glm::vec4(0, 1, 0, 0)) {
        highestBlockHt = calcTerrainFinalHeight(glm::vec2(pos[0] + getX(), pos[2] + getZ()));
    }

    if (neighbor == EMPTY || blockTypeCubeType.at(neighbor) != 0) { //if our neighbor is empty or transparent
        if(blockFaces.at(curr).at(dir)[2] == 1.0f) { //If our current block is opaque
            (*opaqueFaces)++;
            for (int i = 0; i < 4; i++) {
                chunkvbo.m_vboDataOpaque.push_back(findPos(dir, i) + glm::vec4(pos,0.f) + glm::vec4(xCoord, 0, zCoord, 0)); // pos
                chunkvbo.m_vboDataOpaque.push_back(glm::vec4(nor)); // nor
                chunkvbo.m_vboDataOpaque.push_back(indexToPos(blockFaces.at(curr).at(dir), i)); // col
                chunkvbo.m_vboDataOpaque.push_back(blockTypeSFX.at(curr));
                chunkvbo.m_vboDataOpaque.push_back(glm::vec4(highestBlockHt, 0, 0, 0)); //to change later

            }
        } else if (neighbor == EMPTY) { // Transparent Block population
            (*transpFaces)++;
            for (int i = 0; i < 4; i++) {
                chunkvbo.m_vboDataTransparent.push_back(findPos(dir, i) + glm::vec4(pos,0.f) + glm::vec4(xCoord, 0, zCoord, 0)); // pos
                chunkvbo.m_vboDataTransparent.push_back(glm::vec4(nor)); // nor
                chunkvbo.m_vboDataTransparent.push_back(indexToPos(blockFaces.at(curr).at(dir), i)); // col
                chunkvbo.m_vboDataTransparent.push_back(blockTypeSFX.at(curr));
                chunkvbo.m_vboDataTransparent.push_back(glm::vec4(highestBlockHt, 0, 0, 0)); //to change later
            }
        }

        //context->printGLErrorLog();
    } else if(blockTypeCubeType.at(curr) == 3) { //If our current block is opaque
        (*opaqueFaces)++;
        for (int i = 0; i < 4; i++) {
            chunkvbo.m_vboDataOpaque.push_back(findPos(dir, i) + glm::vec4(pos,0.f) + glm::vec4(xCoord, 0, zCoord, 0)); // pos
            chunkvbo.m_vboDataOpaque.push_back(glm::vec4(nor)); // nor
            chunkvbo.m_vboDataOpaque.push_back(indexToPos(blockFaces.at(curr).at(dir), i)); // col
            chunkvbo.m_vboDataOpaque.push_back(blockTypeSFX.at(curr));
            chunkvbo.m_vboDataOpaque.push_back(glm::vec4(highestBlockHt, 0, 0, 0)); //to change later

        }
    } else if(blockTypeCubeType.at(curr) == 2 && blockTypeSFX.at(curr)[0] == 2.f) { //Standee wave
        (*opaqueFaces)++;
        for (int i = 0; i < 4; i++) {
            chunkvbo.m_vboDataOpaque.push_back(findPos(dir, i) + glm::vec4(pos,0.f) + glm::vec4(xCoord, 0, zCoord, 0)); // pos
            chunkvbo.m_vboDataOpaque.push_back(glm::vec4(nor)); // nor
            chunkvbo.m_vboDataOpaque.push_back(indexToPos(blockFaces.at(curr).at(dir), i)); // col

            if(findPos(dir, i)[1] == 1.f) {//top 2 of the face
                chunkvbo.m_vboDataOpaque.push_back(blockTypeSFX.at(curr));
            } else {
                glm::vec4 sfxStaticMod = blockTypeSFX.at(curr);
                sfxStaticMod[0] = 0;
                chunkvbo.m_vboDataOpaque.push_back(sfxStaticMod);
            }

            chunkvbo.m_vboDataOpaque.push_back(glm::vec4(highestBlockHt, 0, 0, 0)); //to change later
        }
    } else if(blockTypeCubeType.at(curr) == 2 && (blockTypeSFX.at(curr)[0] >= 3.f)) { //Standee waves, was originalyl vines
        (*opaqueFaces)++;
        for (int i = 0; i < 4; i++) {
            chunkvbo.m_vboDataOpaque.push_back(findPos(dir, i) + glm::vec4(pos,0.f) + glm::vec4(xCoord, 0, zCoord, 0)); // pos
            chunkvbo.m_vboDataOpaque.push_back(glm::vec4(nor)); // nor
            chunkvbo.m_vboDataOpaque.push_back(indexToPos(blockFaces.at(curr).at(dir), i)); // col

            if(blockTypeSFX.at(curr)[0] == 3.f) {
                if(findPos(dir, i)[1] == 1.f) {//bottom 2 of the face
                    glm::vec4 sfxStaticMod = blockTypeSFX.at(curr);
                    sfxStaticMod[0] = 0;
                    chunkvbo.m_vboDataOpaque.push_back(sfxStaticMod);
                } else {
                    chunkvbo.m_vboDataOpaque.push_back(blockTypeSFX.at(curr));
                }
            } else {
                chunkvbo.m_vboDataOpaque.push_back(blockTypeSFX.at(curr));
            }

            chunkvbo.m_vboDataOpaque.push_back(glm::vec4(highestBlockHt, 0, 0, 0)); //to change later

        }
    }

}

// create function: stores all per-vertex data in interleaved format in single VBO
void Chunk::createVBOdata() {
    chunkVBO chunkvbo;

    // sets hasvbo true in create
    createInterleavedData(chunkvbo);
    // chunkvbo.chunk->hasVBO = true;

    bufferInterleavedData(chunkvbo);
}

void Chunk::destroyVBOdata() {
    Drawable::destroyVBOdata();
    isBuffered = false;
    hasVBO = false;
}

std::vector<Chunk*> Chunk::getNeighbors() {
    std::vector<Chunk*> out;
    for(auto &c: m_neighbors) {
        if (c.second) {
            out.push_back(c.second);
        }
    }
    return out;
}

void Chunk::createInterleavedData(chunkVBO &c) {
    c.chunk = this;

    int opaqueFaces = 0;
    int opaqueVerts = 0;

    int transpFaces = 0;
    int transpVerts = 0;

    for (int x = 0; x < 16; ++x) {
        for (int y = 0; y < 256; ++y) {
            for (int z = 0; z < 16; ++z) {
                BlockType curr = getBlockAt(x,y,z);
                glm::vec3 currPos = glm::vec3(x,y,z);
                if (curr != EMPTY) {
                    if(blockTypeCubeType.at(curr) == 2) {
                        createStandeeData(x, y, z, &curr, currPos, &opaqueFaces, &transpFaces, &c);

                    } else  {
                        createCubeData(x, y, z, &curr, currPos, &opaqueFaces, &transpFaces, &c);
                    }

                }
            }
        }
    }

    for (int i = 0; i < opaqueFaces; i++) {
        c.m_idxOpaque.push_back(opaqueVerts);
        c.m_idxOpaque.push_back(opaqueVerts + 2);
        c.m_idxOpaque.push_back(opaqueVerts + 1);
        c.m_idxOpaque.push_back(opaqueVerts);
        c.m_idxOpaque.push_back(opaqueVerts + 3);
        c.m_idxOpaque.push_back(opaqueVerts + 2);
        opaqueVerts += 4;
    }

    for(int i = 0; i < transpFaces; i++) {
        c.m_idxTransparent.push_back(transpVerts);
        c.m_idxTransparent.push_back(transpVerts + 2);
        c.m_idxTransparent.push_back(transpVerts + 1);
        c.m_idxTransparent.push_back(transpVerts);
        c.m_idxTransparent.push_back(transpVerts + 3);
        c.m_idxTransparent.push_back(transpVerts + 2);
        transpVerts += 4;
    }
    if (c.m_vboDataOpaque.size() > 0 || c.m_vboDataTransparent.size() > 0) {
        hasVBO = true;

    }
}

void Chunk::createCubeData(int x, int y, int z, BlockType* curr, glm::vec3& currPos, int* opaqueFaces, int* transpFaces, chunkVBO* chunkvbo) {
    BlockType xPosFaceBlock = GRASS;
    if (x == 15) {
        if (m_neighbors.at(XPOS) != nullptr) {
            xPosFaceBlock = m_neighbors[XPOS]->getBlockAt(0, y, z);
        }
    } else {
        xPosFaceBlock = getBlockAt(x + 1, y, z);
    }
    updateFaceVBOs(xPosFaceBlock, currPos, glm::vec4(1,0,0,0), opaqueFaces, transpFaces, *curr, XPOS, *chunkvbo); //UPDATES FOR ONE FACE

    // x-negative
    BlockType xNegFaceBlock = GRASS;
    if (x == 0) {
        if (m_neighbors.at(XNEG) != nullptr) {
            xNegFaceBlock = m_neighbors[XNEG]->getBlockAt(15, y, z);
        }
    } else {
        xNegFaceBlock = getBlockAt(x - 1, y, z);
    }
    updateFaceVBOs(xNegFaceBlock, currPos, glm::vec4(-1,0,0,0), opaqueFaces,  transpFaces, *curr, XNEG, *chunkvbo);

    // y-positive
    if (y == 255) {
        updateFaceVBOs(EMPTY, currPos, glm::vec4(0,1,0,0), opaqueFaces,  transpFaces, *curr, YPOS, *chunkvbo);
    } else {
        updateFaceVBOs(getBlockAt(x, y + 1, z), currPos, glm::vec4(0,1,0,0), opaqueFaces,  transpFaces, *curr, YPOS, *chunkvbo);
    }

    // y-negative
    if (y == 0) {
        updateFaceVBOs(EMPTY, currPos, glm::vec4(0,-1,0,0), opaqueFaces, transpFaces,  *curr, YNEG, *chunkvbo);
    } else {
        updateFaceVBOs(getBlockAt(x, y - 1, z), currPos, glm::vec4(0,-1,0,0), opaqueFaces,  transpFaces, *curr, YNEG, *chunkvbo);
    }

    // z-positive
    BlockType zPosFaceBlock = GRASS;
    if (z == 15) {
        if (m_neighbors.at(ZPOS) != nullptr) {
            zPosFaceBlock = m_neighbors[ZPOS]->getBlockAt(x, y, 0);
        }
    } else {
        zPosFaceBlock = getBlockAt(x, y, z + 1);
    }
    updateFaceVBOs(zPosFaceBlock, currPos, glm::vec4(0,0,1,0), opaqueFaces, transpFaces,  *curr, ZPOS, *chunkvbo);

    // z-negative
    BlockType zNegFaceBlock = GRASS;
    if (z == 0) {
        if (m_neighbors.at(ZNEG) != nullptr) {
            zNegFaceBlock = m_neighbors[ZNEG]->getBlockAt(x, y, 15);
        }
    } else {
        zNegFaceBlock = getBlockAt(x, y, z - 1);
    }
    updateFaceVBOs(zNegFaceBlock, currPos, glm::vec4(0,0,-1,0), opaqueFaces,  transpFaces, *curr, ZNEG, *chunkvbo);
}

void Chunk::createStandeeData(int x, int y, int z, BlockType* curr, glm::vec3& currPos, int* opaqueFaces, int* transpFaces, chunkVBO* chunkvbo) {
    //RIGHT DIAGONAL
    BlockType rightPosFaceBlock = GRASS; //Front
    updateFaceVBOs(rightPosFaceBlock, currPos, glm::vec4(1,0,-1,0), opaqueFaces, transpFaces, *curr, R_POS_DIAG, *chunkvbo); //UPDATES FOR ONE FACE

    BlockType rightNegFaceBlock = GRASS;
    updateFaceVBOs(rightNegFaceBlock, currPos, glm::vec4(-1,0,1,0), opaqueFaces,  transpFaces, *curr, R_NEG_DIAG, *chunkvbo);

    //LEFT DIAGONAL
    BlockType leftPosFaceBlock = GRASS; //Front
    updateFaceVBOs(leftPosFaceBlock, currPos, -glm::vec4(-1,0,-1,0), opaqueFaces, transpFaces, *curr, L_POS_DIAG, *chunkvbo); //UPDATES FOR ONE FACE

    BlockType leftNegFaceBlock = GRASS;
    updateFaceVBOs(leftNegFaceBlock, currPos, -glm::vec4(1,0,1,0), opaqueFaces,  transpFaces, *curr, L_NEG_DIAG, *chunkvbo);

}

// function that takes in vector of interleaved vertex data and vector of index ddata and buffers them into appropriate VBOs
void Chunk::bufferInterleavedData(chunkVBO &chunkvbo) {
    if (isBuffered) {
        throw std::runtime_error("Chunk already has been buffered, 242");
    }
    if (!hasVBO) {
        //qDebug() << "count: " << chunkvbo.m_idxOpaque.size();
        // throw std::runtime_error("Chunk did not have VBO");
    }
    // only create VBO data for block faces that lie on boundary between empty block and filled block

    std::vector<glm::vec4> interleavedOpaque = chunkvbo.m_vboDataOpaque;

    std::vector<GLuint> idxOpaque = chunkvbo.m_idxOpaque;

    m_countOpaque = idxOpaque.size();

    generateInterleavedOpaque(); //Split into opaque and traansp
    bindInterleavedOpaque();
    generateOpaqueSFX();
    bindOpaqueSFX();
    mp_context->glBufferData(GL_ARRAY_BUFFER, interleavedOpaque.size() * sizeof(glm::vec4), interleavedOpaque.data(), GL_STATIC_DRAW);

    generateOpaqueIdx();
    bindOpaqueIdx();
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxOpaque.size() * sizeof(GLuint), idxOpaque.data(), GL_STATIC_DRAW);


    std::vector<glm::vec4> interleaveDTransparent = chunkvbo.m_vboDataTransparent;
    std::vector<GLuint> idxTransparent = chunkvbo.m_idxTransparent;

    m_countTransparent = idxTransparent.size();

    generateInterleaveDTransparent(); //Split into opaque and traansp
    bindInterleaveDTransparent();
    generateTransparentSFX();
    binDTransparentSFX();
    mp_context->glBufferData(GL_ARRAY_BUFFER, interleaveDTransparent.size() * sizeof(glm::vec4), interleaveDTransparent.data(), GL_STATIC_DRAW);

    generateTransparentIdx();
    binDTransparentIdx();
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxTransparent.size() * sizeof(GLuint), idxTransparent.data(), GL_STATIC_DRAW);

    isBuffered = true;
}

GLenum Chunk::drawMode() {
    return GL_TRIANGLES;
}

const static std::unordered_map<Direction, Direction, EnumHash> oppositeDirection {
    {XPOS, XNEG},
    {XNEG, XPOS},
    {YPOS, YNEG},
    {YNEG, YPOS},
    {ZPOS, ZNEG},
    {ZNEG, ZPOS}
};

void Chunk::linkNeighbor(uPtr<Chunk> &neighbor, Direction dir) {
    if(neighbor != nullptr) {
        this->m_neighbors[dir] = neighbor.get();
        neighbor->m_neighbors[oppositeDirection.at(dir)] = this;
    }
}

glm::vec4 Chunk::findPos(Direction dir, int i) {

    glm::vec4 offset = glm::vec4(0);
    float zFightHelper = 0.05; //For z fighting with grassy grass : )

    if (dir == XPOS) {

        switch(i) {
        case 0:
            offset = glm::vec4(1.f,1.f,0.f,1.f);
            break;
        case 1:
            offset = glm::vec4(1.f,0.f,0.f,1.f);
            break;
        case 2:
            offset = glm::vec4(1.f,0.f,1.f,1.f);
            break;
        case 3:
            offset = glm::vec4(1.f,1.f,1.f,1.f);
            break;
        };

    } else if (dir == XNEG) {

        switch(i) {
        case 0:
            offset = glm::vec4(0.f,1.f,1.f,1.f);
            break;
        case 1:
            offset = glm::vec4(0.f,0.f,1.f,1.f);
            break;
        case 2:
            offset = glm::vec4(0.f,0.f,0.f,1.f);
            break;
        case 3:
            offset = glm::vec4(0.f,1.f,0.f,1.f);
            break;
        };

    } else if (dir == YPOS) {

        switch(i) {
        case 0:
            offset = glm::vec4(1.f,1.f,0.f,1.f);
            break;
        case 1:
            offset = glm::vec4(1.f,1.f,1.f,1.f);
            break;
        case 2:
            offset = glm::vec4(0.f,1.f,1.f,1.f);
            break;
        case 3:
            offset = glm::vec4(0.f,1.f,0.f,1.f);
            break;
        };

    } else if (dir == YNEG) {

        switch(i) {
        case 0:
            offset = glm::vec4(1.f,0.f,1.f,1.f);
            break;
        case 1:
            offset = glm::vec4(1.f,0.f,0.f,1.f);
            break;
        case 2:
            offset = glm::vec4(0.f,0.f,0.f,1.f);
            break;
        case 3:
            offset = glm::vec4(0.f,0.f,1.f,1.f);
            break;
        };

    } else if (dir == ZPOS) {

        switch(i) {
        case 0:
            offset = glm::vec4(1.f,1.f,1.f,1.f);
            break;
        case 1:
            offset = glm::vec4(1.f,0.f,1.f,1.f);
            break;
        case 2:
            offset = glm::vec4(0.f,0.f,1.f,1.f);
            break;
        case 3:
            offset = glm::vec4(0.f,1.f,1.f,1.f);
            break;
        };

    } else if (dir == ZNEG) { // ZNEG

        switch(i) {
        case 0:
            offset = glm::vec4(0.f,1.f,0.f,1.f);
            break;
        case 1:
            offset = glm::vec4(0.f,0.f,0.f,1.f);
            break;
        case 2:
            offset = glm::vec4(1.f,0.f,0.f,1.f);
            break;
        case 3:
            offset = glm::vec4(1.f,1.f,0.f,1.f);
            break;
        };

    } else if(dir == R_POS_DIAG) { //TODO
        switch(i) {
        case 1:
            offset = glm::vec4(0.f + zFightHelper,0,0,1);
            break;
        case 2:
            offset = glm::vec4(1.f + zFightHelper,0,1,1);
            break;
        case 3:
            offset = glm::vec4(1.f+ zFightHelper,1,1,1);
            break;
        case 0:
            offset = glm::vec4(0.f+ zFightHelper,1,0,1);
            break;
        };
    }  else if(dir == R_NEG_DIAG) { //TODO
        switch(i) {
        case 2:
            offset = glm::vec4(0.f,0,0,1);
            break;
        case 3:
            offset = glm::vec4(0.f,1,0,1);
            break;
        case 0:
            offset = glm::vec4(1.f,1,1,1);
            break;
        case 1:
            offset = glm::vec4(1.f,0,1,1);
            break;
        };
    } else if(dir == L_POS_DIAG) { //TODO
        switch(i) {
        case 1:
            offset = glm::vec4(0.f + zFightHelper,0,1,1);
            break;
        case 2:
            offset = glm::vec4(1.f+ zFightHelper,0,0,1);
            break;
        case 3:
            offset = glm::vec4(1.f+ zFightHelper,1,0,1);
            break;
        case 0:
            offset = glm::vec4(0.f+ zFightHelper,1,1,1);
            break;
        };
    }  else if(dir == L_NEG_DIAG) { //TODO
        switch(i) {
        case 2:
            offset = glm::vec4(0.f,0,1,1);
            break;
        case 3:
            offset = glm::vec4(0.f,1,1,1);
            break;
        case 0:
            offset = glm::vec4(1.f,1,0,1);
            break;
        case 1:
            offset = glm::vec4(1.f,0,0,1);
            break;
        };
    }

    return offset;

}

//temporary function to find default color for block types
glm::vec3 Chunk::findColor(BlockType t) {
    if (t == STONE) {
        return (glm::vec3(0.5f));
    } else if (t == DIRT) {
        return (glm::vec3(121.f, 85.f, 58.f) / 255.f);
    } else if (t == GRASS) {
        return (glm::vec3(95.f, 159.f, 53.f) / 255.f);
    } else if (t == WATER) {
        return (glm::vec3(0.f, 0.f, 0.75f));
    } else if (t == SAND) {
        return (glm::vec3(1.f, 1.f, 0.f));
    }else if (t == SNOW) {
        return (glm::vec3(1.f, 1.f, 1.f));
    } else if (t == LAVA) {
        return (glm::vec3(1.f, 0.f, 0.f));
    } else if (t == BEDROCK) {
        return (glm::vec3(1.f, 1.f, 1.f));
    } else if (t == GR_STONE) {
        return (glm::vec3(1.f, 1.f, 1.f));
    } else if (t == BLUE_STONE) {
        return (glm::vec3(1.f, 1.f, 1.f));
    } else if (t == TEAL_STONE) {
        return (glm::vec3(1.f, 1.f, 1.f));
    } else {
        // Other block types are not yet handled, so we default to debug purple
        return (glm::vec3(1.f, 0.f, 1.f));
    }
}

bool Chunk::drawTree(int x, int y, int z) { // (x,y,z) is where you're planting tree
    if ((x%16 <= 3) ||
            (x%16 >= 12)||
            (z%16 <= 3) ||
            (z%16 >= 12))
        return false;
    else if (getBlockAt(x,y,z) == GRASS) { // oak tree
        //trunk
        setBlockAt(x,y+1,z,WOOD);
        setBlockAt(x,y+2,z,WOOD);
        setBlockAt(x,y+3,z,WOOD);
        setBlockAt(x,y+4,z,WOOD);
        setBlockAt(x,y+5,z,WOOD);
        setBlockAt(x,y+6,z,WOOD);

        //leaves
        setBlockAt(x,y+7,z,LEAVES);
        setBlockAt(x+1,y+7,z,LEAVES);
        setBlockAt(x-1,y+7,z,LEAVES);
        setBlockAt(x,y+7,z-1,LEAVES);
        setBlockAt(x,y+7,z+1,LEAVES);

        setBlockAt(x,y+6,z,LEAVES);
        setBlockAt(x+1,y+6,z,LEAVES);
        setBlockAt(x-1,y+6,z,LEAVES);
        setBlockAt(x,y+6,z-1,LEAVES);
        setBlockAt(x,y+6,z+1,LEAVES);
        setBlockAt(x+1,y+6,z+1,LEAVES);
        setBlockAt(x-1,y+6,z+1,LEAVES);
        setBlockAt(x+1,y+6,z-1,LEAVES);
        setBlockAt(x-1,y+6,z-1,LEAVES);

        setBlockAt(x+1,y+5,z,LEAVES);
        setBlockAt(x-1,y+5,z,LEAVES);
        setBlockAt(x,y+5,z-1,LEAVES);
        setBlockAt(x,y+5,z+1,LEAVES);
        setBlockAt(x+1,y+5,z+1,LEAVES);
        setBlockAt(x-1,y+5,z+1,LEAVES);
        setBlockAt(x+1,y+5,z-1,LEAVES);
        setBlockAt(x-1,y+5,z-1,LEAVES);

        setBlockAt(x,y+4,z,LEAVES);
        setBlockAt(x+1,y+4,z,LEAVES);
        setBlockAt(x-1,y+4,z,LEAVES);
        setBlockAt(x,y+4,z-1,LEAVES);
        setBlockAt(x,y+4,z+1,LEAVES);
        setBlockAt(x+1,y+4,z+1,LEAVES);
        setBlockAt(x-1,y+4,z+1,LEAVES);
        setBlockAt(x+1,y+4,z-1,LEAVES);
        setBlockAt(x-1,y+4,z-1,LEAVES);

        setBlockAt(x+2,y+4,z,LEAVES);
        setBlockAt(x+2,y+4,z-1,LEAVES);
        setBlockAt(x+2,y+4,z+1,LEAVES);

        setBlockAt(x-2,y+4,z,LEAVES);
        setBlockAt(x-2,y+4,z-1,LEAVES);
        setBlockAt(x-2,y+4,z+1,LEAVES);

        setBlockAt(x,y+4,z+2,LEAVES);
        setBlockAt(x-1,y+4,z+2,LEAVES);
        setBlockAt(x+1,y+4,z+2,LEAVES);

        setBlockAt(x,y+4,z-2,LEAVES);
        setBlockAt(x-1,y+4,z-2,LEAVES);
        setBlockAt(x+1,y+4,z-2,LEAVES);

        setBlockAt(x+2,y+5,z,LEAVES);
        setBlockAt(x+2,y+5,z-1,LEAVES);
        setBlockAt(x+2,y+5,z+1,LEAVES);

        setBlockAt(x-2,y+5,z,LEAVES);
        setBlockAt(x-2,y+5,z-1,LEAVES);
        setBlockAt(x-2,y+5,z+1,LEAVES);

        setBlockAt(x,y+5,z+2,LEAVES);
        setBlockAt(x-1,y+5,z+2,LEAVES);
        setBlockAt(x+1,y+5,z+2,LEAVES);

        setBlockAt(x,y+5,z-2,LEAVES);
        setBlockAt(x-1,y+5,z-2,LEAVES);
        setBlockAt(x+1,y+5,z-2,LEAVES);

    } else if (getBlockAt(x,y,z) == SAND && getBlockAt(x,y+1,z) != WATER && (rand() % 5 == 4)) { // palm tree

        //trunk
        setBlockAt(x,y+1,z,PALM_WOOD);
        setBlockAt(x,y+2,z,PALM_WOOD);
        setBlockAt(x,y+3,z,PALM_WOOD);

        setBlockAt(x+1,y+3,z,PALM_WOOD);
        setBlockAt(x+1,y+4,z,PALM_WOOD);
        setBlockAt(x+1,y+5,z,PALM_WOOD);
        setBlockAt(x+1,y+6,z,PALM_WOOD);
        setBlockAt(x+1,y+7,z,PALM_WOOD);

        setBlockAt(x+1, y+8, z,LEAVES);
        setBlockAt(x+2, y+8, z,LEAVES);
        setBlockAt(x, y+8, z,LEAVES);
        setBlockAt(x+1, y+8, z+1,LEAVES);
        setBlockAt(x+1, y+8, z-1,LEAVES);

        setBlockAt(x+3, y+7, z,LEAVES);
        setBlockAt(x-1, y+7, z,LEAVES);
        setBlockAt(x+1, y+7, z+2,LEAVES);
        setBlockAt(x+1, y+7, z-2,LEAVES);

        setBlockAt(x+4, y+6, z,LEAVES);
        setBlockAt(x-2, y+6, z,LEAVES);
        setBlockAt(x+1, y+6, z+3,LEAVES);
        setBlockAt(x+1, y+6, z-3,LEAVES);


    }

    return true;
}


float Chunk::calcTerrainFinalHeight(glm::vec2 blockPos){
    float dryness = terrainNoise::calcDryness(blockPos);
    float height = terrainNoise::calcHeight(blockPos);

    // biome mixing
    float moistness = terrainNoise::calcMoistness(blockPos / 500.f);
    float temp = terrainNoise::calcTemp(blockPos / 500.f);

    //Dry Terrains
    float forestHeight = terrainNoise::calcYOffsetHills(blockPos);
    float snowMtnHeight = terrainNoise::calcYOffsetMountains(blockPos);

    //Wet Terrains
    float islandsHeight = terrainNoise::calcYOffsetIslands(blockPos);
    float pillarsHeight = terrainNoise::tianziNoise(blockPos);

    // Water mountain biome - mix grass and sand blocks
    // have peaks that level off
    // have large bodies of water
    float waterMountHeight = terrainNoise::calcWaterMountains(blockPos);
    float spikyMountHeight = terrainNoise::calcSpikyBiome(blockPos);

    float dryTerrains = glm::mix(forestHeight, snowMtnHeight, height);
    float wetTerrains = glm::mix(islandsHeight, pillarsHeight, (1-height));

    float finalHt; //forestHeight; //waterMountHeight;
    //  float finalHt = glm::mix(wetTerrains, dryTerrains, dryness);

    std::map<int, float> iDToHeight;
    iDToHeight.insert(std::pair(0, snowMtnHeight));
    iDToHeight.insert(std::pair(1, forestHeight));
    iDToHeight.insert(std::pair(2, waterMountHeight));
    iDToHeight.insert(std::pair(3, spikyMountHeight));
    iDToHeight.insert(std::pair(4, pillarsHeight));
    iDToHeight.insert(std::pair(5, islandsHeight));

    return getBiomeSectionHeight(moistness, temp, &iDToHeight);

    //terrainNoise::floatingIslandNoise(glm::vec3(blockPos.x, 0, blockPos.y));//pillarsHeight;//getBiomeSectionHeight(moistness, temp, &iDToHeight);
}

// within green line - 100 % of its height value for given temp and moisture
// take noise values - raise to exponent will raise peaks
// map big range to like 0.01 and then when it gets to 0.75, we have abrupt peak

// biome blending:
// make a temp noise function. redo dryness? Use temp and dryness to determine height

void Chunk::setChunkTerrain(int chunkX, int chunkZ){

    int globalChunkX = chunkX;// * 16;
    int globalChunkZ = chunkZ;// * 16;

    for(int x = 0; x < 16; x++) {
        for(int z = 0; z < 16; z++) {
            glm::vec2 blockPos(globalChunkX + x, globalChunkZ + z);

            float finalHt = calcTerrainFinalHeight(blockPos);
            // biome mixing
            float moistness = terrainNoise::calcMoistness(blockPos / 500.f);
            float temp = terrainNoise::calcTemp(blockPos / 500.f);


            int id = 4;//getBiomeId(moistness, temp); //2; //getBiomeId(moistness, temp);  // 4


            // function to map id, moistness and temp level
            // to a final height. input is map of
            // biome ids to their heights^, id, moist, temp
            // return is a float of height that may be a mix
            // or is purely height of id.

            //  bool isDry = true; //(dryness > 0.99);
            // bool isHigh = false; //(height > 0.25);

            setBlockAt(x,0,z,BEDROCK); // set y=0 blocks to bedrock

            int y = 1;
            if (getBiomeId(moistness, temp) != 3) { //truncating the cave biomes for the ones that we dont want it (
                y = 128;
            }

            for(y; y < finalHt; y++) {
                if(y >= 0 && y <= 128) {

                    if (terrainNoise::isCave(glm::vec3(blockPos.x, y, blockPos.y)) && getBiomeId(moistness, temp) == 3) {
                        if (y < 25) {
                            setBlockAt(x,y,z, LAVA);
                        } else {
                            setBlockAt(x, y, z, EMPTY); // CAVES
                            if (getBlockAt(x,y-1,z) == STONE) {
                                if (rand() % 10 == 9) {
                                    if (rand() % 2 == 1) {
                                        setBlockAt(x,y,z, CAVE_PLANT);
                                    } else {
                                        setBlockAt(x,y,z, CAVE_PLANT_SMOL);
                                    }
                                }
                            }
                        }
                    } else {
                        setBlockAt(x, y, z, STONE);
                        if (y-1 > 25) {
                            if (getBlockAt(x,y-1,z) == EMPTY && rand() % 10 == 9) {
                                for (int i = y - 1; i > y - (rand() % 7); i--) {
                                    if (getBlockAt(x,i,z) != LAVA && getBlockAt(x,i-1,z) != LAVA) {
                                        if (i == y-1) {
                                            setBlockAt(x,i,z, CAVE_VINE_ROOT);
                                        } else {
                                            setBlockAt(x,i,z, CAVE_VINE);
                                        }
                                    }
                                }
                            }
                        }
                    }

                } else if (y > 128 && y < 255) {
                    setBlockAt(x, y, z, getAppropriateBiomeBlock(id, moistness, temp, y, finalHt));
                }


            }


            glm::vec2 islandBound = terrainNoise::floatingIslandNoise(glm::vec3(blockPos.x, y, blockPos.y));
            if(id == 4 && islandBound.x != islandBound.y) { //only generate floating islands for tianzi
                for(y = glm::max(150.f, islandBound.x); y < islandBound.y; y++) {
                    if(getBlockAt(x, y, z) == EMPTY) {
                        if(y == islandBound.y - 1) {
                            setBlockAt(x, y, z, GRASS); // FLOATY ISLANDS
                        } else {
                            setBlockAt(x, y, z, STONE); // FLOATY ISLANDS
                        }
                    }
                }

                if (terrainNoise::fbm(glm::vec2(blockPos.x, blockPos.y)) > 0.3f) {
                    int vineLength = terrainNoise::fbm(glm::vec2(blockPos.x, blockPos.y)) * (rand() % 5 + 2);
                    int vineRoot = glm::max(150.f, islandBound.x) - 1;
                    while(getBlockAt(x, vineRoot, z) == EMPTY && glm::max(150.f, islandBound.x) - 1 - vineRoot < vineLength) {
                        if(y == glm::max(150.f, islandBound.x) - 1) {
                            setBlockAt(x, vineRoot, z, NORMAL_VINE_ROOT); // FLOATY ISLANDS
                        } else {
                            setBlockAt(x, vineRoot, z, NORMAL_VINE); // FLOATY ISLANDS
                        }
                        vineRoot--;
                    }
                }

            }


            if(getBlockAt(x, 138, z) == EMPTY) { //Water Logic
                setBlockAt(x, finalHt, z, SAND);

                for(int y = std::max(128.0f, finalHt + 1.0f); y < 138; y++){
                    setBlockAt(x, y, z, WATER);
                    if((y == finalHt + 1.f) && (getBlockAt(x,y,z) == WATER) && (y < 137) && terrainNoise::fbm(glm::vec2(x,z)) > 0.92f){
                        setBlockAt(x, finalHt + 1, z, SEA_GRASS);
                    }
                }
            } else {
                setBlockAt(x, finalHt, z, getAppropriateBiomeBlock(id, moistness, temp, finalHt, finalHt)); //Final Block
                addTerrainObjects(finalHt, id, moistness, temp, x, z);
                if(id == 4) {
                    addTerrainObjects(islandBound.y - 1, id, moistness, temp, x, z);
                }

            }

        }
    }
}

void Chunk::addTerrainObjects(float finalHt, int id, float moistness, float temp, int x, int z) { //ADDED FUNCTION to be able to add objects to all different hts
    if(getAppropriateBiomeBlock(id, moistness, temp, finalHt, finalHt) == GRASS) {
        if(terrainNoise::fbm(glm::vec2(x,z)) > 0.9f){
            setBlockAt(x, finalHt + 1, z, S_GRASS);
        } else if (finalHt + 1 < 175) {
            if (rand() % 90 == 89) {
                setBlockAt(x, finalHt + 1, z, RED_PLANT);
            } else if (rand() % 90 == 89) {
                setBlockAt(x, finalHt + 1, z, YELLOW_PLANT);
            } else if (rand() % 90 == 89) {
                setBlockAt(x, finalHt + 1, z, GREEN_PLANT);
            } else if (rand() % 90 == 89) {
                setBlockAt(x, finalHt + 1, z, FLOATY_PARTICLES_1);
            } else if (rand() % 90 == 89) {
                setBlockAt(x, finalHt + 1, z, FLOATY_PARTICLES_2);
            }
        }

    } else if(getBlockAt(x, finalHt, z) != BEDROCK)  {
        if(getBlockAt(x,finalHt,z) == TEAL_STONE && rand() % 90 == 89) {
            setBlockAt(x,finalHt+1,z,BLUE_MOUNT_PLANT);
        } else if (getBlockAt(x,finalHt,z) == MOSSY_STONE && rand() % 90 == 89) {
            setBlockAt(x,finalHt + 1,z,GREEN_PLANT);
        } else if (getBlockAt(x,finalHt,z) == SAND && getBlockAt(x,finalHt+1,z) != WATER && rand() % 90 == 89) {
            setBlockAt(x, finalHt + 1, z, PALM_FAN);
        } else if (getBlockAt(x,finalHt,z) == SAND && getBlockAt(x,finalHt+1,z) != WATER && rand() % 90 == 89) {
            int random = rand() % 4;
            for (int i = 1; i < random + 1; i++) {
                setBlockAt(x, finalHt + i, z, STEM);
                setBlockAt(x, finalHt+ i + 1, z, FLOWER);
            }
        }
        else if (rand() % 90 == 89) {
            setBlockAt(x, finalHt + 1, z, FLOATY_PARTICLES_1);

        } else if (rand() % 90 == 89) {
            setBlockAt(x, finalHt + 1, z, FLOATY_PARTICLES_2);
        }
    }


    if(id == 4) {
        if(rand() % 30 == 1){
            drawTree(x, finalHt, z);
        }
    } else {
        if(rand() % 90 == 89){
            drawTree(x, finalHt, z);
        }
    }


    if (x == 8 && z == 8 && getBlockAt(x,finalHt,z) == GRASS && rand() % 10 == 9 && id != 4 ) {
        drawLTree(x,finalHt+1,z);
    }

    if (x == 8 && z == 8 && getBlockAt(x,finalHt+1,z) != WATER && getBlockAt(x,finalHt,z) == SAND && rand() % 10 == 9) {
        drawPalm(x,finalHt+1,z);
    }

}

float Chunk::getBiomeSectionHeight(float moistLevel, float tempLevel, std::map<int, float> *iDToHeight) {

    float M_THRESH = 0.8f;
    float T_THRESH1 = -0.2f;
    float T_THRESH2 = 0.2f;
    float DM = 0.1f;
    float DT = 0.1f;

    float wu = glm::smoothstep(M_THRESH - DM, M_THRESH + DM, moistLevel);
    float wv1 = glm::smoothstep(T_THRESH1 - DT, T_THRESH1 + DT, tempLevel);
    float wv2 = glm::smoothstep(T_THRESH2 - DT, T_THRESH2 + DT, tempLevel);


    float snowHeight = iDToHeight->at(0);
    float forestHeight = iDToHeight->at(1);
    float waterMountHeight = iDToHeight->at(2);
    float spikyMountHeight = iDToHeight->at(3);
    float pillarsHeight = iDToHeight->at(4);
    float islandHeight = iDToHeight->at(5);

    // 100% biome
    if (moistLevel <= M_THRESH - DM && tempLevel > T_THRESH2 + DT) { // Snow
        //qDebug("only snow");
        return snowHeight;
        //return glm::clamp(snowHeight, 0.f, 160.f);
    } else if (moistLevel <= M_THRESH - DM && tempLevel <= T_THRESH2 - DT &&
               tempLevel > T_THRESH1 + DT) { // Forest
        //qDebug("only forest");
        return forestHeight;
        //return glm::clamp(0.f, 160.f, forestHeight);
    } else if (moistLevel <= M_THRESH - DM && tempLevel <= T_THRESH1 - DT) { // WM
        //qDebug("only water mountain");
        return waterMountHeight;
        // return glm::clamp(0.f, 160.f, waterMountHeight);
    } else if (moistLevel > M_THRESH + DM && tempLevel > T_THRESH2 + DT) { // SP
        //qDebug("only spiky mountain");
        return spikyMountHeight;
        //return glm::clamp(0.f, 160.f, spikyMountHeight);
    } else if (moistLevel > M_THRESH + DM && tempLevel <= T_THRESH2 - DT &&
               tempLevel > T_THRESH1 + DT) { // Pillars
        //qDebug("only pillars");
        return pillarsHeight;
        //return glm::clamp(0.f, 160.f, pillarsHeight);
    } else if (moistLevel > M_THRESH + DM && tempLevel <= T_THRESH1 - DT) { // Is
        //qDebug("only island");
        return islandHeight;
        //return glm::clamp(0.f, 160.f, islandHeight);
    }

    // biome mixing only horizontally
    if (moistLevel > M_THRESH - DM && moistLevel <= M_THRESH + DM
            && tempLevel > T_THRESH2 + DT) { // Snow + SpM
        //qDebug("mixing spiky mountain & snow");
        return glm::mix(snowHeight, spikyMountHeight, wu);
    } else if (moistLevel > M_THRESH - DM && moistLevel <= M_THRESH + DM
               && tempLevel > T_THRESH1 + DT && tempLevel <= T_THRESH2 - DT) { // For + Pill
        //qDebug("mixing forest & pillars");
        return glm::mix(forestHeight, pillarsHeight, wu);
    } else if (moistLevel > M_THRESH - DM && moistLevel <= M_THRESH + DM
               && tempLevel <= T_THRESH1 - DT) { // water mount + island
        //qDebug("mixing watermount & island");
        return glm::mix(waterMountHeight, islandHeight, wu);
    }

    // biome mixing only vertically
    if (moistLevel <= M_THRESH - DM
            && tempLevel > T_THRESH2 - DT
            && tempLevel <= T_THRESH2 + DT) { // Snow + forest
        //qDebug("mixing forest & snow");
        // changed snow, forest
        return glm::mix(forestHeight, snowHeight, wv2);
    } else if (moistLevel <= M_THRESH - DM
               && tempLevel > T_THRESH1 - DT
               && tempLevel <= T_THRESH1 + DT) { // For + WM
        //qDebug("mixing forest & water mountain");
        return glm::mix(waterMountHeight, forestHeight, wv1);
    } else if (moistLevel > M_THRESH + DM
               && tempLevel <= T_THRESH2 + DT
               && tempLevel > T_THRESH2 - DT) { // SpM + Pill
        //qDebug("mixing spiky mountain & pill");
        return glm::mix(pillarsHeight, spikyMountHeight, wv2);
    } else if (moistLevel > M_THRESH + DM
               && tempLevel <= T_THRESH1 + DT
               && tempLevel > T_THRESH1 - DT) { // Is + Pill
        //qDebug("mixing island & pillars");
        return glm::mix(islandHeight, pillarsHeight, wv1);
        //return glm::mix(pillarsHeight, islandHeight, wv1);
    }

    // biome mixing four biomes
    if (moistLevel > M_THRESH - DM
            && moistLevel <= M_THRESH + DM
            && tempLevel > T_THRESH2 - DT
            && tempLevel <= T_THRESH2 + DT) { // Snow + forest + spM + pill
        //qDebug("mixing spiky & pill & forest & snow");
        return glm::mix(glm::mix(forestHeight, snowHeight, wv2),
                        glm::mix(pillarsHeight, spikyMountHeight, wv2), wu);
    } else { // forest + waterMount + pill + island
        //qDebug("mixing water mount & pillars & forest & snow");
        return glm::mix(glm::mix(waterMountHeight, forestHeight, wv1),
                        glm::mix(islandHeight, pillarsHeight, wv1), wu);
    }
}


/*
 * Snow: 0
 * Forest/Plains: 1
 * Water Mountain: 2
 * Spiky Mountain: 3
 * Pillars: 4
 * Island: 5
 */
int Chunk::getBiomeId(float moistLevel, float tempLevel) {
    float M_THRESH = 0.8f;
    float T_THRESH1 = -0.2f;
    float T_THRESH2 = 0.2f;
    //    float DM = 0.1f;
    //    float DT = 0.1f;

    if (moistLevel <= M_THRESH && tempLevel > T_THRESH2) { // Snow
        //qDebug() << "in snow";
        return 0;
    } else if (moistLevel <= M_THRESH && tempLevel <= T_THRESH2 &&
               tempLevel > T_THRESH1) { // Forest
        //qDebug() << "in forest";

        return 1;
    } else if (moistLevel <= M_THRESH && tempLevel <= T_THRESH1) { // WM
        //qDebug() << "in wm";

        return 2;
    } else if (moistLevel > M_THRESH && tempLevel > T_THRESH2) { // SP
        //qDebug() << "in spiky mountain";

        return 3;
    } else if (moistLevel > M_THRESH && tempLevel <= T_THRESH2 &&
               tempLevel > T_THRESH1) { // Pillars
        //qDebug() << "in pillars";

        return 4;
    } else { // Is
        //qDebug() << "in island";

        return 5;
    }
    /*

    if (moistLevel <= m_thresh && tempLevel <= t_thresh1) {   // Water Mountain
        return 2;
    } else if (moistLevel <= m_thresh && tempLevel <= t_thresh2 && tempLevel >= t_thresh1) {   // Forest/Plains
        return 1;
    } else if (moistLevel <= m_thresh && tempLevel >= t_thresh2) {   // Snow
        return 0;
    } else if (moistLevel >= m_thresh && tempLevel <= t_thresh1) { // Island
        return 5;
    } else if (moistLevel >= m_thresh && tempLevel <= t_thresh2) { // Pillars
        return 4;
    } else { // Spiky Mountains
        return 3;
    }
    */
}

BlockType Chunk::getAppropriateBiomeBlock(int id, float moistLevel, float tempLevel, int height, int maxHeight){
    float m_thresh = 0.5f;
    float t_thresh1 = 0.33f;
    float t_thresh2 = 0.66f;

    if (id == 2) {   // Water Mountain
        if (height == maxHeight) {
            return GRASS;
        }
        //        else if (height < 150) {
        //            float randNum = (float) rand() / RAND_MAX;
        //            if (randNum > 0.5) {
        //                return DARK_DIRT;
        //            } else if (randNum > 0.3) {
        //                return DIRT;
        //            } else {
        //                return GR_STONE;
        //            }
        //        }
        else {
            return DARK_DIRT;
        }
    } else if (id == 1) {   // Forest/Plains
        if(height == maxHeight) {
            return GRASS;
        } else if (height > 128){
            return DIRT; // DIRT
        }
    } else if (id == 0) {   // Snow
        if(height == maxHeight && height > 190) {
            return MOSSY_STONE;
        } else {
            return STONE;
        }
    } else if (id == 5) { // Island
        if(height == maxHeight) {
            return SAND;
        } else if (height > 128){
            return SAND;
        }
    } else if (id == 4) { // Pillars
        if(height == maxHeight) {
            return GRASS;
        } else if (height > 128){
            return GR_STONE;
        }
    } else { // Spiky Mountains
        float noise = glm::abs(terrainNoise::perlinNoise2D(glm::vec2(height, maxHeight) / 100.f));
        float fbm = terrainNoise::fbm2(glm::vec2(height, maxHeight) / 80.f);
        float offset =  glm::mod(glm::floor(noise * 100.f), 10.f);
        float offset2 = glm::mod(glm::floor(fbm * 100.f), 10.f);
        float threshold = 160.f + (glm::mod(offset * offset2, 10.f) + offset);
        float randNum;

        if (height > 190.f && height == maxHeight) {
            return SNOW;
        } else if (height > 180.f) {
            randNum = (float) rand() / RAND_MAX;
            if (randNum > 0.3) {
                return SNOW;
            } else {
                return TEAL_STONE;
            }
        } else if (height > threshold) {
            randNum = (float) rand() / RAND_MAX;
            if (height > threshold - 10.f) {
                if (randNum > 0.2) {
                    return TEAL_STONE;
                } else {
                    return BLUE_STONE;
                }
            } else {
                if (randNum > 0.7) {
                    return TEAL_STONE;
                } else {
                    return BLUE_STONE;
                }
            }
        }
        else {
            return BLUE_STONE;
        }
    }


}

glm::vec4 Chunk::indexToPos(glm::vec4 bottomLeft, int index) {
    if(index == 1) {
        return bottomLeft;
    } else if(index == 2) {
        return bottomLeft + glm::vec4(1, 0, 0, 0);
    } else if(index == 3) {
        return bottomLeft + glm::vec4(1, 1, 0, 0);
    } else if(index == 0) {
        return bottomLeft + glm::vec4(0, 1, 0, 0);
    }
};

const std::unordered_map<BlockType, std::unordered_map<Direction, glm::vec4, EnumHash>, EnumHash> Chunk::blockFaces {
    //Basic info for the block [UV.X, UV.Y, Alpha transparency, Whether you can walk through]
    // UV X and Y - number these by the NUMBER OF BLOCKS it is from the bottom left, not number of pixels
    // Alpha Transparency - 1 for opaque, 0.9 for semi-transparent
    // Walk-throughability - 0 for no, 1 for yes.

    { GRASS, {{XPOS, glm::vec4(3, 15, 1, 0)},
              {XNEG, glm::vec4(3, 15, 1, 0)},
              {ZPOS, glm::vec4(3, 15, 1, 0)},
              {ZNEG, glm::vec4(3, 15, 1, 0)},
              {YPOS, glm::vec4(8, 13, 1, 0)},
              {YNEG, glm::vec4(2, 15, 1, 0)}}},
    { LEAVES,{{XPOS, glm::vec4(4, 12, 1, 1)},
              {XNEG, glm::vec4(4, 12, 1, 1)},
              {ZPOS, glm::vec4(4, 12, 1, 1)},
              {ZNEG, glm::vec4(4, 12, 1, 1)},
              {YPOS, glm::vec4(4, 12, 1, 1)},
              {YNEG, glm::vec4(4, 12, 1, 1)}}},
    { PALM_LEAVES,{{XPOS, glm::vec4(4, 5, 1, 0)},
                   {XNEG, glm::vec4(4, 5, 1, 0)},
                   {ZPOS, glm::vec4(4, 5, 1, 0)},
                   {ZNEG, glm::vec4(4, 5, 1, 0)},
                   {YPOS, glm::vec4(4, 5, 1, 0)},
                   {YNEG, glm::vec4(4, 5, 1, 0)}}},
    { WOOD, {{XPOS, glm::vec4(4, 14, 1, 0)},
             {XNEG, glm::vec4(4, 14, 1, 0)},
             {ZPOS, glm::vec4(4, 14, 1, 0)},
             {ZNEG, glm::vec4(4, 14, 1, 0)},
             {YPOS, glm::vec4(5, 14, 1, 0)},
             {YNEG, glm::vec4(5, 14, 1, 0)}}},
    { EYWA_WOOD, {{XPOS, glm::vec4(5, 9, 1, 0)},
                  {XNEG, glm::vec4(5, 9, 1, 0)},
                  {ZPOS, glm::vec4(5, 9, 1, 0)},
                  {ZNEG, glm::vec4(5, 9, 1, 0)},
                  {YPOS, glm::vec4(5, 9, 1, 0)},
                  {YNEG, glm::vec4(5, 9, 1, 0)}}},
    { LOG, {{XPOS, glm::vec4(4, 9, 1, 0)},
            {XNEG, glm::vec4(4, 9, 1, 0)},
            {ZPOS, glm::vec4(4, 9, 1, 0)},
            {ZNEG, glm::vec4(4, 9, 1, 0)},
            {YPOS, glm::vec4(4, 9, 1, 0)},
            {YNEG, glm::vec4(4, 9, 1, 0)}}},
    { PALM_WOOD, {{XPOS, glm::vec4(9, 6, 1, 0)},
                  {XNEG, glm::vec4(9, 6, 1, 0)},
                  {ZPOS, glm::vec4(9, 6, 1, 0)},
                  {ZNEG, glm::vec4(9, 6, 1, 0)},
                  {YPOS, glm::vec4(9, 6, 1, 0)},
                  {YNEG, glm::vec4(9, 6, 1, 0)}}},
    { DIRT, {{XPOS, glm::vec4(2, 15, 1, 0)},
             {XNEG, glm::vec4(2, 15, 1, 0)},
             {ZPOS, glm::vec4(2, 15, 1, 0)},
             {ZNEG, glm::vec4(2, 15, 1, 0)},
             {YPOS, glm::vec4(2, 15, 1, 0)},
             {YNEG, glm::vec4(2, 15, 1, 0)}}},
    { WATER, {{XPOS, glm::vec4(13, 3, 0.9, 1)},
              {XNEG, glm::vec4(13, 3, 0.9, 1)},
              {ZPOS, glm::vec4(13, 3, 0.9, 1)},
              {ZNEG, glm::vec4(13, 3, 0.9, 1)},
              {YPOS, glm::vec4(13, 3, 0.9, 1)},
              {YNEG, glm::vec4(13, 3, 0.9, 1)}}},
    { STONE, {{XPOS, glm::vec4(0, 13, 1, 0)},
              {XNEG, glm::vec4(0, 13, 1, 0)},
              {ZPOS, glm::vec4(0, 13, 1, 0)},
              {ZNEG, glm::vec4(0, 13, 1, 0)},
              {YPOS, glm::vec4(0, 15, 1, 0)},
              {YNEG, glm::vec4(0, 15, 1, 0)}}},
    { SNOW, {{XPOS, glm::vec4(2, 11, 1, 0)},
             {XNEG, glm::vec4(2, 11, 1, 0)},
             {ZPOS, glm::vec4(2, 11, 1, 0)},
             {ZNEG, glm::vec4(2, 11, 1, 0)},
             {YPOS, glm::vec4(2, 11, 1, 0)},
             {YNEG, glm::vec4(2, 11, 1, 0)}}},
    { SAND, {{XPOS, glm::vec4(2, 14, 1, 0)},
             {XNEG, glm::vec4(2, 14, 1, 0)},
             {ZPOS, glm::vec4(2, 14, 1, 0)},
             {ZNEG, glm::vec4(2, 14, 1, 0)},
             {YPOS, glm::vec4(2, 14, 1, 0)},
             {YNEG, glm::vec4(2, 14, 1, 0)}}},

    { BEDROCK, {{XPOS, glm::vec4(1, 14, 1, 0)},
                {XNEG, glm::vec4(1, 14, 1, 0)},
                {ZPOS, glm::vec4(1, 14, 1, 0)},
                {ZNEG, glm::vec4(1, 14, 1, 0)},
                {YPOS, glm::vec4(1, 14, 1, 0)},
                {YNEG, glm::vec4(1, 14, 1, 0)}}},
    { GR_STONE,{{XPOS, glm::vec4(1, 15, 1, 0)},
                {XNEG, glm::vec4(1, 15, 1, 0)},
                {ZPOS, glm::vec4(1, 15, 1, 0)},
                {ZNEG, glm::vec4(1, 15, 1, 0)},
                {YPOS, glm::vec4(1, 15, 1, 0)},
                {YNEG, glm::vec4(1, 15, 1, 0)}}},
    { BLUE_STONE,{{XPOS, glm::vec4(0, 6, 1, 0)},
                  {XNEG, glm::vec4(0, 6, 1, 0)},
                  {ZPOS, glm::vec4(0, 6, 1, 0)},
                  {ZNEG, glm::vec4(0, 6, 1, 0)},
                  {YPOS, glm::vec4(0, 6, 1, 0)},
                  {YNEG, glm::vec4(0, 6, 1, 0)}}},
    { TEAL_STONE,  {{XPOS, glm::vec4(0, 7, 1, 0)},
                    {XNEG, glm::vec4(0, 7, 1, 0)},
                    {ZPOS, glm::vec4(0, 7, 1, 0)},
                    {ZNEG, glm::vec4(0, 7, 1, 0)},
                    {YPOS, glm::vec4(0, 8, 1, 0)},
                    {YNEG, glm::vec4(0, 8, 1, 0)}}},
    { DARK_DIRT,  {{XPOS, glm::vec4(4, 12, 1, 0)},
                   {XNEG, glm::vec4(4, 12, 1, 0)},
                   {ZPOS, glm::vec4(4, 12, 1, 0)},
                   {ZNEG, glm::vec4(4, 12, 1, 0)},
                   {YPOS, glm::vec4(4, 12, 1, 0)},
                   {YNEG, glm::vec4(4, 12, 1, 0)}}},
    { MOSSY_STONE, {{XPOS, glm::vec4(2, 12, 1, 0)},
                    {XNEG, glm::vec4(2, 12, 1, 0)},
                    {ZPOS, glm::vec4(2, 12, 1, 0)},
                    {ZNEG, glm::vec4(2, 12, 1, 0)},
                    {YPOS, glm::vec4(2, 12, 1, 0)},
                    {YNEG, glm::vec4(2, 12, 1, 0)}}},

    //Can walk through it and have transparency
    { LAVA, {{XPOS, glm::vec4(13, 4, 0.9f, 1)},
             {XNEG, glm::vec4(13, 4, 0.9f, 1)},
             {ZPOS, glm::vec4(13, 4, 0.9f, 1)},
             {ZNEG, glm::vec4(13, 4, 0.9f, 1)},
             {YPOS, glm::vec4(13, 4, 0.9f, 1)},
             {YNEG, glm::vec4(13, 4, 0.9f, 1)}}},
    { WATER, {{XPOS, glm::vec4(13, 3, 0.9, 1)},
              {XNEG, glm::vec4(13, 3, 0.9, 1)},
              {ZPOS, glm::vec4(13, 3, 0.9, 1)},
              {ZNEG, glm::vec4(13, 3, 0.9, 1)},
              {YPOS, glm::vec4(13, 3, 0.9, 1)},
              {YNEG, glm::vec4(13, 3, 0.9, 1)}}},

    //Can walk through it but doesn't have transparency
    { S_GRASS, {{R_POS_DIAG, glm::vec4(7, 13, 1, 1)},
                {R_NEG_DIAG, glm::vec4(7, 13, 1, 1)},
                {L_POS_DIAG, glm::vec4(7, 13, 1, 1)},
                {L_NEG_DIAG, glm::vec4(7, 13, 1, 1)}}},
    { CAVE_PLANT,{{R_POS_DIAG, glm::vec4(4, 1, 1, 1)},
                  {R_NEG_DIAG, glm::vec4(4, 1, 1, 1)},
                  {L_POS_DIAG, glm::vec4(4, 1, 1, 1)},
                  {L_NEG_DIAG, glm::vec4(4, 1, 1, 1)}}},
    { CAVE_PLANT_SMOL,{{R_POS_DIAG, glm::vec4(3, 1, 1, 1)},
                       {R_NEG_DIAG, glm::vec4(3, 1, 1, 1)},
                       {L_POS_DIAG, glm::vec4(3, 1, 1, 1)},
                       {L_NEG_DIAG, glm::vec4(3, 1, 1, 1)}}},
    { CAVE_VINE,{{R_POS_DIAG, glm::vec4(15, 9, 1, 1)},
                 {R_NEG_DIAG, glm::vec4(15, 9, 1, 1)},
                 {L_POS_DIAG, glm::vec4(15, 9, 1, 1)},
                 {L_NEG_DIAG, glm::vec4(15, 9, 1, 1)}}},
    { CAVE_VINE_ROOT,{{R_POS_DIAG, glm::vec4(15, 9, 1, 1)},
                      {R_NEG_DIAG, glm::vec4(15, 9, 1, 1)},
                      {L_POS_DIAG, glm::vec4(15, 9, 1, 1)},
                      {L_NEG_DIAG, glm::vec4(15, 9, 1, 1)}}},
    { RED_PLANT,{{R_POS_DIAG, glm::vec4(3, 5, 1, 1)},
                 {R_NEG_DIAG, glm::vec4(3, 5, 1, 1)},
                 {L_POS_DIAG, glm::vec4(3, 5, 1, 1)},
                 {L_NEG_DIAG, glm::vec4(3, 5, 1, 1)}}},
    { YELLOW_PLANT,{{R_POS_DIAG, glm::vec4(5, 4, 1, 1)},
                    {R_NEG_DIAG, glm::vec4(5, 4, 1, 1)},
                    {L_POS_DIAG, glm::vec4(5, 4, 1, 1)},
                    {L_NEG_DIAG, glm::vec4(5, 4, 1, 1)}}},
    { GREEN_PLANT,{{R_POS_DIAG, glm::vec4(8, 10, 1, 1)},
                   {R_NEG_DIAG, glm::vec4(8, 10, 1, 1)},
                   {L_POS_DIAG, glm::vec4(8, 10, 1, 1)},
                   {L_NEG_DIAG, glm::vec4(8, 10, 1, 1)}}},
    { FLOWER,{{R_POS_DIAG, glm::vec4(2, 1, 1, 1)},
              {R_NEG_DIAG, glm::vec4(2, 1, 1, 1)},
              {L_POS_DIAG, glm::vec4(2, 1, 1, 1)},
              {L_NEG_DIAG, glm::vec4(2, 1, 1, 1)}}},
    { STEM,{{R_POS_DIAG, glm::vec4(2, 0, 1, 1)},
            {R_NEG_DIAG, glm::vec4(2, 0, 1, 1)},
            {L_POS_DIAG, glm::vec4(2, 0, 1, 1)},
            {L_NEG_DIAG, glm::vec4(2, 0, 1, 1)}}},
    { STRING,{{R_POS_DIAG, glm::vec4(10, 10, 1, 1)},
              {R_NEG_DIAG, glm::vec4(10, 10, 1, 1)},
              {L_POS_DIAG, glm::vec4(10, 10, 1, 1)},
              {L_NEG_DIAG, glm::vec4(10, 10, 1, 1)}}},
    { STRING_BASE,{{R_POS_DIAG, glm::vec4(10, 10, 1, 1)},
                   {R_NEG_DIAG, glm::vec4(10, 10, 1, 1)},
                   {L_POS_DIAG, glm::vec4(10, 10, 1, 1)},
                   {L_NEG_DIAG, glm::vec4(10, 10, 1, 1)}}},
    { SEA_GRASS, {{R_POS_DIAG, glm::vec4(15, 10, 1, 1)},
                  {R_NEG_DIAG, glm::vec4(15, 10, 1, 1)},
                  {L_POS_DIAG, glm::vec4(15, 10, 1, 1)},
                  {L_NEG_DIAG, glm::vec4(15, 10, 1, 1)}}},
    { PALM_FAN, {{R_POS_DIAG, glm::vec4(3, 8, 1, 1)},
                 {R_NEG_DIAG, glm::vec4(3, 8, 1, 1)},
                 {L_POS_DIAG, glm::vec4(3, 8, 1, 1)},
                 {L_NEG_DIAG, glm::vec4(3, 8, 1, 1)}}},
    { BLUE_MOUNT_PLANT, {{R_POS_DIAG, glm::vec4(15, 8, 1, 1)},
                         {R_NEG_DIAG, glm::vec4(15, 8, 1, 1)},
                         {L_POS_DIAG, glm::vec4(15, 8, 1, 1)},
                         {L_NEG_DIAG, glm::vec4(15, 8, 1, 1)}}},
    { FLOATY_PARTICLES_1, {{R_POS_DIAG, glm::vec4(0, 1, 1, 1)},
                           {R_NEG_DIAG, glm::vec4(0, 1, 1, 1)},
                           {L_POS_DIAG, glm::vec4(1, 1, 1, 1)},
                           {L_NEG_DIAG, glm::vec4(1, 1, 1, 1)}}},

    { FLOATY_PARTICLES_2, {{R_POS_DIAG, glm::vec4(0, 0, 1, 1)},
                           {R_NEG_DIAG, glm::vec4(0, 0, 1, 1)},
                           {L_POS_DIAG, glm::vec4(1, 0, 1, 1)},
                           {L_NEG_DIAG, glm::vec4(1, 0, 1, 1)}}},

    { NORMAL_VINE,{{R_POS_DIAG, glm::vec4(15, 7, 1, 1)},
                   {R_NEG_DIAG, glm::vec4(15, 7, 1, 1)},
                   {L_POS_DIAG, glm::vec4(15, 7, 1, 1)},
                   {L_NEG_DIAG, glm::vec4(15, 7, 1, 1)}}},
    { NORMAL_VINE_ROOT,{{R_POS_DIAG, glm::vec4(15, 7, 1, 1)},
                        {R_NEG_DIAG, glm::vec4(15, 7, 1, 1)},
                        {L_POS_DIAG, glm::vec4(15, 7, 1, 1)},
                        {L_NEG_DIAG, glm::vec4(15, 7, 1, 1)}}},

};


//SFX for the block [MovementMode, Glows, Animateable (gifs), Shininess]
// MovementMode: 0 = no movement, 1 = ocean waves; 2 = STANDEE Wind (static base);  3 = tree Wind (static base); 4 for vertical wiggle; 5 for vertical bob;
// Glows: 0 = no glow, any value > 1 = luminance
// Animateable: 0 = not animatable, x = numFrames for gif - 1
// Shininess: 0 for not; 1 for water shine (blinn phong); 2 for glowy


const std::unordered_map<BlockType, glm::vec4, EnumHash> Chunk::blockTypeSFX {
    { GRASS, glm::vec4(0.f, 0.f, 0.f, 0.f)},
    { DIRT, glm::vec4(0.f, 0.f, 0.f, 0.f)},
    { STONE, glm::vec4(0.f, 0.f, 0.f, 0.f)},
    { SNOW, glm::vec4(0.f, 0.f, 0.f, 0.f)},
    { SAND, glm::vec4(0.f, 0.f, 0.f, 0.f)},
    { BEDROCK, glm::vec4(0.f, 0.f, 0.f, 0.f)},
    { WOOD, glm::vec4(0.f, 0.f, 0.f, 0.f)},
    { LEAVES, glm::vec4(4.f, 0.f, 0.f, 0.f)},
    { PALM_WOOD, glm::vec4(0.f, 0.f, 0.f, 0.f)},
    { PALM_LEAVES, glm::vec4(0.f, 0.f, 0.f, 0.f)},
    { EYWA_WOOD, glm::vec4(0.f, 0.f, 0.f, 0.f)},
    { LOG, glm::vec4(0.f, 0.f, 0.f, 0.f)},

    { CAVE_PLANT, glm::vec4(2.f, 0.f, 0.f, 2.f)},
    { CAVE_PLANT_SMOL, glm::vec4(2.f, 0.f, 0.f, 2.f)},
    { CAVE_VINE, glm::vec4(4.f, 0.f, 0.f, 2.f)},
    { CAVE_VINE_ROOT, glm::vec4(3.f, 0.f, 0.f, 2.f)},
    { FLOATY_PARTICLES_1, glm::vec4(5.f, 0.f, 0, 3.f)},
    { FLOATY_PARTICLES_2, glm::vec4(5.f, 0.f, 0, 3.f)},
    { FLOWER, glm::vec4(4.f, 0.f, 0.f, 2.f)},
    { STEM, glm::vec4(4.f, 0.f, 0.f, 2.f)},
    { STRING, glm::vec4(4.f, 0.f, 0.f, 3.f)},
    { STRING_BASE, glm::vec4(3.f, 0.f, 0.f, 3.f)},


    { RED_PLANT, glm::vec4(2.f, 0.f, 0.f, 2.f)},
    { YELLOW_PLANT, glm::vec4(2.f, 0.f, 0.f, 2.f)},
    { GREEN_PLANT, glm::vec4(2.f, 0.f, 0.f, 2.f)},
    { BLUE_MOUNT_PLANT, glm::vec4(2.f, 0.f, 0.f, 2.f)},
    { PALM_FAN, glm::vec4(2.f, 0.f, 0.f, 2.f)},

    { GR_STONE, glm:: vec4(0.f, 0.f, 0.f, 0.f)},
    { BLUE_STONE, glm:: vec4(0.f, 0.f, 0.f, 0.f)},
    { TEAL_STONE, glm:: vec4(0.f, 0.f, 0.f, 0.f)},
    { DARK_DIRT, glm:: vec4(0.f, 0.f, 0.f, 0.f)},
    { MOSSY_STONE, glm:: vec4(0.f, 0.f, 0.f, 0.f)},

    { WATER, glm::vec4(1.f, 0.f, 2.f, 1.f)},
    { LAVA, glm::vec4(1.f, 0.f, 2.f, 1.f)},

    { S_GRASS, glm::vec4(2.f , 0.f, 0.f, 0.f)}, //set to 0 for the bottom of the grass
    { SEA_GRASS, glm::vec4(2.f , 0.f, 0.f, 0.f)}, //set to 0 for the bottom of the grass

    { NORMAL_VINE, glm::vec4(4.f, 0.f, 0.f, 0.f)},
    { NORMAL_VINE_ROOT, glm::vec4(3.f, 0.f, 0.f, 0.f)},

};


const std::unordered_map<BlockType, int, EnumHash> Chunk::blockTypeCubeType {
    //OPAQUE BLOCKS get 0
    { GRASS, 0},
    { DIRT, 0},
    { STONE, 0},
    { SNOW, 0},
    { SAND, 0},
    { GR_STONE, 0},
    { BLUE_STONE, 0},
    { TEAL_STONE, 0},
    { BEDROCK, 0},
    { WOOD, 0},
    { PALM_WOOD, 0},
    { DARK_DIRT, 0},
    { MOSSY_STONE, 0},
    { PALM_LEAVES, 0},
    { EYWA_WOOD, 0},
    { LOG, 0},

    //TRANSPARENT BLOCKS get 1
    { WATER, 1},
    { LAVA, 1},

    //GRASS/STANDEE BLOCKS get 2
    { S_GRASS, 2},
    { SEA_GRASS, 2},
    { CAVE_PLANT, 2},
    { CAVE_PLANT_SMOL, 2},
    { CAVE_VINE, 2}, { CAVE_VINE_ROOT, 2},
    { NORMAL_VINE, 2}, { NORMAL_VINE_ROOT, 2},
    { RED_PLANT, 2},
    { YELLOW_PLANT, 2},
    { GREEN_PLANT, 2},
    { BLUE_MOUNT_PLANT,2},
    { PALM_FAN, 2},
    { FLOWER, 2},
    { STEM, 2},
    { STRING, 2}, {STRING_BASE, 2},
    { FLOATY_PARTICLES_1, 2}, { FLOATY_PARTICLES_2, 2},

    //Semi-transparent normal blocks that i dont want to fuck over
    { LEAVES, 3}

};

void Chunk::drawPlants(int x, int y, int z) {
    if (y < 170 && x > 0 && x < 16 && z > 0 && z < 16) {
        if (getBlockAt(x,y+1,z) == EMPTY) {
            if (rand() % 5 == 2) {
                setBlockAt(x,y+1,z, RED_PLANT);
            } else if (rand() % 5 == 2) {
                setBlockAt(x,y+1,z, YELLOW_PLANT);
            } else if (rand() % 5 == 2) {
                setBlockAt(x,y+1,z, CAVE_PLANT);
            } else if (rand() % 5 == 2) {
                setBlockAt(x,y+1,z, BLUE_MOUNT_PLANT);
            }
        }
    }
}

void Chunk::drawEywaStrings(int x, int y, int z) {
    if (getBlockAt(x,y-1,z) == EMPTY && rand() % 9 == 4) {
        setBlockAt(x,y-1,z,STRING_BASE);
        if (getBlockAt(x,y-2,z) == EMPTY) {
            setBlockAt(x,y-2,z,STRING);
            if (getBlockAt(x,y-3,z) == EMPTY) {
                setBlockAt(x,y-3,z,STRING);
                if (getBlockAt(x,y-4,z) == EMPTY) {
                    setBlockAt(x,y-4,z,STRING);
                    if (getBlockAt(x,y-5,z) == EMPTY) {
                        setBlockAt(x,y-5,z,STRING);
                        if (getBlockAt(x,y-6,z) == EMPTY) {
                            setBlockAt(x,y-6,z,STRING);
                            if (getBlockAt(x,y-7,z) == EMPTY) {
                                setBlockAt(x,y-7,z,STRING);
                                if (getBlockAt(x,y-8,z) == EMPTY) {
                                    setBlockAt(x,y-8,z,STRING);
                                    if (getBlockAt(x,y-9,z) == EMPTY) {
                                        setBlockAt(x,y-9,z,STRING);
                                        if (getBlockAt(x,y-10,z) == EMPTY) {
                                            setBlockAt(x,y-10,z,STRING);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void Chunk::bresenham(glm::vec3 start, glm::vec3 stop, BlockType b) {
    // Bresenham line drawing algorithm

    float x1 = start[0];
    float x2 = stop[0];
    float y1 = start[1];
    float y2 = stop[1];
    float z1 = start[2];
    float z2 = stop[2];

    float dx = abs(x2 - x1), dy = abs(y2 - y1), dz = abs(z2 - z1);
    float p1, p2;

    float xs = (x2 > x1) ? 1 : -1;
    float ys = (y2 > y1) ? 1 : -1;
    float zs = (z2 > z1) ? 1 : -1;

    // Driving axis is X-axis // YZ Plane
    if (dx >= dy && dx >= dz) {
        p1 = 2 * dy - dx;
        p2 = 2 * dz - dx;
        while (int(x1) != int(x2)){
            if(x1 >= 0 && x1 < 16 && z1 >= 0 && z1 < 16) {
                setBlockAt(int(x1), int(y1), int(z1), b);
                if (b == EYWA_WOOD) {
                    drawEywaStrings(x1, y1, z1);
                } else {
                    drawPlants(x1, y1, z1);
                }
            }
            x1 += xs;
            if (p1 >= 0){
                y1 += ys;
                p1 -= 2 * dx;
            }
            if (p2 >= 0) {
                z1 += zs;
                p2 -= 2 * dx;
            }
            p1 += 2 * dy;
            p2 += 2 * dz;
        }
        if(x1 >= 0 && x1 < 16 && z1 >= 0 && z1 < 16) {
            setBlockAt(int(x1), int(y1), int(z1), b);
            if (b == EYWA_WOOD) {
                drawEywaStrings(x1, y1, z1);
            }else {
                drawPlants(x1, y1, z1);
            }
        }
    }

    // Driving axis is Z-axis // XY Plane
    if (dz >= dy && dz >= dx) {
        p1 = 2 * dy - dz;
        p2 = 2 * dx - dz;
        while (int(z1) != int(z2)){
            if(x1 >= 0 && x1 < 16 && z1 >= 0 && z1 < 16) {
                setBlockAt(int(x1), int(y1), int(z1), b);
                if (b == EYWA_WOOD) {
                    drawEywaStrings(x1, y1, z1);
                }else {
                    drawPlants(x1, y1, z1);
                }
            }
            z1 += zs;
            if (p1 >= 0){
                y1 += ys;
                p1 -= 2 * dz;
            }
            if (p2 >= 0) {
                x1 += xs;
                p2 -= 2 * dz;
            }
            p1 += 2 * dy;
            p2 += 2 * dx;
        }
        if(x1 >= 0 && x1 < 16 && z1 >= 0 && z1 < 16) {
            setBlockAt(int(x1), int(y1), int(z1), b);
            if (b == EYWA_WOOD) {
                drawEywaStrings(x1, y1, z1);
            }else {
                drawPlants(x1, y1, z1);
            }
        }
    }

    // Driving axis is Y-axis // XZ Plane
    if (dy >= dx && dy >= dz) {
        p1 = 2 * dx - dy;
        p2 = 2 * dz - dy;
        while (int(y1) != int(y2)){
            if(x1 >= 0 && x1 < 16 && z1 >= 0 && z1 < 16) {
                setBlockAt(int(x1), int(y1), int(z1), b);
                if (b == EYWA_WOOD) {
                    drawEywaStrings(x1, y1, z1);
                }else {
                    drawPlants(x1, y1, z1);
                }
            }
            y1 += ys;
            if (p1 >= 0){
                x1 += xs;
                p1 -= 2 * dy;
            }
            if (p2 >= 0) {
                z1 += zs;
                p2 -= 2 * dy;
            }
            p1 += 2 * dx;
            p2 += 2 * dz;
        }
        if(x1 >= 0 && x1 < 16 && z1 >= 0 && z1 < 16) {
            setBlockAt(int(x1), int(y1), int(z1), b);
            if (b == EYWA_WOOD) {
                drawEywaStrings(x1, y1, z1);
            }else {
                drawPlants(x1, y1, z1);
            }
        }
    }

}

void Chunk::drawPalm(int x, int y, int z) {
    L_Tree tree = L_Tree("Y", glm::vec3(x,y,z), 3, 6); // set iterations here
    tree.generateTree();

    for (int i = 0; i < tree.branches.size()-1; i++) {
        bresenham(tree.branches[i], tree.branches[i+1],LOG);
    }
}



void Chunk::drawLTree(int x, int y, int z) {

    L_Tree tree = L_Tree("X", glm::vec3(x,y,z), 4, 5); // set iterations here
    tree.generateTree();

    for (int i = 0; i < tree.branches.size()-1; i++) {
        bresenham(tree.branches[i], tree.branches[i+1],EYWA_WOOD);
    }
}
