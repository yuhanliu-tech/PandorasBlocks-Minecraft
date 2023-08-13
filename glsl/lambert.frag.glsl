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
uniform sampler2D u_Texture;
uniform sampler2D u_NormalMap;
uniform sampler2D u_heightMapTexture;
uniform mat4 u_lightSpaceMatrix;
uniform vec4 u_lightDir;

uniform float u_timeOfDay;

uniform float u_Time;

// These are the interpolated values out of the rasterizer, so you can't know
// their specific values without knowing the vertices that contributed to them
in vec4 fs_Pos;
in vec4 fs_Nor;
in vec4 fs_LightVec;
in vec4 fs_Col;
in vec4 fs_lightSpacePos;
in vec4 fs_BlockDepth;


//SFX for the block [MovementMode, Glows, Animateable (gifs), Shininess]
// MovementMode: 0 = no movement, 1 = ocean waves; 2 = foliage Wind
// Glows: 0 = no glow, any value > 1 = luminance
// Animateable: 0 = not animatable
// Shininess: 0 for not; 1 for water shine (blinn phong)

in vec4 fs_BlockSFX;

out vec4 out_Col; // This is the final output color that you will see on your
// screen for the pixel that is currently being processed.

bool approxEqual(float value, float diff) {
    return(abs(value - diff) < 0.1f);
}


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


float liquidFbm(vec2 v) {
    v -= u_Time;

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
    return 1 - total;
}




mat3 getTBN(vec3 origNorm) {
    vec3 tan; //Tangent?
    vec3 bitan; //bitangent

    if (abs(origNorm.x) > (origNorm.y)) {
        tan = vec3(-origNorm.z, 0.f, origNorm.x) / sqrt(origNorm.x * origNorm.x + origNorm.z * origNorm.z);
    }
    else {
        tan = vec3(0.f, origNorm.z, -origNorm.y) / sqrt(origNorm.y * origNorm.y + origNorm.z * origNorm.z);
    }

    bitan = cross(origNorm, tan);

    origNorm = normalize(origNorm);
    tan = normalize(tan);
    bitan = normalize(bitan);

    return mat3(tan, bitan, origNorm);
}

float calcSpecular(vec4 camPos, vec4 fragPos, vec4 light, vec4 norm){
    vec4 viewVec = normalize(camPos - fragPos); //maybe conflict

    vec4 H = normalize(viewVec + light);
    float specularIntensity = max(pow(dot(H, norm), 70.f), 0);

    return specularIntensity;
}


const vec2 poissonDisk[4] = vec2[](
        vec2( -0.94201624, -0.39906216 ),
vec2( 0.94558609, -0.76890725 ),
vec2( -0.094184101, -0.92938870 ),
vec2( 0.34495938, 0.29387760 )
);

float shadowCalc() {
    if (u_lightSpaceMatrix == mat4(0)) {
        return 0;
    }

    float visibility = 1.0f;

    //Bias Calculating
    vec4 n = normalize(fs_Nor);
    vec4 l = normalize(u_lightDir);
    float bias = 0.0005 * tan(acos(clamp(dot(n, l), 0.f, 1.f))); // cosTheta is dot( n,l ), clamped between 0 and 1
    bias = clamp(bias, 0, 0.01);


    vec3 pos = fs_lightSpacePos.xyz;
    float depth = texture(u_heightMapTexture, pos.xy).r;

    for (int i = 0; i < 4; i++){
        if (texture(u_heightMapTexture, pos.xy + poissonDisk[i]/8000.f).r  <  pos.z - bias) {
            if(u_timeOfDay == 1) {
                visibility = 0;
            } else {
                visibility -= 0.2;
            }
        }
    }

    //    if(visibility > 0.4f) {
    //        visibility = 1.f;
    //    }

    //    if(depth < pos.z - bias) {
    //        visibility = 0.f;
    //    }
    //     //depth < pos.z - bias ? 0.0f : 1.0f;


    if(u_timeOfDay == 1 && visibility != 0) {
        return 0.2f; //night shadows
    } else if(u_timeOfDay == 2) {
        return 0;
    }

    return visibility;
}



void main(){
    //Calculating texture and base texture color
    vec2 uv = vec2 (fs_Col.xy/16.f);

    float loopingTime = (fs_BlockSFX[2]) * fract(u_Time * 2) / 16.f;//Ranges from 0 to 3
    uv.x += loopingTime;

    //vec4 diffuseColor = texture(u_distanceTexture, uv);
    vec4 diffuseColor = texture(u_Texture, uv);
    if(diffuseColor.a < 0.5f) {
        discard;
    }

    //Sampling normals and converting to range [-1, 1]
    vec3 normMap = texture(u_NormalMap, uv).rgb;
    normMap = normalize(normMap * 2.0f - 1.0f);

    //Calculating tangent and bitangent from a normal
    mat3 tbn = getTBN(fs_Nor.xyz);
    vec3 newNorm = tbn * normMap;

    //Calculating normals for water :)
    float x = floor(fs_Pos.x * 4)/4;
    float z = floor(fs_Pos.z * 4)/4;
    float incr = 0.1;

    vec3 waterPerturb = vec3((liquidFbm(vec2(x + incr, z)) - liquidFbm(vec2(x - incr, z)))/incr,
                             0,
                             (liquidFbm(vec2(x, z + incr)) - liquidFbm(vec2(x, z - incr)))/incr);

    waterPerturb.y = sqrt(1 - fs_Nor.x * fs_Nor.x - fs_Nor.z * fs_Nor.z);

    if(length(normMap) < 1.f) { //Only assigning normal maps to textures who have a normal map
        newNorm = fs_Nor.xyz;
    }

    // Compute final shaded color
    vec3 preSFXCol = diffuseColor.rgb;


    if(approxEqual(fs_BlockSFX[0], 1.f)) { //Height-based rendering for water
        float height = fs_BlockDepth.x;
        vec4 colorOffset;
        float heightdiff = 135.f - height;
        preSFXCol.g -= heightdiff * 0.1f;
        preSFXCol.r -= heightdiff * 0.1f;
        preSFXCol.b -= heightdiff * 0.01f;
    }

    float specularIntensity = 0.f;

    if(approxEqual(fs_BlockSFX[3], 1.f) && approxEqual(fs_BlockSFX[0], 1.f)) {
        specularIntensity = calcSpecular(u_CameraPos, fs_Pos, fs_LightVec, vec4(normalize(fs_Nor.xyz + waterPerturb), 1.f));
    } else if(approxEqual(fs_BlockSFX[3], 1.f)) {
        specularIntensity = calcSpecular(u_CameraPos, fs_Pos, fs_LightVec, vec4(normalize(newNorm), 1.f));
    }

    float shadowCalc = shadowCalc();

    specularIntensity *= shadowCalc;

    // Calculate the diffuse term for Lambert shading
    float diffuseTerm = dot(normalize(vec4(newNorm, 1.f)), normalize(fs_LightVec) * 1.2f);

    // Avoid negative lighting values
    diffuseTerm = clamp(diffuseTerm, 0, 1);

    float ambientTerm = 0.2;


    float lightIntensity = shadowCalc * diffuseTerm + ambientTerm;   //Add a small float value to the color multiplier
    //to simulate ambient lighting. This ensures that faces that are not
    //lit by our point light are not completely black.

    mat4 lightMatrix = mat4(vec4(lightIntensity, 0, 0, 0), vec4(0, lightIntensity, 0, 0), vec4(0, 0, lightIntensity, 0), vec4(0, 0, 0, 1));

    out_Col = vec4(preSFXCol * lightIntensity, diffuseColor.a) + vec4(specularIntensity);

    out_Col[3] = fs_Col[2];

    //If the block is meant to glow, screw the color processing
    if(approxEqual(fs_BlockSFX[3], 2)){
        out_Col = diffuseColor;
    }

    if(approxEqual(fs_BlockSFX[3], 3)){
        float dist = abs(length(u_CameraPos.xz - fs_Pos.xz)/10.f);
        out_Col = max(vec4(vec3(clamp(0, 1, 1-dist)), 1.f), diffuseColor);

    }


}
