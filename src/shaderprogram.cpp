#include "shaderprogram.h"
#include <QFile>
#include <QStringBuilder>
#include <QTextStream>
#include <QDebug>
#include <stdexcept>
#include <iostream>


ShaderProgram::ShaderProgram(OpenGLContext *context)
    : vertShader(), fragShader(), prog(),
      attrPos(-1), attrNor(-1), attrCol(-1), attrBlockSFX(-1), attrUV(-1),
      unifModel(-1), unifModelInvTr(-1), unifViewProj(-1), unifColor(-1), unifTextureSample(-1), unifNormalMap(-1), unifTime(-1), unifIdleTime(-1),
      unifCameraPos(-1), unifCameraLook(-1), unifScreenDimensions(-1), unifNearestBlockDist(-1),
      context(context), unif_liquidType(-1), unif_sceneTexture(-1), unif_rawDistanceTexture(-1),
      unif_heightMapTexture(-1), unif_lightSpaceMatrix(-1), unif_lightDir(-1), unif_skyTexture(-1), unif_timeOfDay(-1), unif_moonDir(-1), attrBlockDepths(-1)
{}

void ShaderProgram::create(const char *vertfile, const char *fragfile)
{
    // Allocate space on our GPU for a vertex shader and a fragment shader and a shader program to manage the two
    vertShader = context->glCreateShader(GL_VERTEX_SHADER);
    fragShader = context->glCreateShader(GL_FRAGMENT_SHADER);
    prog = context->glCreateProgram();
    // Get the body of text stored in our two .glsl files
    QString qVertSource = qTextFileRead(vertfile);
    QString qFragSource = qTextFileRead(fragfile);

    char* vertSource = new char[qVertSource.size()+1];
    strcpy(vertSource, qVertSource.toStdString().c_str());
    char* fragSource = new char[qFragSource.size()+1];
    strcpy(fragSource, qFragSource.toStdString().c_str());


    // Send the shader text to OpenGL and store it in the shaders specified by the handles vertShader and fragShader
    context->glShaderSource(vertShader, 1, (const char**)&vertSource, 0);
    context->glShaderSource(fragShader, 1, (const char**)&fragSource, 0);
    // Tell OpenGL to compile the shader text stored above
    context->glCompileShader(vertShader);
    context->glCompileShader(fragShader);
    // Check if everything compiled OK
    GLint compiled;
    context->glGetShaderiv(vertShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        printShaderInfoLog(vertShader);
    }
    context->glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        printShaderInfoLog(fragShader);
    }

    // Tell prog that it manages these particular vertex and fragment shaders
    context->glAttachShader(prog, vertShader);
    context->glAttachShader(prog, fragShader);
    context->glLinkProgram(prog);

    // Check for linking success
    GLint linked;
    context->glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    if (!linked) {
        printLinkInfoLog(prog);
    }

    // Get the handles to the variables stored in our shaders
    // See shaderprogram.h for more information about these variables

    attrPos = context->glGetAttribLocation(prog, "vs_Pos");
    attrNor = context->glGetAttribLocation(prog, "vs_Nor");
    attrCol = context->glGetAttribLocation(prog, "vs_Col");
    attrUV = context->glGetAttribLocation(prog, "vs_UV");
    attrBlockSFX = context -> glGetAttribLocation(prog, "vs_BlockSFX");
    attrBlockDepths = context -> glGetAttribLocation(prog, "vs_BlockDepth");

    if(attrCol == -1) attrCol = context->glGetAttribLocation(prog, "vs_ColInstanced");
    attrPosOffset = context->glGetAttribLocation(prog, "vs_OffsetInstanced");
    //Eventually add this

    unifModel      = context->glGetUniformLocation(prog, "u_Model");
    unifModelInvTr = context->glGetUniformLocation(prog, "u_ModelInvTr");
    unifViewProj   = context->glGetUniformLocation(prog, "u_ViewProj");
    unifColor      = context->glGetUniformLocation(prog, "u_Color");

    unifTime      = context->glGetUniformLocation(prog, "u_Time");
    unifIdleTime      = context->glGetUniformLocation(prog, "u_idleTime");
    unifNearestBlockDist      = context->glGetUniformLocation(prog, "u_nearestBlockDist");
    unif_liquidType = context -> glGetUniformLocation(prog, "u_liquidType");
    unif_sceneTexture = context -> glGetUniformLocation(prog, "u_sceneTexture");
    //unif_distanceTexture = context -> glGetUniformLocation(prog, "u_distanceTexture");
    unif_rawDistanceTexture = context -> glGetUniformLocation(prog, "u_rawDistanceTexture");
    unif_heightMapTexture = context -> glGetUniformLocation(prog, "u_heightMapTexture");

    unifTextureSample = context->glGetUniformLocation(prog, "u_Texture");
    unifNormalMap = context->glGetUniformLocation(prog, "u_NormalMap");
    unifCameraPos = context->glGetUniformLocation(prog, "u_CameraPos");

    // procedural sky
    unifDimensions = context->glGetUniformLocation(prog, "u_Dimensions");
    unifEye = context->glGetUniformLocation(prog, "u_Eye");
    unif_skyTexture  = context -> glGetUniformLocation(prog, "u_skyTexture");

    unifCameraLook = context->glGetUniformLocation(prog, "u_CameraLook");
    unifScreenDimensions = context->glGetUniformLocation(prog, "u_ScreenDimensions");

    unif_lightSpaceMatrix = context -> glGetUniformLocation(prog, "u_lightSpaceMatrix");
    unif_lightDir = context -> glGetUniformLocation(prog, "u_lightDir");
    unif_moonDir = context -> glGetUniformLocation(prog, "u_moonDir");

    unif_timeOfDay = context -> glGetUniformLocation(prog, "u_timeOfDay");
}

void ShaderProgram::useMe()
{
    context->glUseProgram(prog);
}

void ShaderProgram::overlayLiquidType(const int type)
{
    useMe();

    if (unif_liquidType != -1)
    {
        context->glUniform1i(unif_liquidType, type);
    }
}

void ShaderProgram::setModelMatrix(const glm::mat4 &model)
{
    useMe();

    if (unifModel != -1) {
        // Pass a 4x4 matrix into a uniform variable in our shader
        // Handle to the matrix variable on the GPU
        context->glUniformMatrix4fv(unifModel,
                                    // How many matrices to pass
                                    1,
                                    // Transpose the matrix? OpenGL uses column-major, so no.
                                    GL_FALSE,
                                    // Pointer to the first element of the matrix
                                    &model[0][0]);
    }

    if (unifModelInvTr != -1) {
        glm::mat4 modelinvtr = glm::inverse(glm::transpose(model));
        // Pass a 4x4 matrix into a uniform variable in our shader
        // Handle to the matrix variable on the GPU
        context->glUniformMatrix4fv(unifModelInvTr,
                                    // How many matrices to pass
                                    1,
                                    // Transpose the matrix? OpenGL uses column-major, so no.
                                    GL_FALSE,
                                    // Pointer to the first element of the matrix
                                    &modelinvtr[0][0]);
    }
}

void ShaderProgram::setViewProjMatrix(const glm::mat4 &vp)
{
    // Tell OpenGL to use this shader program for subsequent function calls
    useMe();

    if(unifViewProj != -1) {
        // Pass a 4x4 matrix into a uniform variable in our shader
        // Handle to the matrix variable on the GPU
        context->glUniformMatrix4fv(unifViewProj,
                                    // How many matrices to pass
                                    1,
                                    // Transpose the matrix? OpenGL uses column-major, so no.
                                    GL_FALSE,
                                    // Pointer to the first element of the matrix
                                    &vp[0][0]);
    }
}

void ShaderProgram::setGeometryColor(glm::vec4 color)
{
    useMe();

    if(unifColor != -1)
    {
        context->glUniform4fv(unifColor, 1, &color[0]);
    }
}


void ShaderProgram::setTimeOfDay(float t){
    useMe();
    if(unif_timeOfDay != -1)
    {
        context->glUniform1f(unif_timeOfDay, t);
    }
};


void ShaderProgram::drawInterleavedOpaque(Drawable &c) { // second drawing function that renders drawable with interleaved VBOs

    useMe();
    context->printGLErrorLog();

//    if (unif_sceneTexture != -1) {
//        context -> glUniform1i(unif_sceneTexture, 0);
//    }
    if (unifTextureSample = -1) {
        context->glUniform1i(unifTextureSample, 0);
    }

    if(c.elemCountOpaque() < 0) { //making sure that the drawable is non-empty
        throw std::out_of_range("Attempting to draw a drawable with m_count of " + std::to_string(c.elemCountOpaque()) + "!");
    }

//    context->printGLErrorLog();

    if (c.bindInterleavedOpaque() && c.bindOpaqueSFX()) {

        if (attrPos != -1) {

            context->glEnableVertexAttribArray(attrPos);
            context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, 5 * sizeof(glm::vec4), (void*)0);
        }

        if (attrNor != -1) {

            context->glEnableVertexAttribArray(attrNor);
            context->glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, 5 * sizeof(glm::vec4), (void*)(sizeof(glm::vec4)));
        }

        if (attrCol != -1) {

            context->glEnableVertexAttribArray(attrCol);
            context->glVertexAttribPointer(attrCol, 4, GL_FLOAT, false, 5 * sizeof(glm::vec4), (void*)(2*sizeof(glm::vec4)));
        }

        if (attrBlockSFX != -1) {

            context->glEnableVertexAttribArray(attrBlockSFX);
            context->glVertexAttribPointer(attrBlockSFX, 4, GL_FLOAT, false, 5 * sizeof(glm::vec4), (void*)(3*sizeof(glm::vec4)));
        }

        if (attrBlockDepths != -1) {

            context->glEnableVertexAttribArray(attrBlockDepths);
            context->glVertexAttribPointer(attrBlockDepths, 4, GL_FLOAT, false, 5 * sizeof(glm::vec4), (void*)(4*sizeof(glm::vec4)));
        }

    }

    c.bindOpaqueIdx();
    context -> glDrawElements(c.drawMode(), c.elemCountOpaque(), GL_UNSIGNED_INT,0);

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
    if (attrNor != -1) context->glDisableVertexAttribArray(attrNor);
    if (attrCol != -1) context->glDisableVertexAttribArray(attrCol);
    if (attrBlockSFX != -1) context->glDisableVertexAttribArray(attrBlockSFX);
    if (attrBlockDepths != -1) context->glDisableVertexAttribArray(attrBlockDepths);

//    context->printGLErrorLog();
}


void ShaderProgram::drawInterleaveDTransparent(Drawable &c) { // second drawing function that renders drawable with interleaved VBOs

    useMe();

//    if (unif_sceneTexture != -1) {
//        context -> glUniform1i(unif_sceneTexture, 0);
//    }

    if (unifTextureSample = -1) {
        context->glUniform1i(unifTextureSample, 0);
    }

    //    std::cout << c.elemCountTransparent() << std::endl;
    if(c.elemCountTransparent() < 0) { //making sure that the drawable is non-empty
        throw std::out_of_range("Attempting to draw a drawable with m_count of " + std::to_string(c.elemCountTransparent()) + "!");
    }


    if (c.bindInterleaveDTransparent() && c.binDTransparentSFX()) {

        if (attrPos != -1) {

            context->glEnableVertexAttribArray(attrPos);
            context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, 5 * sizeof(glm::vec4), (void*)0);
        }

        if (attrNor != -1) {

            context->glEnableVertexAttribArray(attrNor);
            context->glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, 5 * sizeof(glm::vec4), (void*)(sizeof(glm::vec4)));
        }

        if (attrCol != -1) {

            context->glEnableVertexAttribArray(attrCol);
            context->glVertexAttribPointer(attrCol, 4, GL_FLOAT, false, 5 * sizeof(glm::vec4), (void*)(2*sizeof(glm::vec4)));
        }

        if (attrBlockSFX != -1) {

            context->glEnableVertexAttribArray(attrBlockSFX);
            context->glVertexAttribPointer(attrBlockSFX, 4, GL_FLOAT, false, 5 * sizeof(glm::vec4), (void*)(3*sizeof(glm::vec4)));
        }

        if (attrBlockDepths != -1) {

            context->glEnableVertexAttribArray(attrBlockDepths);
            context->glVertexAttribPointer(attrBlockDepths, 4, GL_FLOAT, false, 5 * sizeof(glm::vec4), (void*)(4*sizeof(glm::vec4)));
        }


    }

    c.binDTransparentIdx();
    context -> glDrawElements(c.drawMode(), c.elemCountTransparent(), GL_UNSIGNED_INT,0);

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
    if (attrNor != -1) context->glDisableVertexAttribArray(attrNor);
    if (attrCol != -1) context->glDisableVertexAttribArray(attrCol);
    if (attrBlockSFX != -1) context->glDisableVertexAttribArray(attrBlockSFX);
     if (attrBlockDepths != -1) context->glDisableVertexAttribArray(attrBlockDepths);


    context->printGLErrorLog();
}


//This function, as its name implies, uses the passed in GL widget
void ShaderProgram::draw(Drawable &d)
{
    useMe();

    if(d.elemCountOpaque() < 0) {
        throw std::out_of_range("Attempting to draw a drawable with m_count of " + std::to_string(d.elemCountOpaque()) + "!");
    }

    // Each of the following blocks checks that:
    //   * This shader has this attribute, and
    //   * This Drawable has a vertex buffer for this attribute.
    // If so, it binds the appropriate buffers to each attribute.

    // Remember, by calling bindPos(), we call
    // glBindBuffer on the Drawable's VBO for vertex position,
    // meaning that glVertexAttribPointer associates vs_Pos
    // (referred to by attrPos) with that VBO
    if (attrPos != -1 && d.bindPos()) {
        context->glEnableVertexAttribArray(attrPos);
        context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, 0, NULL);
    }

    if (attrNor != -1 && d.bindNor()) {
        context->glEnableVertexAttribArray(attrNor);
        context->glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, 0, NULL);
    }

    if (attrCol != -1 && d.bindCol()) {
        context->glEnableVertexAttribArray(attrCol);
        context->glVertexAttribPointer(attrCol, 4, GL_FLOAT, false, 0, NULL);
    }


    // Bind the index buffer and then draw shapes from it.
    // This invokes the shader program, which accesses the vertex buffers.
    d.bindOpaqueIdx();
    context->glDrawElements(d.drawMode(), d.elemCountOpaque(), GL_UNSIGNED_INT, 0);

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
    if (attrNor != -1) context->glDisableVertexAttribArray(attrNor);
    if (attrCol != -1) context->glDisableVertexAttribArray(attrCol);
    if (attrBlockSFX != -1) context->glDisableVertexAttribArray(attrBlockSFX);
    if (attrBlockDepths != -1) context->glDisableVertexAttribArray(attrBlockDepths);

    context->printGLErrorLog();
}



void ShaderProgram::drawInstanced(InstancedDrawable &d)
{
    useMe();

    if(d.elemCountOpaque() < 0) {
        throw std::out_of_range("Attempting to draw a drawable with m_count of " + std::to_string(d.elemCountOpaque()) + "!");
    }

    // Each of the following blocks checks that:
    //   * This shader has this attribute, and
    //   * This Drawable has a vertex buffer for this attribute.
    // If so, it binds the appropriate buffers to each attribute.

    // Remember, by calling bindPos(), we call
    // glBindBuffer on the Drawable's VBO for vertex position,
    // meaning that glVertexAttribPointer associates vs_Pos
    // (referred to by attrPos) with that VBO
    if (attrPos != -1 && d.bindPos()) {
        context->glEnableVertexAttribArray(attrPos);
        context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, 0, NULL);
        context->glVertexAttribDivisor(attrPos, 0);
    }

    if (attrNor != -1 && d.bindNor()) {
        context->glEnableVertexAttribArray(attrNor);
        context->glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, 0, NULL);
        context->glVertexAttribDivisor(attrNor, 0);
    }

    if (attrCol != -1 && d.bindCol()) {
        context->glEnableVertexAttribArray(attrCol);
        context->glVertexAttribPointer(attrCol, 3, GL_FLOAT, false, 0, NULL);
        context->glVertexAttribDivisor(attrCol, 1);
    }

    if (attrPosOffset != -1 && d.bindOffsetBuf()) {
        context->glEnableVertexAttribArray(attrPosOffset);
        context->glVertexAttribPointer(attrPosOffset, 3, GL_FLOAT, false, 0, NULL);
        context->glVertexAttribDivisor(attrPosOffset, 1);
    }

    // Bind the index buffer and then draw shapes from it.
    // This invokes the shader program, which accesses the vertex buffers.
    d.bindOpaqueIdx();
    context->glDrawElementsInstanced(d.drawMode(), d.elemCountOpaque(), GL_UNSIGNED_INT, 0, d.instanceCount());
    context->printGLErrorLog();

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
    if (attrNor != -1) context->glDisableVertexAttribArray(attrNor);
    if (attrCol != -1) context->glDisableVertexAttribArray(attrCol);
    if (attrPosOffset != -1) context->glDisableVertexAttribArray(attrPosOffset);

}

void ShaderProgram::drawOverlay(Drawable &d) {
    useMe();

    context->glUniform1i(unif_sceneTexture, 2);
    //context->glUniform1i(unif_distanceTexture, 3);
    context->glUniform1i(unif_rawDistanceTexture, 4);
    context->glUniform1i(unif_heightMapTexture, 5);
    context->glUniform1i(unif_skyTexture, 6);



    if(d.elemCountOpaque() < 0) {
        throw std::out_of_range("Attempting to draw a drawable with m_count of " + std::to_string(d.elemCountOpaque()) + "!");
    }

    if (attrPos != -1 && d.bindPos()) {
        context->glEnableVertexAttribArray(attrPos);
        context->glVertexAttribPointer(attrPos, 4, GL_FLOAT, false, 0, NULL);
    }

    if (attrNor != -1 && d.bindNor()) {
        context->glEnableVertexAttribArray(attrNor);
        context->glVertexAttribPointer(attrNor, 4, GL_FLOAT, false, 0, NULL);
    }

    if (attrUV != -1 && d.bindUV()) {
        context->glEnableVertexAttribArray(attrUV);
        context->glVertexAttribPointer(attrUV, 2, GL_FLOAT, false, 0, NULL);
    }

    // Bind the index buffer and then draw shapes from it.
    // This invokes the shader program, which accesses the vertex buffers.
    d.bindOpaqueIdx();
    context->glDrawElements(d.drawMode(), d.elemCountOpaque(), GL_UNSIGNED_INT, 0);

    if (attrPos != -1) context->glDisableVertexAttribArray(attrPos);
    if (attrNor != -1) context->glDisableVertexAttribArray(attrNor);

    context->printGLErrorLog();
}


char* ShaderProgram::textFileRead(const char* fileName) {
    char* text;

    if (fileName != NULL) {
        FILE *file = fopen(fileName, "rt");

        if (file != NULL) {
            fseek(file, 0, SEEK_END);
            int count = ftell(file);
            rewind(file);

            if (count > 0) {
                text = (char*)malloc(sizeof(char) * (count + 1));
                count = fread(text, sizeof(char), count, file);
                text[count] = '\0';	//cap off the string with a terminal symbol, fixed by Cory
            }
            fclose(file);
        }
    }
    return text;
}

QString ShaderProgram::qTextFileRead(const char *fileName)
{
    QString text;
    QFile file(fileName);
    if(file.open(QFile::ReadOnly))
    {
        QTextStream in(&file);
        text = in.readAll();
        text.append('\0');
    }
    return text;
}

void ShaderProgram::printShaderInfoLog(int shader)
{
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    context->glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

    // should additionally check for OpenGL errors here

    if (infoLogLen > 0)
    {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        context->glGetShaderInfoLog(shader,infoLogLen, &charsWritten, infoLog);
        qDebug() << "ShaderInfoLog:" << "\n" << infoLog << "\n";
        delete [] infoLog;
    }

    // should additionally check for OpenGL errors here
}

void ShaderProgram::printLinkInfoLog(int prog)
{
    int infoLogLen = 0;
    int charsWritten = 0;
    GLchar *infoLog;

    context->glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLogLen);

    // should additionally check for OpenGL errors here

    if (infoLogLen > 0) {
        infoLog = new GLchar[infoLogLen];
        // error check for fail to allocate memory omitted
        context->glGetProgramInfoLog(prog, infoLogLen, &charsWritten, infoLog);
        qDebug() << "LinkInfoLog:" << "\n" << infoLog << "\n";
        delete [] infoLog;
    }
}


void ShaderProgram::setTexture(int slot) {
    useMe();
    if(unifTextureSample != -1)
    {
        context->glUniform1i(unifTextureSample, slot);
    }
}

void ShaderProgram::setShadowTexture(int slot) {
    useMe();
    if(unif_heightMapTexture!= -1)
    {
        context->glUniform1i(unif_heightMapTexture, slot);
    }
}





void ShaderProgram::setTime(float t) {
    useMe();
    if(unifTime != -1)
    {
        context->glUniform1f(unifTime, t);
    }
}

void ShaderProgram::setIdleTime(float idleTime) {
    useMe();
    if(unifIdleTime != -1)
    {
        context->glUniform1f(unifIdleTime, idleTime);
    }
}

void ShaderProgram::setNearestBlockDist(float dist) {
    useMe();
    if(unifNearestBlockDist != -1)
    {
        context->glUniform1f(unifNearestBlockDist, dist);
    }
}

void ShaderProgram::setCameraPos(glm::vec4 camPos) {
    useMe();
    if(unifCameraPos != -1)
    {
        context-> glUniform4f(unifCameraPos, camPos[0], camPos[1], camPos[2], 1.f);
    }
}

void ShaderProgram::setCameraLook(glm::vec4 camLook) {
    useMe();
    if(unifCameraLook != -1)
    {
        context-> glUniform4f(unifCameraLook, camLook[0], camLook[1], camLook[2], camLook[3]);
    }
}


void ShaderProgram::setScreenDimensions(glm::vec2 screenDimensions) {
    useMe();
    if(unifScreenDimensions != -1)
    {
        context-> glUniform2f(unifScreenDimensions, screenDimensions[0],  screenDimensions[1]);
    }
}

void ShaderProgram::setNormalMap(int slot) {
    useMe();
    if(unifNormalMap != -1)
    {
        context->glUniform1i(unifNormalMap, slot);
    }
}

void ShaderProgram::setLightSpaceMatrix(glm::mat4 lightSpaceMat4) {
    useMe();
    if(unif_lightSpaceMatrix != -1)
    {
        context->glUniformMatrix4fv(unif_lightSpaceMatrix,
                        // How many matrices to pass
                           1,
                        // Transpose the matrix? OpenGL uses column-major, so no.
                           GL_FALSE,
                        // Pointer to the first element of the matrix
                           &lightSpaceMat4[0][0]);
        }
}
void ShaderProgram::setLightDir(glm::vec4 lightDir) {
    useMe();
    if(unif_lightDir != -1)
    {
        context-> glUniform4f(unif_lightDir, lightDir[0], lightDir[1], lightDir[2], lightDir[3]);
    }
}

void ShaderProgram::setMoonDir(glm::vec4 moonDir) {
    useMe();
    if(unif_moonDir != -1)
    {
        context-> glUniform4f(unif_moonDir, moonDir[0], moonDir[1], moonDir[2], moonDir[3]);
    }
}
