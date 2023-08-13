#include "drawable.h"
#include <glm_includes.h>

Drawable::Drawable(OpenGLContext* context)
    : m_countOpaque(-1), m_countTransparent(-1), m_bufOpaqueIdx(), m_bufTransparentIdx(), m_bufPos(), m_bufNor(), m_bufCol(), m_bufUV(),
      m_idxGeneratedOpaque(false),  m_idxGenerateDTransparent(false), m_posGenerated(false), m_norGenerated(false), m_colGenerated(false), m_UVGenerated(false),
      m_interleaveGeneratedOpaque(false), m_interleaveGenerateDTransparent(false), m_SFXGeneratedOpaque(false), m_SFXGenerateDTransparent(false),
      mp_context(context)
{}

Drawable::~Drawable()
{}


void Drawable::destroyVBOdata()
{
    mp_context->glDeleteBuffers(1, &m_bufOpaqueIdx);
    mp_context->glDeleteBuffers(1, &m_bufTransparentIdx);
    mp_context->glDeleteBuffers(1, &m_bufPos);
    mp_context->glDeleteBuffers(1, &m_bufNor);
    mp_context->glDeleteBuffers(1, &m_bufCol);
    mp_context->glDeleteBuffers(1, &m_bufInterleavedOpaque);
    mp_context->glDeleteBuffers(1, &m_bufInterleaveDTransparent);

    mp_context->glDeleteBuffers(1, &m_bufSFXOpaque);
    mp_context->glDeleteBuffers(1, &m_bufSFXTransparent);

    m_idxGeneratedOpaque = m_idxGenerateDTransparent, m_posGenerated = m_norGenerated = m_colGenerated = m_interleaveGeneratedOpaque = m_interleaveGenerateDTransparent = m_SFXGeneratedOpaque = m_SFXGenerateDTransparent =   false;
    m_countOpaque = -1;
    m_countTransparent = -1;
}

GLenum Drawable::drawMode()
{
    // Since we want every three indices in bufIdx to be
    // read to draw our Drawable, we tell that the draw mode
    // of this Drawable is GL_TRIANGLES

    // If we wanted to draw a wireframe, we would return GL_LINES

    return GL_TRIANGLES;
}

int Drawable::elemCountOpaque()
{
    return m_countOpaque;
}


int Drawable::elemCountTransparent()
{
    return m_countTransparent;
}


void Drawable::generateOpaqueIdx()
{
    m_idxGeneratedOpaque = true;
    // Create a VBO on our GPU and store its handle in bufIdx
    mp_context->glGenBuffers(1, &m_bufOpaqueIdx);
}

void Drawable::generateTransparentIdx()
{
    m_idxGenerateDTransparent = true;
    // Create a VBO on our GPU and store its handle in bufIdx
    mp_context->glGenBuffers(1, &m_bufTransparentIdx);
}

void Drawable::generatePos()
{
    m_posGenerated = true;
    // Create a VBO on our GPU and store its handle in bufPos
    mp_context->glGenBuffers(1, &m_bufPos);
}

void Drawable::generateNor()
{
    m_norGenerated = true;
    // Create a VBO on our GPU and store its handle in bufNor
    mp_context->glGenBuffers(1, &m_bufNor);
}

void Drawable::generateCol()
{
    m_colGenerated = true;
    // Create a VBO on our GPU and store its handle in bufCol
    mp_context->glGenBuffers(1, &m_bufCol);
}

void Drawable::generateUV() {
    m_UVGenerated = true;
    mp_context->glGenBuffers(1,&m_bufUV);
}

void Drawable::generateInterleavedOpaque() {
    m_interleaveGeneratedOpaque = true;
    mp_context->glGenBuffers(1, &m_bufInterleavedOpaque);
}

void Drawable::generateInterleaveDTransparent() {
    m_interleaveGenerateDTransparent= true;
    mp_context->glGenBuffers(1, &m_bufInterleaveDTransparent);
}

bool Drawable::bindInterleavedOpaque() {
    if(m_interleaveGeneratedOpaque) {
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufInterleavedOpaque);
    }
    return m_interleaveGeneratedOpaque;
}

bool Drawable::bindInterleaveDTransparent() {
    if(m_interleaveGenerateDTransparent) {
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufInterleaveDTransparent);
    }
    return m_interleaveGenerateDTransparent;
}

void Drawable::generateOpaqueSFX() {
    m_SFXGeneratedOpaque = true;
    mp_context->glGenBuffers(1, &m_bufSFXOpaque);
}

void Drawable::generateTransparentSFX() {
    m_SFXGenerateDTransparent = true;
    mp_context->glGenBuffers(1, &m_bufSFXTransparent);
}

bool Drawable::bindOpaqueIdx()
{
    if(m_idxGeneratedOpaque) {
        mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufOpaqueIdx);
    }
    return m_idxGeneratedOpaque;
}

bool Drawable::binDTransparentIdx()
{
    if(m_idxGenerateDTransparent) {
        mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufTransparentIdx);
    }
    return m_idxGenerateDTransparent;
}

bool Drawable::bindOpaqueSFX()
{
    if(m_SFXGeneratedOpaque){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufSFXOpaque);
    }
    return m_SFXGeneratedOpaque;
}


bool Drawable::binDTransparentSFX()
{
    if(m_SFXGenerateDTransparent){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufSFXTransparent);
    }
    return m_SFXGenerateDTransparent;
}

bool Drawable::bindUV() {
    if (m_UVGenerated) {
        mp_context -> glBindBuffer(GL_ARRAY_BUFFER, m_bufUV);
    }
    return m_UVGenerated;
}

bool Drawable::bindPos()
{
    if(m_posGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufPos);
    }
    return m_posGenerated;
}

bool Drawable::bindNor()
{
    if(m_norGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufNor);
    }
    return m_norGenerated;
}

bool Drawable::bindCol()
{
    if(m_colGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);
    }
    return m_colGenerated;
}

InstancedDrawable::InstancedDrawable(OpenGLContext *context)
    : Drawable(context), m_numInstances(0), m_bufPosOffset(-1), m_offsetGenerated(false)
{}

InstancedDrawable::~InstancedDrawable(){}

int InstancedDrawable::instanceCount() const {
    return m_numInstances;
}

void InstancedDrawable::generateOffsetBuf() {
    m_offsetGenerated = true;
    mp_context->glGenBuffers(1, &m_bufPosOffset);
}

bool InstancedDrawable::bindOffsetBuf() {
    if(m_offsetGenerated){
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufPosOffset);
    }
    return m_offsetGenerated;
}


void InstancedDrawable::clearOffsetBuf() {
    if(m_offsetGenerated) {
        mp_context->glDeleteBuffers(1, &m_bufPosOffset);
        m_offsetGenerated = false;
    }
}
void InstancedDrawable::clearColorBuf() {
    if(m_colGenerated) {
        mp_context->glDeleteBuffers(1, &m_bufCol);
        m_colGenerated = false;
    }
}
