#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "openglcontext.h"
#include <glm_includes.h>
#include <glm/glm.hpp>

#include "drawable.h"
#include "scene/chunk.h"

class ShaderProgram
{
public:
    GLuint vertShader; // A handle for the vertex shader stored in this shader program
    GLuint fragShader; // A handle for the fragment shader stored in this shader program
    GLuint prog;       // A handle for the linked shader program stored in this class

    int attrPos; // A handle for the "in" vec4 representing vertex position in the vertex shader
    int attrNor; // A handle for the "in" vec4 representing vertex normal in the vertex shader
    int attrCol; // A handle for the "in" vec4 representing vertex color in the vertex shader
    int attrPosOffset; // A handle for a vec3 used only in the instanced rendering shader
    int attrBlockSFX; //handle for sfx handling
    int attrBlockDepths;

    int attrUV;

    int unif_liquidType;
    int unif_sceneTexture;

    int unif_rawDistanceTexture;
    int unif_heightMapTexture;

    int unif_skyTexture;


    int unifModel; // A handle for the "uniform" mat4 representing model matrix in the vertex shader
    int unifModelInvTr; // A handle for the "uniform" mat4 representing inverse transpose of the model matrix in the vertex shader
    int unifViewProj; // A handle for the "uniform" mat4 representing combined projection and view matrices in the vertex shader
    int unifColor; // A handle for the "uniform" vec4 representing color of geometry in the vertex shader

    int unifTime;
    int unifIdleTime;

    int unifTextureSample;//texture
    int unifNormalMap;

    int unifCameraPos; //Added handle for world space
    int unifCameraLook; //Added handle for world space
    int unifScreenDimensions; //Added handle for world space
    int unifNearestBlockDist;


    // procedural sky
    int unifDimensions;
    int unifEye;

    int unif_lightSpaceMatrix;
    int unif_lightDir;
    int unif_moonDir;

    int unif_timeOfDay;
    int unif_waterBedHeight;

public:
    ShaderProgram(OpenGLContext* context);
    // Sets up the requisite GL data and shaders from the given .glsl files
    void create(const char *vertfile, const char *fragfile);
    // Tells our OpenGL context to use this shader to draw things
    void useMe();
    // Pass the given model matrix to this shader on the GPU
    void setModelMatrix(const glm::mat4 &model);
    // Pass the given Projection * View matrix to this shader on the GPU
    void setViewProjMatrix(const glm::mat4 &vp);
    // Pass the given color to this shader on the GPU
    void setGeometryColor(glm::vec4 color);
    // Draw the given object to our screen using this ShaderProgram's shaders

    void setTexture(int slot); //New
    void setTime(float time);
    void setIdleTime(float idleTime);
    void setCameraPos(glm::vec4 camPos);
    void setCameraLook(glm::vec4 camLook);
    void setScreenDimensions(glm::vec2 screenWidthHeight);
    void setNearestBlockDist(float dist);

    void setShadowTexture(int slot);

    void setNormalMap(int slot); //New

    void setLightSpaceMatrix(glm::mat4 lightSpaceMatrix);
    void setLightDir(glm::vec4 lightDirection);
    void setMoonDir(glm::vec4 moonDirection);

    void setTimeOfDay(float timeOfDay);

    void draw(Drawable &d);
    // Draw the given object to our screen multiple times using instanced rendering
    void drawInstanced(InstancedDrawable &d);
    // Utility function used in create()
    char* textFileRead(const char*);
    // Utility function that prints any shader compilation errors to the console
    void printShaderInfoLog(int shader);
    // Utility function that prints any shader linking errors to the console
    void printLinkInfoLog(int prog);

    // second drawing function that renders drawable with interleaved VBOs
    void drawInterleavedOpaque(Drawable &c);
    void drawInterleaveDTransparent(Drawable &c);

    void drawOverlay(Drawable &d);


    void drawHeightMap(Drawable &d);

    void overlayLiquidType(const int);

    QString qTextFileRead(const char*);

private:
    OpenGLContext* context;   // Since Qt's OpenGL support is done through classes like QOpenGLFunctions_3_2_Core,
                            // we need to pass our OpenGL context to the Drawable in order to call GL functions
                            // from within this class.
};


#endif // SHADERPROGRAM_H
