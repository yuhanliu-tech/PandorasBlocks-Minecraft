#version 150

uniform mat4 u_ViewProj;    // We're actually passing the inverse of the viewproj
                            // from our CPU, but it's named u_ViewProj so we don't
                            // have to bother rewriting our ShaderProgram class

uniform ivec2 u_Dimensions; // Screen dimensions

uniform vec3 u_Eye; // Camera pos

uniform float u_Time;

uniform float u_timeOfDay;

uniform vec4 u_lightDir;
uniform vec4 u_moonDir;

out vec4 outColor;

const float PI = 3.14159265359;
const float TWO_PI = 6.28318530718;

// hard coded arrays of 5 colors for sunset and dusk

// Sunset palette
const vec3 sunset[5] = vec3[](vec3(255, 229, 119) / 255.0,
                               vec3(254, 192, 81) / 255.0,
                               vec3(255, 137, 103) / 255.0,
                               vec3(253, 96, 81) / 255.0,
                               vec3(57, 32, 51) / 255.0);
// Dusk palette
const vec3 dusk[5] = vec3[](vec3(144, 96, 144) / 255.0,
                            vec3(96, 72, 120) / 255.0,
                            vec3(72, 48, 120) / 255.0,
                            vec3(48, 24, 96) / 255.0,
                            vec3(0, 24, 72) / 255.0);

// Noon palette
const vec3 noon[5] = vec3[](vec3(165, 229, 255) / 255.0,
                            vec3(145, 224, 255) / 255.0,
                            vec3(115, 215, 255) / 255.0,
                            vec3(85, 206, 255) / 255.0,
                            vec3(55, 198, 255) / 255.0);

// Midnight palette
const vec3 midnight[5] = vec3[](vec3(133, 89, 136) / 255.0,
                         vec3(107, 73, 132) / 255.0,
                         vec3(72, 52, 117) / 255.0,
                         vec3(43, 47, 119) / 255.0,
                         vec3(20,24,82) / 255.0);



const vec3 sunColor = vec3(255, 255, 190) / 255.0;
const vec3 moonColor = vec3(247, 228, 178) / 255.0;
const vec3 cloudColor = sunset[3];

// Gives UV coordinates by finding polar coordinates of XYZ on a unit sphere
// Assume p is point on sphere of radius 1 and use arcTan equation to find angle relative to equator (phi, theta)
vec2 sphereToUV(vec3 p) {
    float phi = atan(p.z, p.x);
    if(phi < 0) {
        phi += TWO_PI;
    }
    float theta = acos(p.y);
    return vec2(1 - phi / TWO_PI, 1 - theta / PI);
}

// mapping from uv coordinates to hard-coded gradient values based on uv.y
vec3 uvToSunset(vec2 uv) {
    if(uv.y < 0.5) { // e.g. y-coordinate at or below equator - yellow
        return sunset[0];
    }
    else if(uv.y < 0.55) { // linear interpolation between yellow/mango
        return mix(sunset[0], sunset[1], (uv.y - 0.5) / 0.05); // last parameter, t value must be between 0 and 1 (subtract floor and divide by max)
    }
    else if(uv.y < 0.6) {
        return mix(sunset[1], sunset[2], (uv.y - 0.55) / 0.05);
    }
    else if(uv.y < 0.65) {
        return mix(sunset[2], sunset[3], (uv.y - 0.6) / 0.05);
    }
    else if(uv.y < 0.75) {
        return mix(sunset[3], sunset[4], (uv.y - 0.65) / 0.1);
    }
    return sunset[4];
}

// mapping from uv coordinates to hard-coded gradient values based on uv.y
vec3 uvToDusk(vec2 uv) {
    if(uv.y < 0.5) {
        return dusk[0];
    }
    else if(uv.y < 0.55) {
        return mix(dusk[0], dusk[1], (uv.y - 0.5) / 0.05);
    }
    else if(uv.y < 0.6) {
        return mix(dusk[1], dusk[2], (uv.y - 0.55) / 0.05);
    }
    else if(uv.y < 0.65) {
        return mix(dusk[2], dusk[3], (uv.y - 0.6) / 0.05);
    }
    else if(uv.y < 0.75) {
        return mix(dusk[3], dusk[4], (uv.y - 0.65) / 0.1);
    }
    return dusk[4];
}

// mapping from uv coordinates to hard-coded gradient values based on uv.y
vec3 uvToNoon(vec2 uv) {
    if(uv.y < 0.5) {
        return noon[0];
    }
    else if(uv.y < 0.55) {
        return mix(noon[0], noon[1], (uv.y - 0.5) / 0.05);
    }
    else if(uv.y < 0.6) {
        return mix(noon[1], noon[2], (uv.y - 0.55) / 0.05);
    }
    else if(uv.y < 0.65) {
        return mix(noon[2], noon[3], (uv.y - 0.6) / 0.05);
    }
    else if(uv.y < 0.75) {
        return mix(noon[3], noon[4], (uv.y - 0.65) / 0.1);
    }
    return noon[4];
}

// mapping from uv coordinates to hard-coded gradient values based on uv.y
vec3 uvToMidnight(vec2 uv) {
    if(uv.y < 0.5) {
        return midnight[0];
    }
    else if(uv.y < 0.55) {
        return mix(midnight[0], midnight[1], (uv.y - 0.5) / 0.05);
    }
    else if(uv.y < 0.6) {
        return mix(midnight[1], midnight[2], (uv.y - 0.55) / 0.05);
    }
    else if(uv.y < 0.65) {
        return mix(midnight[2], midnight[3], (uv.y - 0.6) / 0.05);
    }
    else if(uv.y < 0.75) {
        return mix(midnight[3], midnight[4], (uv.y - 0.65) / 0.1);
    }
    return midnight[4];
}



vec2 random2( vec2 p ) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

vec3 random3( vec3 p ) {
    return fract(sin(vec3(dot(p,vec3(127.1, 311.7, 191.999)),
                          dot(p,vec3(269.5, 183.3, 765.54)),
                          dot(p, vec3(420.69, 631.2,109.21))))
                 *43758.5453);
}

float WorleyNoise3D(vec3 p)
{
    // Tile the space
    vec3 pointInt = floor(p);
    vec3 pointFract = fract(p);

    float minDist = 1.0; // Minimum distance initialized to max.

    // Search all neighboring cells and this cell for their point
    for(int z = -1; z <= 1; z++)
    {
        for(int y = -1; y <= 1; y++)
        {
            for(int x = -1; x <= 1; x++)
            {
                vec3 neighbor = vec3(float(x), float(y), float(z));

                // Random point inside current neighboring cell
                vec3 point = random3(pointInt + neighbor);

                // Animate the point
                point = 0.5 + 0.5 * sin(u_Time * 0.01 + 6.2831 * point); // 0 to 1 range

                // Compute the distance b/t the point and the fragment
                // Store the min dist thus far
                vec3 diff = neighbor + point - pointFract;
                float dist = length(diff);
                minDist = min(minDist, dist);
            }
        }
    }
    return minDist;
}

float WorleyNoise(vec2 uv)
{
    // Tile the space
    vec2 uvInt = floor(uv);
    vec2 uvFract = fract(uv);

    float minDist = 1.0; // Minimum distance initialized to max.

    // Search all neighboring cells and this cell for their point
    for(int y = -1; y <= 1; y++)
    {
        for(int x = -1; x <= 1; x++)
        {
            vec2 neighbor = vec2(float(x), float(y));

            // Random point inside current neighboring cell
            vec2 point = random2(uvInt + neighbor);

            // Animate the point
            point = 0.5 + 0.5 * sin(u_Time * 0.01 + 6.2831 * point); // 0 to 1 range

            // Compute the distance b/t the point and the fragment
            // Store the min dist thus far
            vec2 diff = neighbor + point - uvFract;
            float dist = length(diff);
            minDist = min(minDist, dist);
        }
    }
    return minDist;
}



float WorleyNoiseStars(vec3 p)
{
    // Tile the space
    vec3 pointInt = floor(p*100.f);
    vec3 pointFract = fract(p* 100.f);

    float minDist = 1.0; // Minimum distance initialized to max.

    // Search all neighboring cells and this cell for their point
    for(int z = -1; z <= 1; z++)
    {
        for(int y = -1; y <= 1; y++)
        {
            for(int x = -1; x <= 1; x++)
            {
                vec3 neighbor = vec3(float(x), float(y), float(z));

                // Random point inside current neighboring cell
                vec3 point = random3(pointInt + neighbor);

                // Animate the point
                //point = 0.5 + 0.5 * sin(u_Time * 0.01 + 6.2831 * point); // 0 to 1 range

                // Compute the distance b/t the point and the fragment
                // Store the min dist thus far
                vec3 diff = neighbor + point - pointFract;
                float dist = length(diff);
                minDist = min(minDist, dist);
            }
        }
    }


    if(minDist > 0.1f) {
        return 0.f;
    } else {
        return 1.f;
    }

    //return minDist;
}

vec3 random3Time( vec3 p ) {
    return fract(sin(vec3(dot(p,vec3(127.1, 311.7, 191.999)),
                          dot(p,vec3(269.5, 183.3, 765.54)),
                          dot(p, vec3(420.69, 631.2,109.21))))
                 * 43758.545 + 5 *  u_Time * 0.013);
}


float WorleyNoiseShootingStars(vec3 p)
{
    // Tile the space
    vec3 pointInt = floor(p * 10.f);
    vec3 pointFract = fract(p * 10.f);

    float minDist = 1; // Minimum distance initialized to max.

    // Search all neighboring cells and this cell for their point
    for(int z = -1; z <= 1; z++)
    {
        for(int y = -1; y <= 1; y++)
        {
            for(int x = -1; x <= 1; x++)
            {
                vec3 neighbor = vec3(float(x), float(y), float(z));

                // Random point inside current neighboring cell
                vec3 point = random3Time(pointInt + neighbor);

                // Compute the distance b/t the point and the fragment
                // Store the min dist thus far
                vec3 diff = neighbor + point - pointFract;
                float dist = length(abs(diff));
                minDist = min(minDist, dist);
            }
        }
    }


    if(minDist > 0.08f) {
        return 0.f;
    } else {
        return 1.f;
    }

    //return minDist;
}

// function that uses shell of FBM with Worley noise as basis noise function
// returns worley noise float generated from 3d space -- avoids obvious uv-wrapping issues
float worleyFBM(vec3 uv) {
    float sum = 0;
    float freq = 4;
    float amp = 0.5;
    for(int i = 0; i < 8; i++) {
        sum += WorleyNoise3D(uv * freq) * amp;
        freq *= 2;
        amp *= 0.5;
    }
    return sum;
}

vec3 rotate(vec3 p, float theta) {
    return vec3(p.x, cos(theta) * p.y - (sin(theta) * p.z), sin(theta) * p.y + (cos(theta) * p.z));
}

//#define RAY_AS_COLOR
//#define SPHERE_UV_AS_COLOR
#define WORLEY_OFFSET

void main()
{
    // ray casting
    vec2 ndc = (gl_FragCoord.xy / vec2(u_Dimensions)) * 2.0 - 1.0; // -1 to 1 NDC


    vec4 p = vec4(ndc.xy, 1, 1); // Pixel at the far clip plane
    p *= 1000.0; // Times far clip plane value
    p = /*Inverse of*/ u_ViewProj * p; // Convert from unhomogenized screen to world

    // procedural sky shader turns each pixel into ray originating from u_Eye
    vec3 rayDir = normalize(p.xyz - u_Eye);

#ifdef RAY_AS_COLOR
    outColor = vec4(0.5 * (rayDir + vec3(1,1,1)),1);
    return;
#endif

    vec2 uv = sphereToUV(rayDir); // finding polar coordinates of XYZ on a unit sphere
#ifdef SPHERE_UV_AS_COLOR
    outColor = vec4(uv,0,1);
    return;
#endif

    vec2 offset = vec2(0.0); // get rid of harsh lines between interpolations of color
#ifdef WORLEY_OFFSET
    // Get a noise value in the range [-1, 1]
    // by using Worley noise as the noise basis of FBM
    offset = vec2(worleyFBM(rayDir));
    offset *= 2.0;
    offset -= vec2(1.0);
#endif

    // Compute a gradient from the bottom of the sky-sphere to the top
    vec3 sunsetColor = uvToSunset(uv + offset * 0.1);
    vec3 duskColor = uvToDusk(uv + offset * 0.1);
    vec3 noonColor = uvToNoon(uv + offset * 0.1);
    vec3 midnightColor = uvToMidnight(uv + offset * 0.1);

    if(u_timeOfDay == 1) {
        if(WorleyNoiseStars(rayDir) == 1.f) {
            duskColor = vec3(1.f);
            midnightColor = vec3(1.f);
        }
    }


//    if(WorleyNoiseShootingStars(rayDir) == 1.f) {
//        duskColor = vec3(1.f,0, 0);
//        midnightColor = vec3(1.f, 0 , 0 );
//    }


    outColor = vec4(noonColor, 1);

    // Add a glowing sun in the sky, sun is a layer ontop of sunset gradient
    // ray leaving and going to this direction goes through sun
    // vary sun direction based on time to simulate day cycle
    vec3 sunDir = normalize(u_lightDir.xyz);
    float sunSize = 30;
    float angle = acos(dot(rayDir, sunDir)) * 360.0 / PI;

    vec3 moonDir =  normalize(u_moonDir.xyz); //normalize(rotate(vec3(1, 5, -1.0), u_Time * 0.01))
    float moonSize = 5;
    float moonAngle = acos(dot(rayDir, moonDir)) * 360.0 / PI;
#define SUNSET_THRESHOLD 0.75
#define DUSK_THRESHOLD -0.1
#define NOON 0.1


    float raySunDot = dot(rayDir, sunDir);
    float t = (raySunDot - SUNSET_THRESHOLD) / (DUSK_THRESHOLD - SUNSET_THRESHOLD);
    float dayDot = dot(sunDir, vec3(0,1,0));

    // make a moon
    if (moonAngle < moonSize) {
        if (moonAngle <7.5) {
            outColor = vec4(moonColor, 1);
        } else {
            outColor = vec4(mix(moonColor, duskColor, (angle - 7.5) / 22.5),1);
        }
    } else

    // ray is looking into just the sky
    if (dayDot > NOON) {
        outColor = vec4(mix(sunsetColor, noonColor, smoothstep(NOON, NOON + 0.4, dayDot)),1);
    } else
    if(raySunDot > SUNSET_THRESHOLD) {
        outColor = vec4(mix(sunsetColor, duskColor, t),1);
    }
    // Any dot product between 0.75 and -0.1 is a LERP b/t sunset and dusk color
    else if(raySunDot > DUSK_THRESHOLD) {
        outColor = vec4(mix(sunsetColor, duskColor, t),1);
    }
    // Any dot product <= -0.1 are pure dusk color
    else {
        outColor = vec4(duskColor, 1);
    }


    // If the angle between our ray dir and vector to center of sun
    // is less than the threshold, then we're looking at the sun
    if(angle < sunSize) { // ray falls within sun cona
        // Full center of sun
        if(angle < 7.5) {
            outColor = vec4(sunColor,1);
        }
        // Corona of sun, mix with sky color
        else {
            outColor = vec4(mix(sunColor, outColor.xyz, (angle - 7.5) / 22.5),1);
        }
    }






//        if(raySunDot > SUNSET_THRESHOLD) {
//            outColor = vec4(mix(sunsetColor, duskColor, t),1);
//        }
//        // Any dot product between 0.75 and -0.1 is a LERP b/t sunset and dusk color
//        else if(raySunDot > DUSK_THRESHOLD) {
//            outColor = vec4(mix(sunsetColor, duskColor, t),1);
//        }
//        // Any dot product <= -0.1 are pure dusk color
//        else {
//            outColor = vec4(duskColor, 1);
//        }



}
