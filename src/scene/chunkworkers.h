#pragma once
#include "chunkworkers.h"
#include <QRunnable>
#include <QMutex>
#include <QThread>
#include <unordered_set>
#include "chunk.h"

class FBMWorker : public QRunnable {
private:
    // coords of terrain zone being generated
    int m_xCorner, m_zCorner;
    std::vector<Chunk*> m_chunksToFill; // iterate through this, for each chunk, generate terrain
    std::unordered_set<Chunk*>* mp_chunksCompleted;     // chunk pointers
    QMutex *mp_chunksCompletedLock;     // resource that thread shares w main thread of minecraft
public:
    FBMWorker(int x, int z, std::vector<Chunk*> chunksToFill,
              std::unordered_set<Chunk*>* chunksCompleted, QMutex* chunksCompletedLock);
    // compute noise functions
    // fill results in to a chunk's block type data
    void run() override;
};

// move this stuff into chunk's struct
// in chunk.h ?
/*
struct ChunkVBOData {
    // completely implement after Diana's done w texturing
    // compute transparent and opaque vbos separately

    // transparent for water

    std::vector<float> m_vrtxDataOpq;
    std::vector<GLuint> m_idxDataOpq;
    std::vector<float> m_vrtxDataTra;
    std::vector<GLuint> m_idxDataTra;

    Chunk* mp_chunk;

    ChunkVBOData(Chunk* c) : mp_chunk(c), m_vrtxDataOpq{},
        m_vrtxDataTra{}, m_idxDataOpq{}, m_idxDataTra{} {}
};
*/

bool isTransparent(BlockType t);

class VBOWorker : public QRunnable {
private:
    Chunk* mp_chunk;
    std::vector<chunkVBO>* mp_chunkVBOsCompleted;
    QMutex *mp_chunkVBOsCompletedLock;

public:
    VBOWorker(Chunk *c, std::vector<chunkVBO>* dat, QMutex* datLock);
    void run() override;

    // in Chunk class ?
  //  static void buildVBODataForChunk(Chunk *chunk, ChunkVBOData &c);
};

