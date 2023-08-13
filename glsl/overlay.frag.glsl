#version 150
// ^ Change this to version 130 if you have compatibility issues

// Refer to the lambert shader files for useful comments

in vec4 fs_Col;
in vec2 fs_UV;

out vec4 out_Col;

uniform int u_liquidType;
uniform sampler2D u_sceneTexture;
uniform sampler2D u_distanceTexture;
uniform sampler2D u_rawDistanceTexture;
uniform sampler2D u_heightMapTexture;
uniform sampler2D u_skyTexture;
uniform vec2 u_ScreenDimensions;
uniform mat4 u_ViewProj;    // We're actually passing the inverse of the viewproj
// from our CPU, but it's named u_ViewProj so we don't
// have to bother rewriting our ShaderProgram class

uniform ivec2 u_Dimensions; // Screen dimensions

uniform vec3 u_Eye; // Camera pos

uniform float u_Time;

const float kernel[121] = float[](0.006849, 0.007239, 0.007559, 0.007795, 0.007941, 0.00799, 0.007941, 0.007795, 0.007559, 0.007239, 0.006849, 0.007239, 0.007653, 0.00799, 0.00824, 0.008394, 0.008446, 0.008394, 0.00824, 0.00799, 0.007653, 0.007239, 0.007559, 0.00799, 0.008342, 0.008604, 0.008764, 0.008819, 0.008764, 0.008604, 0.008342, 0.00799, 0.007559, 0.007795, 0.00824, 0.008604, 0.008873, 0.009039, 0.009095, 0.009039, 0.008873, 0.008604, 0.00824, 0.007795, 0.007941, 0.008394, 0.008764, 0.009039, 0.009208, 0.009265, 0.009208, 0.009039, 0.008764, 0.008394, 0.007941, 0.00799, 0.008446, 0.008819, 0.009095, 0.009265, 0.009322, 0.009265, 0.009095, 0.008819, 0.008446, 0.00799, 0.007941, 0.008394, 0.008764, 0.009039, 0.009208, 0.009265, 0.009208, 0.009039, 0.008764, 0.008394, 0.007941, 0.007795, 0.00824, 0.008604, 0.008873, 0.009039, 0.009095, 0.009039, 0.008873, 0.008604, 0.00824, 0.007795, 0.007559, 0.00799, 0.008342, 0.008604, 0.008764, 0.008819, 0.008764, 0.008604, 0.008342, 0.00799, 0.007559, 0.007239, 0.007653, 0.00799, 0.00824, 0.008394, 0.008446, 0.008394, 0.00824, 0.00799, 0.007653, 0.007239, 0.006849, 0.007239, 0.007559, 0.007795, 0.007941, 0.00799, 0.007941, 0.007795, 0.007559, 0.007239, 0.006849);

void main()
{


    //Getting the distance
    float distanceLerp = texture(u_rawDistanceTexture, fs_UV).g; //Perspective-based distance from camera


    vec4 screenCol = vec4(texture(u_sceneTexture, fs_UV).rgb, 1.f);

    //Distance Fog
    vec3 fogColor = texture(u_skyTexture, fs_UV).xyz;// + vec3(117.f, 188.f, 255.f)/255.f * 0.1f;
    float rawDistanceLerp = texture(u_rawDistanceTexture, fs_UV).r; //Raw distance from camera

    screenCol.rgb = mix(screenCol.rgb, fogColor, rawDistanceLerp);

    //Liquid Overlay Logic
    if (u_liquidType == 1) {
        screenCol += vec4(0, 0, screenCol.z * 0.9, 0);
    } else if (u_liquidType == 2) {
        screenCol += vec4(screenCol.x * 0.9, 0, 0, 0);
    }

    out_Col = screenCol;

    //Gaussian Blur Post Processing
    vec4 averagedColor = vec4(0.0f);
    int pixelRadius = 5;

    //Iterate through the pixels within the radius
    for(int r = -pixelRadius; r <= pixelRadius; r++){
        for(int c = -pixelRadius; c <= pixelRadius; c++){
            vec2 neighborUVs = vec2(float(c)/float(u_ScreenDimensions.x), float(r)/float(u_ScreenDimensions.y)) + fs_UV;
            float neighborDistanceLerp = texture(u_rawDistanceTexture, neighborUVs).r; //Sampling our neighbor's distance
            vec4 neighbor = texture(u_sceneTexture, neighborUVs); //And our neighbor's texture
            neighbor.rgb = mix(neighbor.rgb, fogColor, neighborDistanceLerp);
            averagedColor += kernel[(r + pixelRadius) + 11 * (c + pixelRadius)] * neighbor;
        }
    }


    //Comment this out to remove gaussian blur
    out_Col = mix(screenCol, vec4(vec3(averagedColor), 1.0f), distanceLerp);


    //Attempted Bloom Logic
    //   float lumThresh = 0.6;

    //   float luminance = 0.21f * averagedColor.r + 0.72f * averagedColor.g  + 0.07f * averagedColor.b;
    //   if(luminance >= lumThresh) {
    //       out_Col = 0.3 * averagedColor;
    //   }


    //   out_Col = vec4(vec3(texture(u_heightMapTexture, fs_UV)), 1.0f); //TEST TO SEE IF THE ADDED RENDER PASS WORKS
}
