#pragma once
#include "smartpointerhelp.h"
#include "glm_includes.h"
#include <array>
#include <unordered_map>
#include <cstddef>
#include "drawable.h"
#include "terrainNoise.h"
#include <QMutex>
#include "lsystems.h"


//using namespace std;

// C++ 11 allows us to define the size of an enum. This lets us use only one byte
// of memory to store our different block types. By default, the size of a C++ enum
// is that of an int (so, usually four bytes). This *does* limit us to only 256 different
// block types, but in the scope of this project we'll never get anywhere near that many.
enum BlockType : unsigned char
{
    EMPTY, GRASS, DIRT, STONE, WATER, SNOW, SAND, LAVA, BEDROCK, S_GRASS, WOOD, LEAVES, PALM_WOOD, SEA_GRASS,
    CAVE_PLANT, CAVE_PLANT_SMOL, CAVE_VINE, CAVE_VINE_ROOT,
    YELLOW_PLANT, RED_PLANT, GREEN_PLANT,
    GR_STONE, BLUE_STONE, TEAL_STONE, DARK_DIRT,
    PALM_FAN, PALM_LEAVES,
    MOSSY_STONE, BLUE_MOUNT_PLANT, EYWA_WOOD, STRING,
    FLOWER, STEM, LOG,
    STRING_BASE,
    FLOATY_PARTICLES_1, FLOATY_PARTICLES_2, NORMAL_VINE, NORMAL_VINE_ROOT
};

// The six cardinal directions in 3D space
enum Direction : unsigned char
{
    XPOS, XNEG, YPOS, YNEG, ZPOS, ZNEG, R_POS_DIAG, R_NEG_DIAG, L_POS_DIAG, L_NEG_DIAG
};

//#define M_THRESH 0.f
////#define M_THRESH2 1.f
//#define T_THRESH1 -0.33f
//#define T_THRESH2 0.33f
//#define DM 0.1f
//#define DT 0.1f
//#define T_THRESH3 1.f

// Lets us use any enum class as the key of a
// std::unordered_map
struct EnumHash {
    template <typename T>
    size_t operator()(T t) const {
        return static_cast<size_t>(t);
    }
};

class Chunk;

struct chunkVBO {
    Chunk* chunk;
    std::vector<glm::vec4> m_vboDataOpaque;
    std::vector<GLuint> m_idxOpaque;

    std::vector<glm::vec4> m_vboDataTransparent;
    std::vector<GLuint> m_idxTransparent;


    chunkVBO() {}

    chunkVBO(Chunk* c) : chunk(c), m_vboDataOpaque{},
        m_idxOpaque{}, m_vboDataTransparent{}, m_idxTransparent{} {}
};

// One Chunk is a 16 x 256 x 16 section of the world,
// containing all the Minecraft blocks in that area.
// We divide the world into Chunks in order to make
// recomputing its VBO data faster by not having to
// render all the world at once, while also not having
// to render the world block by block.

// TODO have Chunk inherit from Drawable
class Chunk : public Drawable{
private:
    // All of the blocks contained within this Chunk
    std::array<BlockType, 65536> m_blocks;
    int xCoord, zCoord;
    // This Chunk's four neighbors to the north, south, east, and west
    // The third input to this map just lets us use a Direction as
    // a key for this map.
    // These allow us to properly determine
    std::unordered_map<Direction, Chunk*, EnumHash> m_neighbors;

    OpenGLContext* context;

    const static std::unordered_map<BlockType, std::unordered_map<Direction, glm::vec4, EnumHash>, EnumHash> blockFaces;
    const static std::unordered_map<BlockType, glm::vec4, EnumHash> blockTypeSFX;
    const static std::unordered_map<BlockType, int, EnumHash> blockTypeCubeType;

public:
    int m_countOpq;
    int m_countTra;

    bool isBuffered;
    bool hasVBO;
    //bool hasVBOLoaded; // unnecessary
    bool hasBlockData;

    QMutex chunkLock;

    void bufferInterleavedData(chunkVBO &chunkvbo);

    Chunk(OpenGLContext* context, int x, int z);
    BlockType getBlockAt(unsigned int x, unsigned int y, unsigned int z) const;
    BlockType getBlockAt(int x, int y, int z) const;
    void setBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t);
    void linkNeighbor(uPtr<Chunk>& neighbor, Direction dir);

    void createVBOdata() override;
    void destroyVBOdata() override;

    void createInterleavedData(chunkVBO &c);

    // multithreading - secondary create function
//    void bufferInterleavedWorker(const std::vector<glm::vec4> &vrtxDataOpq, const std::vector<GLuint> &idxDataOpq,
//                       const std::vector<glm::vec4> &vrtxDataTra, const std::vector<GLuint> &idxDataTra);

    GLenum drawMode() override;

    void updateFaceVBOs(BlockType, glm::vec3, glm::vec4, int*, int*, BlockType, Direction, chunkVBO &chunkvbo);
    void createCubeData(int x, int y, int z, BlockType* curr, glm::vec3& currPos, int* opaqueFaces, int* transpFaces, chunkVBO* chunkvbo);
    void createStandeeData(int x, int y, int z, BlockType* curr, glm::vec3& currPos, int* opaqueFaces, int* transpFaces, chunkVBO* chunkvbo);

    glm::vec3 findColor(BlockType t);
    glm::vec4 findPos(Direction dir, int i);

    void setChunkTerrain(int chunkX, int chunkZ);
    BlockType getAppropriateBiomeBlock(int id, float moistLevel, float tempLevel, int height, int maxHeight);
    int getBiomeId(float moistLevel, float tempLevel);
    glm::vec4 indexToPos(glm::vec4 bottomLeft, int index);

    float getBiomeSectionHeight(float moistLevel, float tempLevel, std::map<int, float> *iDToHeight);
    float interpFourTerrains(float h1, float h2, float h3, float h4);

    int getX();
    int getZ();

    float calcTerrainFinalHeight(glm::vec2 blockPos);

    void addTerrainObjects(float finalHt, int id, float moistness, float temp, int x, int y);

    std::vector<Chunk*> getNeighbors();

    // procedural objects
    bool drawTree(int x, int y, int z);

    // l-system tree
    void drawLTree(int x, int y, int z);
    void bresenham(glm::vec3, glm::vec3, BlockType);
    void drawEywaStrings(int x, int y, int z);

    void drawPalm(int x, int y, int z);
    void drawPlants(int x, int y, int z);
};
