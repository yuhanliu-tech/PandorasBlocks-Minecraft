#include "mygl.h"
#include <glm_includes.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <QDebug>

static const int shadowmult = 4;

MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      m_worldAxes(this),
      m_progLambert(this), m_progInstanced(this), m_progOverlay(this), m_progDistance(this), m_progHeight(this), m_progSky(this),
      m_frameBuffer(this, width(), height(), devicePixelRatio()), m_quad(this),  //m_frameDistanceBuffer(this, width(), height(), devicePixelRatio()),
      m_frameRawDistanceBuffer(this, width(), height(), devicePixelRatio()), m_skyBuffer(this, width(), height(), devicePixelRatio()),
      m_heightMapBuffer(this, width() * shadowmult, height() * shadowmult, devicePixelRatio(), true),
      m_terrain(this), m_player(glm::vec3(0.f, 140.f, 0.f), m_terrain), m_prevPos(m_player.getPosition()),
      m_prevTime(QDateTime::currentMSecsSinceEpoch()), trackMouse(true),
      mp_textureFile(this), mp_normalFile(this), time(0.f),
      m_renderedTexture(-1), m_renderedNormals(-1), m_timeOfDay(1)

{
    // changed player Pos from 48, 129, 48!!! ^
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
    // Tell the timer to redraw 60 times per second
    m_timer.start(16);
    m_timer.setInterval(5);
    timeAtInit = QDateTime::currentMSecsSinceEpoch();
    m_timeOfLastMovement = timeAtInit;

    setFocusPolicy(Qt::ClickFocus);


    //In opening
    m_lightDir = glm::normalize(glm::vec4(1.0f, 0.7f, 0.f, 0.f)); //sets it as constant
    m_moonDir = glm::normalize(glm::vec4(1.0f, 0.7f, 0.f, 0.f)); //sets it as constant
    float near_plane = m_player.getCamera().getNearClip();
    float far_plane = m_player.getCamera().getFarClip();
    m_lightProj = glm::ortho(-224.f, 224.f, -224.f, 224.f, -400.f, 400.f);
    //calcLightSpaceMatrix();

    setMouseTracking(true); // MyGL will track the mouse's movements even if a mouse button is not pressed
    setCursor(Qt::BlankCursor); // Make the cursor invisible
    moveMouseToCenter(); // needed?
}

void MyGL::moveLightDir(float time) {
    time = time * 0.001f;

    //std::cout << sin(time) << std::endl;
    float PI = 3.14159265359;

    m_lightDir = glm::rotate(glm::mat4(), time, glm::vec3(0, 0, 1)) * m_lightDir;
    m_moonDir = glm::rotate(glm::mat4(), time + PI, glm::vec3(0, 0, 1)) * m_lightDir;


    //glm::rotate(vector normal to axis of rotation, angle as a function of time, axis that you want to rotate about )
}



MyGL::~MyGL() {
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    m_frameBuffer.destroy();

    m_quad.destroyVBOdata();

    //m_frameDistanceBuffer.destroy();
    m_frameRawDistanceBuffer.destroy();
    m_skyBuffer.destroy();
    m_heightMapBuffer.destroy();

}


void MyGL::moveMouseToCenter() {
    if(trackMouse) {
        QCursor::setPos(this->mapToGlobal(QPoint(width() / 2.f, height() / 2.f)));
        m_inputs.mouseX = width() * 0.5f;
        m_inputs.mouseY = height() * 0.5f;
    }
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_CULL_FACE);

    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    //For Transparent Blocks
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    // Set the size with which points should be rendered
    glPointSize(5);

    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.37f, 0.74f, 1.0f, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    //Create the instance of the world axes
    m_worldAxes.createVBOdata();

    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    ////m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    m_progHeight.create(":/glsl/height.vert.glsl", ":/glsl/height.frag.glsl");

    // procedural sky
    m_progSky.create(":/glsl/sky.vert.glsl", ":/glsl/sky.frag.glsl");

    m_progOverlay.create(":/glsl/overlay.vert.glsl", ":/glsl/overlay.frag.glsl");
    m_progDistance.create(":/glsl/distance.vert.glsl", ":/glsl/distance.frag.glsl");

    // Set a color with which to draw geometry.
    // This will ultimately not be used when you change
    // your program to render Chunks with vertex colors
    // and UV coordinates
    m_progLambert.setGeometryColor(glm::vec4(1,0,0,1));

    m_progSky.setModelMatrix(glm::mat4(1));
    m_progSky.setViewProjMatrix(glm::inverse(m_player.mcr_camera.getViewProj()));
    //In initializeGL
    m_progLambert.setLightSpaceMatrix(calcLightSpaceMatrix());
    m_progLambert.setLightDir(m_lightDir);

    m_progSky.setLightDir(m_lightDir);

    m_progOverlay.setModelMatrix(glm::mat4(1));
    m_progOverlay.setViewProjMatrix(glm::mat4(1));

    createTextures();
    printGLErrorLog();

    createNormals();
    printGLErrorLog();


    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);
    printGLErrorLog();

    qDebug() << "successfully initialized";

    printGLErrorLog();
    m_terrain.createInitialChunks(glm::vec2(m_player.mcr_position.x, m_player.mcr_position.z));
    printGLErrorLog();


    m_frameBuffer.create();
    m_frameBuffer.bindToTextureSlot(2);

    //    m_frameDistanceBuffer.create();
    //    m_frameDistanceBuffer.bindToTextureSlot(3);

    m_frameRawDistanceBuffer.create();
    m_frameRawDistanceBuffer.bindToTextureSlot(4);

    m_heightMapBuffer.create();
    m_heightMapBuffer.bindToTextureSlot(5);

    m_skyBuffer.create();
    m_skyBuffer.bindToTextureSlot(6);



    m_quad.createVBOdata();
    printGLErrorLog();
}

void MyGL::resizeGL(int w, int h) {
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_player.setCameraWiDThHeight(static_cast<unsigned int>(w), static_cast<unsigned int>(h));
    glm::mat4 viewproj = m_player.mcr_camera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    //m_progFlat.setViewProjMatrix(viewproj);
    m_progSky.setViewProjMatrix(glm::inverse(viewproj));
    m_progDistance.setViewProjMatrix(viewproj);
    //m_progFlat.setViewProjMatrix(viewproj); //maybe change
    m_progHeight.setViewProjMatrix(viewproj); //Maybe get this

    m_progLambert.setScreenDimensions(glm::vec2(w, h));
    //m_progFlat.setScreenDimensions(glm::vec2(w, h));
    m_progDistance.setScreenDimensions(glm::vec2(w, h));
    m_progOverlay.setScreenDimensions(glm::vec2(w, h));
    m_progHeight.setScreenDimensions(glm::vec2(w, h));


    m_frameBuffer.resize(w, h, 1.f);
    m_frameBuffer.destroy();
    m_frameBuffer.create();

    std::cout<< w << ", " << h << std::endl;

    //    m_frameDistanceBuffer.resize(w, h, 1.f);
    //    m_frameDistanceBuffer.destroy();
    //    m_frameDistanceBuffer.create();

    m_skyBuffer.resize(w, h, 1.f);
    m_skyBuffer.destroy();
    m_skyBuffer.create();

    // procedural sky
    m_progSky.useMe();
    this->glUniform2i(m_progSky.unifDimensions, width(), height());
    this->glUniform3f(m_progSky.unifEye,  m_player.mcr_camera.mcr_position.x,
                      m_player.mcr_camera.mcr_position.y,
                      m_player.mcr_camera.mcr_position.z);
    m_frameRawDistanceBuffer.resize(w, h, 1.f);
    m_frameRawDistanceBuffer.destroy();
    m_frameRawDistanceBuffer.create();

    m_heightMapBuffer.resize(w * shadowmult, h * shadowmult, 1.f);
    m_heightMapBuffer.destroy();
    m_heightMapBuffer.create();

    printGLErrorLog();
}


// MyGL's constructor links tick() to a timer that fires 60 times per second.
// We're treating MyGL as our game engine class, so we're going to perform
// all per-frame actions here, such as performing physics updates on all
// entities in the scene.
void MyGL::tick() {
    //    printGLErrorLog();
    qint64 currTime = QDateTime::currentMSecsSinceEpoch();
    moveLightDir((currTime - timeAtInit)/10000.f);
    m_progLambert.setTime((currTime - timeAtInit)/10000.f);
    m_progDistance.setTime((currTime - timeAtInit)/10000.f);
    m_progHeight.setTime((currTime - timeAtInit)/10000.f);
    m_progOverlay.setTime((currTime - timeAtInit)/10000.f);
    m_progLambert.setTimeOfDay(m_timeOfDay);

    //    printGLErrorLog();


    glm::vec3 prevPos = m_player.getPosition();
    glm::vec3 prevLook = m_player.getCamera().getForward();

    this->m_player.tick(currTime - m_prevTime, this->m_inputs);

    if(prevPos != m_player.getPosition() || prevLook != m_player.getCamera().getForward()){
        m_timeOfLastMovement = currTime;
    }

    m_progDistance.setIdleTime((currTime - m_timeOfLastMovement)/10000.f);


    //    qDebug() << m_player.lookAsQString() << "\nsd";
    m_progLambert.setCameraPos(m_player.getCameraPos());
    m_progSky.setCameraPos(m_player.getCameraPos());
    m_progDistance.setNearestBlockDist(m_player.getNearestBlockDist(&m_terrain));
    m_progDistance.setCameraPos(m_player.getCameraPos());
    m_progHeight.setCameraPos(m_player.getCameraPos()); //Maybe change later ?

    m_progDistance.setCameraLook(glm::vec4(m_player.getCamera().getForward(), 1.f));

    moveMouseToCenter();

    glm::vec3 currPlayerPos = m_player.getPosition();

    m_terrain.multithreadedWork(currPlayerPos, m_prevPos, currTime - m_prevTime);
    //    printGLErrorLog();
    m_prevPos = currPlayerPos;
    m_prevTime = currTime;

    update(); // Calls paintGL() as part of a larger QOpenGLWidget pipeline
    //    printGLErrorLog();
    sendPlayerDataToGUI(); // Updates the info in the secondary window displaying player data
    //    printGLErrorLog();
}

void MyGL::sendPlayerDataToGUI() const {
    emit sig_sendPlayerPos(m_player.posAsQString());
    emit sig_sendPlayerVel(m_player.velAsQString());
    emit sig_sendPlayerAcc(m_player.accAsQString());
    emit sig_sendPlayerLook(m_player.lookAsQString());
    glm::vec2 pPos(m_player.mcr_position.x, m_player.mcr_position.z);
    glm::ivec2 chunk(16 * glm::ivec2(glm::floor(pPos / 16.f)));
    glm::ivec2 zone(64 * glm::ivec2(glm::floor(pPos / 64.f)));
    emit sig_sendPlayerChunk(QString::fromStdString("( " + std::to_string(chunk.x) + ", " + std::to_string(chunk.y) + " )"));
    emit sig_sendPlayerTerrainZone(QString::fromStdString("( " + std::to_string(zone.x) + ", " + std::to_string(zone.y) + " )"));
}

// This function is called whenever update() is called.
// MyGL's constructor links update() to a timer that fires 60 times per second,
// so paintGL() called at a rate of 60 frames per second.
void MyGL::paintGL() {

    //m_progFlat.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_progDistance.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_progLambert.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_progLambert.setLightSpaceMatrix(calcLightSpaceMatrix());
    m_progInstanced.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_progHeight.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_progSky.setViewProjMatrix(glm::inverse(m_player.mcr_camera.getViewProj()));

    m_progSky.setLightDir(m_lightDir);
    m_progSky.setMoonDir(m_moonDir);

    m_progHeight.setLightSpaceMatrix(calcLightSpaceMatrix());
    m_progLambert.setLightSpaceMatrix(calcLightSpaceMatrix());


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // procedural sky (sample unifs)------------------------

    m_progSky.setTimeOfDay(0);//Telling the sky to not render stars nor sun
    m_skyBuffer.bindToTextureSlot(6); //<<<<This line ? Why is it necessray
    m_skyBuffer.bindFrameBuffer(); //Bind the first frame buffer
    glViewport(0, 0, width(), height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear whatever was on the frame buffer before

    m_progSky.useMe();
    this->glUniform3f(m_progSky.unifEye,  m_player.mcr_camera.mcr_position.x,
                      m_player.mcr_camera.mcr_position.y,
                      m_player.mcr_camera.mcr_position.z);
    this->glUniform1f(m_progSky.unifTime, time++);

    glDepthFunc(GL_LEQUAL);
    m_progSky.draw(m_quad); // draw the quad with sky

    glDepthFunc(GL_LESS);

    //HEIGHT PASS -----------------------------------------------------------

    m_heightMapBuffer.bindToTextureSlot(5); //Sets the active texture slot to 5
    m_heightMapBuffer.bindFrameBuffer(); //Bind the first frame buffer
    glViewport(0, 0, m_heightMapBuffer.getWidth(), m_heightMapBuffer.getHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear whatever was on the frame buffer before

    glCullFace(GL_FRONT); //CULLING front triangles for shadow mapping

    renderTerrain(4); //Render the terrain (4 for "height")
    printGLErrorLog();


    glCullFace(GL_BACK); //CULLING BACK triangles

    //RAW DISTANCE PASS -----------------------------------------------------------

    m_frameRawDistanceBuffer.bindToTextureSlot(4); //<<<<This line ? Why is it necessray
    m_frameRawDistanceBuffer.bindFrameBuffer(); //Bind the first frame buffer
    glViewport(0, 0, width(), height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear whatever was on the frame buffer before

    renderTerrain(3); //Render the terrain (3 for "raw distance")
    printGLErrorLog();


    //    //DISTANCE PASS -----------------------------------------------------------

    //    m_frameDistanceBuffer.bindToTextureSlot(3); //<<<<This line ? Why is it necessray
    //    m_frameDistanceBuffer.bindFrameBuffer(); //Bind the first frame buffer
    //    glViewport(0, 0, width(), height());
    //    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Clear whatever was on the frame buffer before

    //    renderTerrain(2); //Render the terrain (2 for "distance")
    //    printGLErrorLog();

    //1) Bind to a proper texture slot on initialization, after create() is called
    //2) In paint: bind framebuffer (.bindFrameBuffer());
    //Bind the texture
    // Clear the frame buffer
    // Make render call with the proper shader

    //FINAL/OVERLAY PASS -----------------------------------------------------------

    m_frameBuffer.bindToTextureSlot(2); //<<<<This line ? Why is it necessray
    m_frameBuffer.bindFrameBuffer(); //Binding second frame buffer
    glViewport(0, 0, width(), height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDepthFunc(GL_LEQUAL);
    m_progSky.setTimeOfDay(1);//Telling the sky to render stars and sun
    m_progSky.draw(m_quad); // draw the quad with sky

    glDepthFunc(GL_LESS);
    renderTerrain(1); //1 for lambert, 2 for flat; render on lambert
    //printGLErrorLog();

    //Binding the active frame buffer to the final screen.
    glBindFramebuffer(GL_FRAMEBUFFER, this->defaultFramebufferObject());
    glViewport(0,0,width() * devicePixelRatio(), height() * devicePixelRatio());
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_player.checkSubmerged() && m_player.swimmingIn == LAVA) {
        m_progOverlay.overlayLiquidType(2);
        //printGLErrorLog();
    } else if (m_player.checkSubmerged() && m_player.swimmingIn == WATER) {
        m_progOverlay.overlayLiquidType(1);
        //printGLErrorLog();
    }else {
        m_progOverlay.overlayLiquidType(0);
        //printGLErrorLog();
    }

    glDepthFunc(GL_LEQUAL);
    m_progOverlay.drawOverlay(m_quad);
    printGLErrorLog();

}

// TODO: Change this so it renders the nine zones of generated
// terrain that surround the player (refer to Terrain::m_generateDTerrain
// for more info)
void MyGL::renderTerrain(int programType) {

    int radiusInZoneSc = static_cast<int>(TERRAIN_DRAW_RADIUS) * 64;
    glm::vec3 pos = this->m_player.getPosition();
    pos = 16.f * glm::floor(pos / 16.f);

    //  m_terrain.draw(-264, 264, -264, 264, &m_progLambert);
    // check render terrain?

    ShaderProgram* prog;



    if(programType == 1) { //Lambert }
        prog = &m_progLambert;
    } else if(programType == 2){
        //prog = &//m_progFlat;
    } else if(programType == 3) {
        prog = &m_progDistance;
    } else if(programType == 4) {
        prog = &m_progHeight;
    } else {
        throw std::runtime_error("cringe");
    }

    prog->setShadowTexture(5);

    m_terrain.draw(-radiusInZoneSc + pos.x, radiusInZoneSc + pos.x,
                   -radiusInZoneSc + pos.z, radiusInZoneSc + pos.z, prog);//changed from Lambert

}


//New function
glm::mat4 MyGL::calcLightSpaceMatrix(){//helper function to calculate the light vec
    glm::vec3 playerxz = glm::vec3(m_player.getCameraPos());
    playerxz.y = 100.f;
    glm::mat4 lightView = glm::lookAt(glm::vec3(m_lightDir) + glm::vec3(playerxz), playerxz, glm::vec3(0.0f, 1.0f, 0.0f)); //not sure if i should use lightpos here
    glm::mat4 moonView = glm::lookAt(glm::vec3(m_moonDir) + glm::vec3(playerxz), playerxz, glm::vec3(0.0f, 1.0f, 0.0f)); //not sure if i should use lightpos here

    float sunGroundAngle = glm::dot(glm::normalize(m_lightDir), glm::vec4(0, 1, 0, 0));
    if(sunGroundAngle < -0.3) {
        lightView = moonView;
        m_timeOfDay = 1.f;
    } else if(sunGroundAngle > 0.3) {
        m_timeOfDay = 0.f; // SET TO DAY
    } else {
        m_timeOfDay = 2.f; //set to night with no moon to avoid weirdo lighting effects
    }

    return m_lightProj * lightView;


};


void MyGL::keyPressEvent(QKeyEvent *e) {
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used, but I really dislike their
    // syntax so I chose to be lazy and use a long
    // chain of if statements instead
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        m_player.rotateOnUpGlobal(-amount);
    } else if (e->key() == Qt::Key_Left) {
        m_player.rotateOnUpGlobal(amount);
    } else if (e->key() == Qt::Key_Up) {
        m_player.rotateOnRightLocal(-amount);
    } else if (e->key() == Qt::Key_Down) {
        m_player.rotateOnRightLocal(amount);
    } else if (e->key() == Qt::Key_W) {
        this->m_inputs.wPressed = true;
    } else if (e->key() == Qt::Key_S) {
        this->m_inputs.sPressed = true;
    } else if (e->key() == Qt::Key_D) {
        this->m_inputs.dPressed = true;
    } else if (e->key() == Qt::Key_A) {
        this->m_inputs.aPressed = true;
    } else if (e->key() == Qt::Key_Q) {
        this->m_inputs.qPressed = true;
    } else if (e->key() == Qt::Key_E) {
        this->m_inputs.ePressed = true;
    } else if (e->key() == Qt::Key_Space) {
        this->m_inputs.spacePressed = true;
        m_player.toggleGrounded();
    } else if (e->key() == Qt::Key_F) {
        m_player.toggleFlight();
    } else if (e->key() == Qt::Key_Tab) {
        toggleMouseTrack();
    } else if (e->key() == Qt::Key_O) {
        this->m_inputs.oPressed = true;
    }

}
void MyGL::keyReleaseEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
    } else if (e->key() == Qt::Key_Left) {
    } else if (e->key() == Qt::Key_Up) {
    } else if (e->key() == Qt::Key_Down) {
    } else if (e->key() == Qt::Key_W) {
        this->m_inputs.wPressed = false;
    } else if (e->key() == Qt::Key_S) {
        this->m_inputs.sPressed = false;
    } else if (e->key() == Qt::Key_D) {
        this->m_inputs.dPressed = false;
    } else if (e->key() == Qt::Key_A) {
        this->m_inputs.aPressed = false;
    } else if (e->key() == Qt::Key_Q) {
        this->m_inputs.qPressed = false;
    }  else if (e->key() == Qt::Key_Space) {
        this->m_inputs.spacePressed = false;
    } else if (e->key() == Qt::Key_E) {
        this->m_inputs.ePressed = false;
    } else if (e->key() == Qt::Key_O) {
        this->m_inputs.oPressed = false;
    }

}
void MyGL::mouseMoveEvent(QMouseEvent *e) {
    if(trackMouse) {
        m_inputs.mouseX = e->pos().x();
        m_inputs.mouseY = e->pos().y();
    }
}
void MyGL::mousePressEvent(QMouseEvent *e) {
    if (e->buttons() == Qt::LeftButton) {
        this->m_player.removeBlock(&m_terrain);
    } else if (e->buttons() == Qt::RightButton) {
        this->m_player.addBlock(&m_terrain);
    }
    this->update();
}
void MyGL::toggleMouseTrack() {
    trackMouse = !trackMouse;
    setMouseTracking(trackMouse);
}

void MyGL::createTextures() {

    mp_textureFile.create(":/textures/minecraft_textures_all.png");
    printGLErrorLog();
    // If this vector of textures were to be altered
    // while the program was running, storing a pointer
    // to an element within it would be bad news, since
    // the elements inside would not be guaranteed to be
    // in the same memory as before.
    // However, this vector's contents are only ever modified
    // in this function, which is called exactly once in the
    // program's lifetime.

    printGLErrorLog();
    mp_textureFile.load(0);
    printGLErrorLog();
};

void MyGL::createNormals() {

    mp_normalFile.create(":/textures/minecraft_normals_all.png");
    printGLErrorLog();
    // If this vector of textures were to be altered
    // while the program was running, storing a pointer
    // to an element within it would be bad news, since
    // the elements inside would not be guaranteed to be
    // in the same memory as before.
    // However, this vector's contents are only ever modified
    // in this function, which is called exactly once in the
    // program's lifetime.

    mp_normalFile.load(1);
    printGLErrorLog();
};
