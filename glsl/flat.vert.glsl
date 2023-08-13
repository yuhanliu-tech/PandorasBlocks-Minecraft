#version 150
// ^ Change this to version 130 if you have compatibility issues

//This is a vertex shader. While it is called a "shader" due to outdated conventions, this file
//is used to apply matrix transformations to the arrays of vertex data passed to it.
//Since this code is run on your GPU, each vertex is transformed simultaneously.
//If it were run on your CPU, each vertex would have to be processed in a FOR loop, one at a time.
//This simultaneous transformation allows your program to run much faster, especially when rendering
//geometry with millions of vertices.

uniform float u_Time;
uniform sampler2D u_Texture;

uniform mat4 u_Model;       // The matrix that defines the transformation of the
                            // object we're rendering. In this assignment,
                            // this will be the result of traversing your scene graph.

uniform mat4 u_ModelInvTr;  // The inverse transpose of the model matrix.
                            // This allows us to transform the object's normals properly
                            // if the object has been non-uniformly scaled.

uniform mat4 u_ViewProj;    // The matrix that defines the camera's transformation.
                            // We've written a static matrix for you to use for HW2,
                            // but in HW3 you'll have to generate one yourself

uniform vec4 u_Color;       // When drawing the cube instance, we'll set our uniform color to represent different block types.

in vec4 vs_Pos;             // The array of vertex positions passed to the shader

in vec4 vs_Nor;             // The array of vertex normals passed to the shader

in vec4 vs_Col;             // The array of vertex colors passed to the shader.

in vec4 vs_BlockSFX;    //SFX for the block [MovementMode, Glows, Animateable (gifs), Shininess]
                                    // MovementMode: 0 = no movement, 1 = ocean waves; 2 = foliage Wind
                                    // Glows: 0 = no glow, any value > 1 = luminance
                                    // Animateable: 0 = not animatable
                                    // Shininess: 0 for not; 1 for water shine (blinn phong)

out vec4 fs_Pos;
out vec4 fs_Nor;            // The array of normals that has been transformed by u_ModelInvTr. This is implicitly passed to the fragment shader.
out vec4 fs_LightVec;       // The direction in which our virtual light lies, relative to each vertex. This is implicitly passed to the fragment shader.
out vec4 fs_Col;            // The color of each vertex. This is implicitly passed to the fragment shader.
out vec4 fs_BlockSFX;

const vec4 lightDir = normalize(vec4(0.5, 1, 0.75, 0));  // The direction of our virtual light, which is used to compute the shading of
                                        // the geometry in the fragment shader.


float displaceWaves(vec2 v) {
    float x = v.x;
    float z = v.y;
    float offsetWaterY = (sin(x + u_Time * 30.f) + sin(x * 5.f + u_Time * 16.f) * 0.1f + sin(x * 14.f + u_Time * 22.f) * 0.2f + cos(x * 0.25f + u_Time * 5.0f))/12.f;
    offsetWaterY += (sin(z + u_Time * 24.f) + sin(z * 2.f + u_Time * 30.f) * 0.1f + sin(z * 12.f + u_Time * 12.f) * 0.3f + cos(z * 0.75f + u_Time * 7.0f))/12.f;
    //offsetWaterY /= 16.f;
    return offsetWaterY - 0.5f;
}

float displaceGrass(vec2 v) {
    float x = v.x;
    float ht = mod(16.f, v.y)/16;

    float offsetGrassZ = sin(u_Time * 30.f + x) + sin(x * 5.f + u_Time * 40.f) * 0.1f + sin(x * 0.25f + u_Time * 5.0f) ;
    offsetGrassZ /= 8.f;

    return offsetGrassZ * ht;
}

float displaceVines(vec2 v) {
    //float x = v.x;
    float y = v.y;

    float offsetVinesZ = sin(u_Time * 30.f + y) + sin(y * 5.f + u_Time * 40.f) * 0.1f + sin(y * 0.25f + u_Time * 5.0f) ;
    offsetVinesZ /= 8.f;

    return offsetVinesZ;
}

float displaceFloaters(vec2 v) {
    float x = v.x;
    float ht = mod(16.f, v.y)/16;

    float offsetGrassZ = sin(u_Time * 30.f + x) + sin(x * 5.f + u_Time * 40.f) * 0.1f + sin(x * 0.25f + u_Time * 5.0f) ;
    offsetGrassZ /= 8.f;

    return offsetGrassZ * ht;
}



void main()
{

    vec4 offsetPos = vs_Pos;
    if(vs_BlockSFX[3] == 1.f) {//It's water! Max height is 128
        offsetPos.y += displaceWaves(offsetPos.xz);
    } else if (vs_BlockSFX[0] == 2.f) {
        offsetPos.z += displaceGrass(offsetPos.xy);
    } else if (vs_BlockSFX[0] == 4.f) {
        offsetPos.z += displaceVines(offsetPos.xy);
    } else if (vs_BlockSFX[0] == 5.f) { //For vertical waves
        offsetPos.y += displaceFloaters(offsetPos.xy);
    }


    fs_BlockSFX = vs_BlockSFX;
    fs_Pos = offsetPos;
    fs_Col = vs_Col;                         // Pass the vertex colors to the fragment shader for interpolation

    mat3 invTranspose = mat3(u_ModelInvTr);
    fs_Nor = vec4(invTranspose * vs_Nor.xyz, 0);          // Pass the vertex normals to the fragment shader for interpolation.
                                                            // Transform the geometry's normals by the inverse transpose of the
                                                            // model matrix. This is necessary to ensure the normals remain
                                                            // perpendicular to the surface after the surface is transformed by
                                                            // the model matrix.


    vec4 modelposition = u_Model * offsetPos;   // Temporarily store the transformed vertex positions for use below

    fs_LightVec = (lightDir);  // Compute the direction in which the light source lies

    gl_Position = u_ViewProj * modelposition;// gl_Position is a built-in variable of OpenGL which is
                                             // used to render the final positions of the geometry's vertices
}
