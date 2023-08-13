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

//Lighting sh*t
uniform vec4 u_lightDir;
uniform mat4 u_lightSpaceMatrix;


in vec4 vs_Pos;             // The array of vertex positions passed to the shader

in vec4 vs_Nor;             // The array of vertex normals passed to the shader

in vec4 vs_Col;             // The array of vertex colors passed to the shader.

in vec4 vs_BlockSFX;    //SFX for the block [MovementMode, Glows, Animateable (gifs), Shininess]
                                    // MovementMode: 0 = no movement, 1 = ocean waves; 2 = foliage Wind
                                    // Glows: 0 = no glow, any value > 1 = luminance
                                    // Animateable: 0 = not animatable
                                    // Shininess: 0 for not; 1 for water shine (blinn phong)

in vec4 vs_BlockDepth;    //Depth data for the block
                                    // Depth of highest above terrain block before this block

out vec4 fs_Pos;
out vec4 fs_Nor;            // The array of normals that has been transformed by u_ModelInvTr. This is implicitly passed to the fragment shader.
out vec4 fs_LightVec;       // The direction in which our virtual light lies, relative to each vertex. This is implicitly passed to the fragment shader.
out vec4 fs_Col;            // The color of each vertex. This is implicitly passed to the fragment shader.
out vec4 fs_BlockSFX;

out vec4 fs_BlockDepth;


out vec4 fs_lightSpacePos;

const mat4 biasMatrix = mat4(
0.5, 0.0, 0.0, 0.0,
0.0, 0.5, 0.0, 0.0,
0.0, 0.0, 0.5, 0.0,
0.5, 0.5, 0.5, 1.0
);

float random1(vec3 p) {
    return fract(sin(dot(p,vec3(127.1, 311.7, 191.999)))
                 *43758.5453);
}

float mySmoothStep(float a, float b, float t) {
    t = smoothstep(0, 1, t);
    return mix(a, b, t);
}

float cubicTriMix(vec3 p) {
    vec3 pFract = fract(p);
    float llb = random1(floor(p) + vec3(0,0,0));
    float lrb = random1(floor(p) + vec3(1,0,0));
    float ulb = random1(floor(p) + vec3(0,1,0));
    float urb = random1(floor(p) + vec3(1,1,0));

    float llf = random1(floor(p) + vec3(0,0,1));
    float lrf = random1(floor(p) + vec3(1,0,1));
    float ulf = random1(floor(p) + vec3(0,1,1));
    float urf = random1(floor(p) + vec3(1,1,1));

    float mixLoBack = mySmoothStep(llb, lrb, pFract.x);
    float mixHiBack = mySmoothStep(ulb, urb, pFract.x);
    float mixLoFront = mySmoothStep(llf, lrf, pFract.x);
    float mixHiFront = mySmoothStep(ulf, urf, pFract.x);

    float mixLo = mySmoothStep(mixLoBack, mixLoFront, pFract.z);
    float mixHi = mySmoothStep(mixHiBack, mixHiFront, pFract.z);

    return mySmoothStep(mixLo, mixHi, pFract.y);
}

float fbm(vec3 p) {
    float amp = 0.5f;
    float freq = 4.0f;
    float sum = 0.0f;
    for(int i = 0; i < 8; i++) {
        sum += cubicTriMix(p * freq) * amp;
        amp *= 0.5f;
        freq *= 2.0f;
    }
    return sum;
}

float noise2D(vec2 p) {
    return length(fract(sin(vec2(dot(p, vec2(127.1f, 311.7f)), dot(p, vec2(269.5f, 183.3f)))) * 43758.5453f));
}

vec2 random2(vec2 p) {
    return fract(sin(
                          vec2(dot(p, vec2(127.1f, 311.7f)),
                                    dot(p, vec2(269.5f, 183.3f)))) * 43758.5453f);
}

float interpNoise2D(float x, float y) {
    int intX = int(floor(x));
    float fractX = fract(x);
    int intY = int(floor(y));
    float fractY = fract(y);

    float v1 = noise2D(vec2(intX, intY));
    float v2 = noise2D(vec2(intX + 1, intY));
    float v3 = noise2D(vec2(intX, intY + 1));
    float v4 = noise2D(vec2(intX + 1, intY + 1));

    float i1 = mix(v1, v2, fractX);
    float i2 = mix(v3, v4, fractX);
    return mix(i1, i2, fractY);
}


float fbm(vec2 v) {
    float total = 0.0f;
    float persistence = 0.5f;
    int octaves = 8;
    float freq = 1.2f;
    float amp = 0.5f;

    for(int i = 1; i <= octaves; i++) {
        total += interpNoise2D(v.x * freq,
                               v.y * freq) * amp;
        freq *= 1.2f;
        amp *= persistence;
    }
    return total;
}


float calcDryness(vec2 v) {

    //Perlin noise function

//    float h = perlinNoise2D(v/500.f);
////    h = abs(h);
////    h = max(0.0f, h - 0.1f);

//    cout << h << endl;
//    h = smoothstep(0.25f, 0.75f, h);
//    return h;//130 + (floor(0 + h * 75));



    v += vec2(200.f);
    float total = 0.0f;
    float persistence = 0.6f;
    int octaves = 12;
    float freq = 0.001f;
    float amp = 0.62f;

    for(int i = 1; i <= octaves; i++) {
        total += interpNoise2D(v.x * freq,
                               v.y * freq) * amp;
        freq = 0.004f;
        amp *= persistence;
    }


    total = smoothstep(0.2f, 0.7f, total);
    return total;
}



float calcHeight(vec2 v) {
    float total = 0.0f;
    float persistence = 0.5f;
    int octaves = 12;
    float freq = 0.001f;
    float amp = 0.5f;

    for(int i = 1; i <= octaves; i++) {
        total += interpNoise2D(v.x * freq,
                               v.y * freq) * amp;
        freq = 0.004f;
        amp *= persistence;
    }

    total = smoothstep(0.2f, 0.7f, total);
    return total;
}




float surflet2D(vec2 P, vec2 gridPoint) {
    // Compute falloff function by converting linear distance to a polynomial
    float distX = abs(P.x - gridPoint.x);
    float distY = abs(P.y - gridPoint.y);
    float tX = 1 - 6 * pow(distX, 5.f) + 15 * pow(distX, 4.f) - 10 * pow(distX, 3.f);
    float tY = 1 - 6 * pow(distY, 5.f) + 15 * pow(distY, 4.f) - 10 * pow(distY, 3.f);
    // Get the random vector for the grid point
    vec2 gradient = 2.f * noise2D(gridPoint) - vec2(1.f);
    // Get the vector from the grid point to P
    vec2 diff = P - gridPoint;
    // Get the value of our height field by dotting grid->P with our gradient
    float height = dot(diff, gradient);
    // Scale our height field (i.e. reduce it) by our polynomial falloff function
    return height * tX * tY;
}



float perlinNoise2D(vec2 uv) {
    float surfletSum = 0.f;
    // Iterate over the four integer corners surrounding uv
    for(int dx = 0; dx <= 1; ++dx) {
        for(int dy = 0; dy <= 1; ++dy) {
            surfletSum += surflet2D(uv, floor(uv) + vec2(dx, dy));
        }
    }
    return surfletSum;
}



float calcYOffsetIslands(vec2 v) {
    float h = perlinNoise2D(v/120.f);
    h = abs(h);
    h = max(0.0f, h - 0.1f);

    return 130 + (floor(0 + h * 75.f));
}

float calcYOffsetMountains(vec2 v) {
    float h = 0; // perlinNoise(v/64.);
    float amp = 0.5f;
    float freq = 128;
    for(int i = 0; i < 4; i++) {

        vec2 offset = vec2(fbm(v/256.0f), fbm(v/300.0f) + (1000));
        float h1 = perlinNoise2D((v + offset)/freq);
        h1 = 1. - abs(h1);
        h1 = pow(h1, 1.5f);
        h += h1 * amp;

        amp *= 0.5f;
        freq *= 0.5f;
    }

    return floor(130.f + h * 100);
}

float calcYOffsetHills(vec2 v) {
    return floor(115.f + 0.5 * (perlinNoise2D(v / 80.f) + 1) * 50);
}

float worleyNoise(vec2 uv) {
    uv += fbm(uv);
    uv /= 50.0f; // Now the space is 10x10 instead of 1x1. Change this to any number you want.
    vec2 uvInt = floor(uv);
    vec2 uvfract = fract(uv);

    float returnH = 1.0f;

    float minDist = 1.0f; // Minimum distance initialized to max.
    for(int y = -1; y <= 1; ++y) {
        for(int x = -1; x <= 1; ++x) {
            vec2 neighbor = vec2(float(x), float(y)); // Direction in which neighbor cell lies
            vec2 point = random2(uvInt + neighbor); // Get the Voronoi centerpoint for the neighboring cell

            float pointHeight = noise2D(point);

            vec2 diff = neighbor + point - uvfract; // Distance between fragment coord and neighbor’s Voronoi point

            float dist = length(diff);

            if(dist < minDist){
                minDist = dist;
                float heightScalar = smoothstep(0.5f, 0.7f, 1-dist);
                returnH = pointHeight * heightScalar;
            }
        }
    }

    float radius = fbm(uv)/2.0f + 0.2f; //gives you random int btween 0 and 1
    if(minDist > radius) {
        returnH *= 0.2f;
    } else {
        returnH *= 3.f;
    }

    returnH = mix(0.5f, 0.7f, returnH);
    returnH += perlinNoise2D(uv)/60.f;

    return 128.f + floor(70.0f * returnH);
}




float calcH(vec2 xz) {
    vec2 blockPos = xz;

    float dryness = calcDryness(blockPos);
    float height = calcHeight(blockPos);

    //Dry Terrains
    float forestHeight = calcYOffsetHills(blockPos);
    float snowMtnHeight = calcYOffsetMountains(blockPos);

    //Wet Terrains
    float islandsHeight = calcYOffsetIslands(blockPos);
    float pillarsHeight = worleyNoise(blockPos);

    float dryTerrains = mix(forestHeight, snowMtnHeight, height);
    float wetTerrains = mix(islandsHeight, pillarsHeight, (1.0f  -height));

    return dryTerrains; //glm::mix(wetTerrains, dryTerrains, dryness);

}

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

bool approxEqual(float value, float diff) {
    return(abs(value - diff) < 0.1f);
}



void main()
{

    vec3 tentativeNor = vs_Nor.xyz;
    vec4 offsetPos = vs_Pos;

    //All Vertex Deformation handling
    if(approxEqual(vs_BlockSFX[3], 1.f)) {//If we want water waves
        offsetPos.y += displaceWaves(offsetPos.xz);
        if(vs_Nor.y == 1.f) { //Calculating normals for water :)
            float x = vs_Pos.x;
            float z = vs_Pos.z;
            float incr = 0.1;
            tentativeNor = vec3((displaceWaves(vec2(x + incr, z)) - displaceWaves(vec2(x - incr, z))),
                                        2 * incr,
                                        (displaceWaves(vec2(x, z + incr)) - displaceWaves(vec2(x, z - incr))));
           tentativeNor = normalize(tentativeNor);
        }
    } else if (approxEqual(vs_BlockSFX[0], 2.f)) { //For grass waves
        offsetPos.z += displaceGrass(offsetPos.xy);
    } else if (approxEqual(vs_BlockSFX[0], 4.f) || approxEqual(vs_BlockSFX[0], 3.f)) { //For vine waves
        offsetPos.z += displaceVines(offsetPos.xy);
    } else if (approxEqual(vs_BlockSFX[0], 5.f)) { //For vertical waves
        offsetPos.y += displaceFloaters(offsetPos.xy);
    }

    fs_BlockSFX = vs_BlockSFX;
    fs_Pos = offsetPos;
    fs_Col = vs_Col;                         // Pass the vertex colors to the fragment shader for interpolation

    mat3 invTranspose = mat3(u_ModelInvTr);
    fs_Nor = vec4(invTranspose * tentativeNor, 0);          // Pass the vertex normals to the fragment shader for interpolation.
                                                            // Transform the geometry's normals by the inverse transpose of the
                                                            // model matrix. This is necessary to ensure the normals remain
                                                            // perpendicular to the surface after the surface is transformed by
                                                            // the model matrix.


    vec4 modelposition = u_Model * offsetPos;   // Temporarily store the transformed vertex positions for use below

    fs_LightVec = u_lightDir; // Compute the direction in which the light source lies

    fs_lightSpacePos = biasMatrix * u_lightSpaceMatrix * (modelposition / modelposition.w);

    gl_Position = u_ViewProj * modelposition;// gl_Position is a built-in variable of OpenGL which is
                                             // used to render the final positions of the geometry's vertices

    fs_BlockDepth = vs_BlockDepth;
}
