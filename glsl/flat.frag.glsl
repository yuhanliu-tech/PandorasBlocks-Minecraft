#version 150
// ^ Change this to version 130 if you have compatibility issues

// This is a fragment shader. If you've opened this file first, please
// open and read lambert.vert.glsl before reading on.
// Unlike the vertex shader, the fragment shader actually does compute
// the shading of geometry. For every pixel in your program's output
// screen, the fragment shader is run for every bit of geometry that
// particular pixel overlaps. By implicitly interpolating the position
// data passed into the fragment shader by the vertex shader, the fragment shader
// can compute what color to apply to its pixel based on things like vertex
// position, light position, and vertex color.

uniform vec4 u_Color; // The color with which to render this instance of geometry.
uniform vec4 u_CameraPos;
uniform vec4 u_CameraLook;
uniform sampler2D u_Texture;
uniform sampler2D u_NormalMap;
uniform float u_nearestBlockDist;
uniform float u_Time;
uniform float u_idleTime;


// These are the interpolated values out of the rasterizer, so you can't know
// their specific values without knowing the vertices that contributed to them
in vec4 fs_Pos;
in vec4 fs_Nor;
in vec4 fs_LightVec;
in vec4 fs_Col;

//SFX for the block [MovementMode, Glows, Animateable (gifs), Shininess]
                                    // MovementMode: 0 = no movement, 1 = ocean waves; 2 = foliage Wind
                                    // Glows: 0 = no glow, any value > 1 = luminance
                                    // Animateable: 0 = not animatable
                                    // Shininess: 0 for not; 1 for water shine (blinn phong)

in vec4 fs_BlockSFX;

out vec4 out_Col; // This is the final output color that you will see on your
// screen for the pixel that is currently being processed.


void main(){
    //Calculating texture and base texture color
    vec2 uv = vec2 (fs_Col.xy/16.f);

    vec4 diffuseColor = texture(u_Texture, uv);
    if(diffuseColor.a < 0.5f) {
        discard;
    }

    float blurMultiplier = clamp(u_idleTime * 2.f, 0, 5);

    float dist = length(u_CameraPos - fs_Pos)/100;
    dist = abs(dot(u_CameraLook, fs_Pos - u_CameraPos));
    float focalLength = u_nearestBlockDist;

    float focalRange = u_nearestBlockDist * 5 * (1/blurMultiplier);
    float dofBlurAmt = min(1.f, abs(dist - focalLength)/focalRange);
    out_Col = vec4(vec3(dofBlurAmt), 1.f);

    //out_Col = vec4(vec3(dist), 1.f);


}
