#include "terrain.h"
#include "cube.h"
#include <stdexcept>
#include <iostream>
#include <QThreadPool>
#include "chunkworkers.h"
#include "lsystems.h"

Terrain::Terrain(OpenGLContext *context)
    : m_chunks(), m_generateDTerrain(), m_geomCube(context), mp_context(context), testMode(false), isInitial(true)
{}

Terrain::~Terrain() {
    m_geomCube.destroyVBOdata();
}

void Terrain::addChunk(int x, int z) {
// creating VBO data, but should we be doing
    // this on a worker thread?? but this is only called in
    if (!hasChunkAt(x,z)) {
        Chunk* c = instantiateChunkAt(x,z,false);

        c->createVBOdata();

        updateChunkNeighbors(x, z);
    }
}

void Terrain::updateChunkNeighbors(int x, int z) {

    // update neighbor faces

    if (hasChunkAt(x + 16, z)) {
        Chunk* neighbor = getChunkAt(x + 16, z).get();
        if (neighbor->hasVBO && neighbor->hasBlockData) {
            spawnVBOWorker(neighbor);
        }

    }

    if (hasChunkAt(x - 16, z)) {
        Chunk* neighbor = getChunkAt(x - 16, z).get();
        if (neighbor->hasVBO && neighbor->hasBlockData) {
            spawnVBOWorker(neighbor);
        }
    }

    if (hasChunkAt(x, z + 16)) {
        Chunk* neighbor = getChunkAt(x, z + 16).get();
        if (neighbor->hasVBO && neighbor->hasBlockData) {
            spawnVBOWorker(neighbor);
        }
    }

    if (hasChunkAt(x, z - 16)) {
        Chunk* neighbor = getChunkAt(x, z - 16).get();
        if (neighbor->hasVBO && neighbor->hasBlockData) {
            spawnVBOWorker(neighbor);
        }
    }

}

// Combine two 32-bit ints into one 64-bit int
// where the upper 32 bits are X and the lower 32 bits are Z
int64_t toKey(int x, int z) {
    int64_t xz = 0xffffffffffffffff;
    int64_t x64 = x;
    int64_t z64 = z;

    // Set all lower 32 bits to 1 so we can & with Z later
    xz = (xz & (x64 << 32)) | 0x00000000ffffffff;

    // Set all upper 32 bits to 1 so we can & with XZ
    z64 = z64 | 0xffffffff00000000;

    // Combine
    xz = xz & z64;
    return xz;
}

glm::ivec2 toCoords(int64_t k) {
    // Z is lower 32 bits
    int64_t z = k & 0x00000000ffffffff;
    // If the most significant bit of Z is 1, then it's a negative number
    // so we have to set all the upper 32 bits to 1.
    // Note the 8    V
    if(z & 0x0000000080000000) {
        z = z | 0xffffffff00000000;
    }
    int64_t x = (k >> 32);

    return glm::ivec2(x, z);
}

// Surround calls to this with try-catch if you don't know whether
// the coordinates at x, y, z have a corresponding Chunk
BlockType Terrain::getBlockAt(int x, int y, int z) const
{
    if(hasChunkAt(x, z)) {
        // Just disallow action below or above min/max height,
        // but don't crash the game over it.
        if(y < 0 || y >= 256) {
            return EMPTY;
        }
        const uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        return c->getBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                             static_cast<unsigned int>(y),
                             static_cast<unsigned int>(z - chunkOrigin.y));
    }
    else {
        //qWarning("no chunk!");
        return EMPTY;

//        throw std::out_of_range("Coordinates " + std::to_string(x) +
//                                " " + std::to_string(y) + " " +
//                                std::to_string(z) + " have no Chunk!");
    }
}

BlockType Terrain::getBlockAt(glm::vec3 p) const {
    return getBlockAt(p.x, p.y, p.z);
}

bool Terrain::hasChunkAt(int x, int z) const {
    // Map x and z to their nearest Chunk corner
    // By flooring x and z, then multiplying by 16,
    // we clamp (x, z) to its nearest Chunk-space corner,
    // then scale back to a world-space location.
    // Note that floor() lets us handle negative numbers
    // correctly, as floor(-1 / 16.f) gives us -1, as
    // opposed to (int)(-1 / 16.f) giving us 0 (incorrect!).
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    int64_t k = toKey(16 * xFloor, 16 * zFloor);
    return m_chunks.find(k) != m_chunks.end() && m_chunks.at(k) != nullptr;
}


uPtr<Chunk>& Terrain::getChunkAt(int x, int z) {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks[toKey(16 * xFloor, 16 * zFloor)];
}


const uPtr<Chunk>& Terrain::getChunkAt(int x, int z) const {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks.at(toKey(16 * xFloor, 16 * zFloor));
}

void Terrain::setBlockAt(int x, int y, int z, BlockType t)
{
    if(hasChunkAt(x, z)) {
        uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        c->setBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                      static_cast<unsigned int>(y),
                      static_cast<unsigned int>(z - chunkOrigin.y),
                      t);
    }
    else {
        throw std::out_of_range("Coordinates " + std::to_string(x) +
                                " " + std::to_string(y) + " " +
                                std::to_string(z) + " have no Chunk!");
    }
}

Chunk* Terrain::instantiateChunkAt(int x, int z, bool init) {
    uPtr<Chunk> chunk = mkU<Chunk>(mp_context, x, z);
    Chunk *cPtr = chunk.get();
    m_chunks[toKey(x, z)] = std::move(chunk);
    // Set the neighbor pointers of itself and its neighbors
    if(hasChunkAt(x, z + 16)) {
        auto &chunkNorth = m_chunks[toKey(x, z + 16)];
        cPtr->linkNeighbor(chunkNorth, ZPOS);
    }
    if(hasChunkAt(x, z - 16)) {
        auto &chunkSouth = m_chunks[toKey(x, z - 16)];
        cPtr->linkNeighbor(chunkSouth, ZNEG);
    }
    if(hasChunkAt(x + 16, z)) {
        auto &chunkEast = m_chunks[toKey(x + 16, z)];
        cPtr->linkNeighbor(chunkEast, XPOS);
    }
    if(hasChunkAt(x - 16, z)) {
        auto &chunkWest = m_chunks[toKey(x - 16, z)];
        cPtr->linkNeighbor(chunkWest, XNEG);
    }

    if (init) {
        cPtr->setChunkTerrain(x, z);
    }

    return cPtr;
}

// TODO: When you make Chunk inherit from Drawable, change this code so
// it draws each Chunk with the given ShaderProgram, remembering to set the
// model matrix to the proper X and Z translation!
void Terrain::draw(int minX, int maxX, int minZ, int maxZ, ShaderProgram *shaderProgram) {
    shaderProgram->setTexture(0);
    shaderProgram->setNormalMap(1);

    if(testMode) {
        CreateTestScene(shaderProgram);
    } else {
        drawTerrain(minX, maxX, minZ, maxZ, shaderProgram);
    }
}

void Terrain::drawTerrain(int minX, int maxX, int minZ, int maxZ, ShaderProgram *shaderProgram) {

    shaderProgram->setModelMatrix(glm::mat4());
    for(int x = minX; x <= maxX; x += 16) {
        for(int z = minZ; z <= maxZ; z += 16) {
            if(hasChunkAt(x, z)) {
                //const uPtr<Chunk> &chunk = getChunkAt(x, z);
                Chunk *chunk = getChunkAt(x,z).get();

                if (chunk != nullptr && chunk->isBuffered) {
//                    mp_context->printGLErrorLog();
                    shaderProgram->drawInterleavedOpaque(*chunk);
//                    mp_context->printGLErrorLog();
                }
            }
        }
    }

    for(int x = minX; x < maxX; x += 16) {
        for(int z = minZ; z < maxZ; z += 16) {
            if(hasChunkAt(x, z)) {
                Chunk *chunk = getChunkAt(x,z).get();

                if (chunk != nullptr && chunk->isBuffered) {
                    mp_context->printGLErrorLog();
                    shaderProgram->drawInterleaveDTransparent(*chunk);
                    mp_context->printGLErrorLog();
                }
            }
        }
    }


}
// multi threading stuff
void Terrain::multithreadedWork(glm::vec3 currPos, glm::vec3 prevPos, float DT) {
    // try expansion - did player cross boundary st we need to make more
    // chunks to fill in terrain
//    m_tryExpansionTimer += DT;

    // DT threshold for trying expansion
//    if (m_tryExpansionTimer < 0.5f) {
//        checkThreadResults();
//        return;
//    }
    tryExpansion(currPos, prevPos);
//    m_tryExpansionTimer = 0.f;
    checkThreadResults();
}

void Terrain::createInitialChunks(glm::vec2 pos) {
    glm::ivec2 currZone(64 * glm::floor(pos.x / 64.f),
                        64 * glm::floor(pos.y / 64.f));
    QSet<int64_t> terrainZonesBorderingCurrPos = terrainZonesBorderingZone(currZone, TERRAIN_CREATE_RADIUS, false);

    for(auto t : terrainZonesBorderingCurrPos) {
        spawnFBMWorker(t);
    }
}

QSet<int64_t> Terrain::terrainZonesBorderingZone(glm::ivec2 zoneCoords, unsigned int radius, boolean onlyCircumference) {
    int radiusInZoneSc = static_cast<int>(radius) * 64;

    // a set of identifiers that will have the coordinates of the terrain zone in question
    QSet<int64_t> result;
        for (int i = -radiusInZoneSc; i <= radiusInZoneSc; i+= 64) {
            for (int j = -radiusInZoneSc; j <= radiusInZoneSc; j+= 64) {
                result.insert(toKey(zoneCoords.x + i, zoneCoords.y + j));
            }
        }
//    }
    return result;
}
void Terrain::tryExpansion(glm::vec3 currPos, glm::vec3 prevPos) {

    // find terrain zone player is currently in and
    // terrain zone player was previously in
    // take Player's curr and prev pos and map to closest multiple of 64
    glm::ivec2 currZone(64 * glm::floor(currPos.x / 64.f),
                        64 * glm::floor(currPos.z / 64.f));
    glm::ivec2 prevZone(64 * glm::floor(prevPos.x / 64.f),
                        64 * glm::floor(prevPos.z / 64.f));

    // find boundary of terrain zones bordering curr and prev pos
    // includes ungenerated terrain zones, so we know which ones to generate
    QSet<int64_t> terrainZonesBorderingCurrPos = terrainZonesBorderingZone(currZone, TERRAIN_CREATE_RADIUS, false);
    QSet<int64_t> terrainZonesBorderingPrevPos = terrainZonesBorderingZone(prevZone, TERRAIN_CREATE_RADIUS, false);

    // lock in beginning, before loop. add loops. then unlock
    // minimize the time that anything is locked

    // check for which terrain zones need to be destroyed
    // if previously in radius but aren't anymore - should be destroyed
    for (auto id : terrainZonesBorderingPrevPos) {
        if (!terrainZonesBorderingCurrPos.contains(id)) {
            glm::ivec2 coord = toCoords(id);
            for (int x = coord.x; x < coord.x + 64; x += 16) {
                for (int z = coord.y; z < coord.y + 64; z += 16) {
                   if (hasChunkAt(x, z)) { //???? (-192, 64)
                        auto &chunk = getChunkAt(x, z);
                        chunk->destroyVBOdata();
                        //chunk->hasVBO = false;  //????
                        //chunk->isBuffered = false;  //????
                   }
                }
            }
        }
    }

    // find which terrain zones around current position need VBO data to be created
    // send to VBOWorkers, or FBMWorkers if doesn't exist in global map
    for (auto id : terrainZonesBorderingCurrPos) {
        glm::ivec2 coord = toCoords(id);
        // already exists but not in prev set - then send to VBOWorker
        if (terrainZoneExists(id)) {
            if (!terrainZonesBorderingPrevPos.contains(id)) {
               // glm::ivec2 coord = toCoords(id);
                for (int x = coord.x; x < coord.x + 64; x += 16) {
                    for (int z = coord.y; z < coord.y + 64; z += 16) {
                        auto &chunk = getChunkAt(x, z);
                        spawnVBOWorker(chunk.get());
                    }
                }
            }
        } else {
            spawnFBMWorker(id);
        }
    }
}
bool Terrain::terrainZoneExists(int64_t id) {
    int count =  m_generateDTerrain.count(id);
    return count != 0;
}
void Terrain::spawnFBMWorker(int64_t zoneToGenerate) {
    m_generateDTerrain.insert(zoneToGenerate);
    vector<Chunk*> chunksForWorker;
    glm::ivec2 coords = toCoords(zoneToGenerate);
    for (int x = coords.x; x < coords.x + 64; x += 16) {
        for (int z = coords.y; z < coords.y + 64; z += 16) {
            Chunk* c;

            // not sure if need to check if there is chunk,
            // since we assume this terrain zone doesn't exist
            if (!hasChunkAt(x, z)) {
                c = instantiateChunkAt(x, z, false);
//                c->m_countOpq = 0;
//                c->m_countTra = 0;
                //updateChunkNeighbors(x, z);
            } else {
                c = getChunkAt(x, z).get();
            }
            //c = instantiateChunkAt(x, z);
            chunksForWorker.push_back(c);
        }
    }
    FBMWorker *worker = new FBMWorker(coords.x, coords.y, chunksForWorker,
                                      &m_chunksThatHaveBlockData, &m_chunksThatHaveBlockDataLock);
    QThreadPool::globalInstance()->start(worker);
   // worker->run();
}
void Terrain::spawnFBMWorkers(const QSet<int64_t> &zonesToGenerate) {
    for (int64_t zone : zonesToGenerate) {
       // glm::ivec2 coord = toCoords(zone);
        spawnFBMWorker(zone);
    }
}
void Terrain::spawnVBOWorker(Chunk* chunkNeedingVBOData) {
    VBOWorker *worker = new VBOWorker(chunkNeedingVBOData, &m_chunksThatHaveVBOs,
                                      &m_chunksThatHaveVBOsLock);
    QThreadPool::globalInstance()->start(worker);
}
void Terrain::spawnVBOWorkers(const std::unordered_set<Chunk*> &chunksNeedingVBOs) {
    for (Chunk * c : chunksNeedingVBOs) {
        spawnVBOWorker(c);
        updateChunkNeighbors(c->getX(), c->getZ());
        // if a neighbor has vbo data
    }
}
void Terrain::checkThreadResults() {
    // sends chunks that have been processed by FBMWorkers to VBOWorkers for VBO data
    m_chunksThatHaveBlockDataLock.lock();
    spawnVBOWorkers(m_chunksThatHaveBlockData);
    m_chunksThatHaveBlockData.clear();
    m_chunksThatHaveBlockDataLock.unlock();
    // collect chunks w VBO data and send to GPU
    m_chunksThatHaveVBOsLock.lock();
    for(chunkVBO &cd : m_chunksThatHaveVBOs) {
        if (cd.chunk->isBuffered) {
            // sent back
            cd.chunk->destroyVBOdata();
        }
      //  else {
            cd.chunk->bufferInterleavedData(cd);
            cd.chunk->isBuffered = true;
     //   }
    }
    m_chunksThatHaveVBOs.clear();
    m_chunksThatHaveVBOsLock.unlock();


}

void Terrain::CreateTestScene(ShaderProgram *shaderProgram) //Used for testing purposes only
{
    shaderProgram->setModelMatrix(glm::mat4());
    for(int x = -128; x < 128; x += 16) {
        for(int z = -128; z < 128; z += 16) {
            addChunk(x,z);
            if(hasChunkAt(x, z)) {
                const uPtr<Chunk> &chunk = getChunkAt(x, z);
                shaderProgram->drawInterleavedOpaque(*chunk);
            }
        }
    }

    for(int x = -128; x < 128; x += 16) {
        for(int z = -128; z < 128; z += 16) {
            if(hasChunkAt(x, z)) {
                const uPtr<Chunk> &chunk = getChunkAt(x, z);
                shaderProgram->drawInterleaveDTransparent(*chunk);
            }
        }
    }

}
