#include "chunkworkers.h"
#include <iostream>

FBMWorker::FBMWorker(int x, int z, std::vector<Chunk*> chunksToFill,
                     std::unordered_set<Chunk*>* chunksCompleted, QMutex* chunksCompletedLock)
    : m_xCorner(x), m_zCorner(z), m_chunksToFill(chunksToFill), mp_chunksCompleted(chunksCompleted),
      mp_chunksCompletedLock(chunksCompletedLock) {}
void FBMWorker::run() {

    for (Chunk *c : m_chunksToFill) {
//        if (c->hasBlockData) {
//            qDebug() << "chunk already has block data";
//        }
        c->setChunkTerrain(c->getX(), c->getZ());
        c->hasBlockData = true;

        mp_chunksCompletedLock->lock();
//        std::cout << "FBM COMPLETE" << std::endl;
        mp_chunksCompleted->insert(c);
        mp_chunksCompletedLock->unlock();
    }

//    mp_chunksCompletedLock->lock();
//        std::cout << "FBM COMPLETE" << std::endl;
//    mp_chunksCompletedLock->unlock();
}

VBOWorker::VBOWorker(Chunk* c, std::vector<chunkVBO>* dat, QMutex* datLock)
     : mp_chunk(c), mp_chunkVBOsCompleted(dat), mp_chunkVBOsCompletedLock(datLock) {}

void VBOWorker::run() {
//    mp_chunkVBOsCompletedLock->lock();
//    std::cout << "starting VBO stuff" << std::endl;
//    mp_chunkVBOsCompletedLock->unlock();
    chunkVBO c(mp_chunk);
    mp_chunk->createInterleavedData(c);
    if (!mp_chunk->hasVBO) {
        qDebug() << "what the heck";
    }
    mp_chunkVBOsCompletedLock->lock();
//    std::cout << "Adding to VBO output" << std::endl;
    mp_chunkVBOsCompleted->push_back(c);
    mp_chunkVBOsCompletedLock->unlock();
}
bool isTransparent(BlockType t) {
    return false;
  //  return transparent_blocks.find(t) != transparent_blocks.end();
}
