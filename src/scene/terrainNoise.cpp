#include "terrainNoise.h"
#include <iostream>

namespace terrainNoise {

// water biome with mountains 
// 

bool isCave(glm::vec3 p) {

    float caveSize = 35.f;
    p /= caveSize; // scale coordinate system


    float noise = perlinNoise3D(p);

    return (noise >= 0.2);

}



bool isIsland(glm::vec3 p) {

    float caveSize = 35.f;
    p /= caveSize; // scale coordinate system


    float noise = perlinNoise3D(p);

    return (noise <= 0.2);

}


float noise2D(glm::vec2 p) {
    return glm::length(glm::fract(glm::sin(
                                      glm::vec2(glm::dot(p, glm::vec2(127.1, 311.7)),
                                                glm::dot(p, glm::vec2(269.5, 183.3)))) *= 43758.5453));
}

float noise3D(glm::vec3 p) {
    return glm::length(glm::fract(glm::sin(glm::vec3(p.x * 127.1, p.y * 269.5, p.z * 631.2)) *= 43758.5453));
}

glm::vec2 random2(glm::vec2 p) {
    return glm::fract(glm::sin(
                          glm::vec2(glm::dot(p, glm::vec2(127.1, 311.7)),
                                    glm::dot(p, glm::vec2(269.5, 183.3)))) *= 43758.5453);
}

glm::vec3 random3(glm::vec3 p) {
    return glm::fract(glm::sin(glm::vec3(glm::dot(p, glm::vec3(127.1, 311.7, 1)),
                                         glm::dot(p, glm::vec3(269.5, 183.3, 1)),
                                         glm::dot(p, glm::vec3(420.6, 631.2, 1)))
                               ) *= 43758.5453);
}

float interpNoise2D(float x, float y) {
    int intX = int(glm::floor(x));
    float fractX = glm::fract(x);
    int intY = int(glm::floor(y));
    float fractY = glm::fract(y);

    float v1 = noise2D(glm::vec2(intX, intY));
    float v2 = noise2D(glm::vec2(intX + 1, intY));
    float v3 = noise2D(glm::vec2(intX, intY + 1));
    float v4 = noise2D(glm::vec2(intX + 1, intY + 1));

    float i1 = glm::mix(v1, v2, fractX);
    float i2 = glm::mix(v3, v4, fractX);
    return glm::mix(i1, i2, fractY);
}

float interpNoise3D(float x,float y, float z) {

    int intX = int(glm::floor(x));
    float fractX = glm::fract(x);
    int intY = int(glm::floor(y));
    float fractY = glm::fract(y);
    int intZ = int(glm::floor(z));
    float fractZ = glm::fract(z);

    float v1 = noise3D(glm::vec3(intX, intY, intZ));
    float v2 = noise3D(glm::vec3(intX + 1, intY, intZ));
    float v3 = noise3D(glm::vec3(intX, intY + 1, intZ));
    float v4 = noise3D(glm::vec3(intX + 1, intY + 1, intZ));

    float v5 = noise3D(glm::vec3(intX, intY, intZ + 1));
    float v6 = noise3D(glm::vec3(intX + 1, intY, intZ + 1));
    float v7 = noise3D(glm::vec3(intX, intY + 1, intZ + 1));
    float v8 = noise3D(glm::vec3(intX + 1, intY + 1, intZ + 1));

    float i1 = glm::mix(v1, v2, fractX);
    float i2 = glm::mix(v3, v4, fractX);
    float i3 = glm::mix(v5, v6, fractX);
    float i4 = glm::mix(v7, v8, fractX);

    float zi1 = glm::mix(i1, i3, fractZ);
    float zi2 = glm::mix(i2, i4, fractZ);

    return glm::mix(zi1, zi2, fractY);
}


float fbm(glm::vec2 v) {
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

float fbm2(glm::vec2 v) {
    float total = 0.0f;
    float persistence = 0.8f;
    int octaves = 10;
    float freq = 1.2f;
    float amp = 0.8f;

    for(int i = 1; i <= octaves; i++) {
        total += interpNoise2D(v.x * freq,
                               v.y * freq) * amp;
        freq *= 1.2f;
        amp *= persistence;
    }
    return total;
}

float calcDryness(glm::vec2 v) {

    //Perlin noise function

    //    float h = perlinNoise2D(v/500.f);
    ////    h = std::abs(h);
    ////    h = glm::max(0.0f, h - 0.1f);

    //    std::cout << h << std::endl;
    //    h = glm::smoothstep(0.25f, 0.75f, h);
    //    return h;//130 + (glm::floor(0 + h * 75));

    v += glm::vec2(200);
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


    total = glm::smoothstep(0.2f, 0.7f, total);
    return total;
}

float calcTemp(glm::vec2 v) {
    //    float val = worleyNoise2(v);
    //    return glm::smoothstep(0.2f, 0.7f, val);

    return perlinNoise2D(v);
}


float calcMoistness(glm::vec2 v) {
    // perturb fbm
    glm::vec2 input1 = 2.f * v - 1.f;
    glm::mat4 rot = glm::rotate(glm::mat4(), glm::radians(30.f), glm::vec3(0.f, 0.f, 1.f));
    glm::vec4 rotated = rot * glm::vec4(v.x, v.y, 0.f, 0.f);
    glm::vec2 input2 = glm::vec2(rotated.x, rotated.y);
    glm::vec2 offset(fbm(input1),
                     fbm(input2));

    return glm::smoothstep(0.2f, 0.7f, fbm(glm::vec2(v.x + offset.x, v.y + offset.y)));

}

float calcHeight(glm::vec2 v) {
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

    total = glm::smoothstep(0.2f, 0.7f, total);
    return total;
}


float surflet2D(glm::vec2 P, glm::vec2 gridPoint) {
    // Compute falloff function by converting linear distance to a polynomial
    float distX = abs(P.x - gridPoint.x);
    float distY = abs(P.y - gridPoint.y);
    float tX = 1 - 6 * pow(distX, 5.f) + 15 * pow(distX, 4.f) - 10 * pow(distX, 3.f);
    float tY = 1 - 6 * pow(distY, 5.f) + 15 * pow(distY, 4.f) - 10 * pow(distY, 3.f);
    // Get the random vector for the grid point
    glm::vec2 gradient = 2.f * noise2D(gridPoint) - glm::vec2(1.f);
    // Get the vector from the grid point to P
    glm::vec2 diff = P - gridPoint;
    // Get the value of our height field by glm::dotting grid->P with our gradient
    float height = glm::dot(diff, gradient);
    // Scale our height field (i.e. reduce it) by our polynomial falloff function
    return height * tX * tY;
}

float surflet3D(glm::vec3 p, glm::vec3 gridPoint) {

    float t2x = abs(p.x - gridPoint.x);
    float t2y = abs(p.y - gridPoint.y);
    float t2z = abs(p.z - gridPoint.z);

    float tx = 1.f - 6.f * pow(t2x, 5.f) + 15.f * pow(t2x, 4.f) - 10.f * pow(t2x, 3.f);
    float ty = 1.f - 6.f * pow(t2y, 5.f) + 15.f * pow(t2y, 4.f) - 10.f * pow(t2y, 3.f);
    float tz = 1.f - 6.f * pow(t2z, 5.f) + 15.f * pow(t2z, 4.f) - 10.f * pow(t2z, 3.f);

    glm::vec3 gradient = random3(gridPoint) * 2.f - glm::vec3(1.f);

    glm::vec3 diff = p - gridPoint;

    float height = glm::dot(diff, gradient);

    return height * tx * ty * tz;
}


float perlinNoise2D(glm::vec2 uv) {
    float surfletSum = 0.f;
    // Iterate over the four integer corners surrounding uv
    for(int dx = 0; dx <= 1; ++dx) {
        for(int dy = 0; dy <= 1; ++dy) {
            surfletSum += surflet2D(uv, glm::floor(uv) + glm::vec2(dx, dy));
        }
    }
    return surfletSum;
}

float perlinNoise3D(glm::vec3 p) {
    float surfletSum = 0;

    for (int dx = 0; dx <= 1; ++dx) {
        for (int dy = 0; dy <= 1; ++dy) {
            for (int dz = 0; dz <= 1; ++dz) {
                surfletSum += surflet3D(p, glm::floor(p) + glm::vec3(dx, dy, dz));
            }
        }
    }
    return surfletSum;
}


glm::vec2 floatingIslandNoise(glm::vec3 p) {
    glm::vec2 uv = glm::vec2(p.x, p.z);
    uv += fbm(uv * 2.f);
    uv /= 60.0; // Now the space is 10x10 instead of 1x1. Change this to any number you want.
    glm::vec2 uvInt = glm::floor(uv);
    glm::vec2 uvfract = glm::fract(uv);

    float islandTop = 1.0;

    float minDist2 = 1.0f;
    float minDist = 1.0; // Minimum distance initialized to glm::max.
    for(int y = -1; y <= 1; ++y) {
        for(int x = -1; x <= 1; ++x) {
            glm::vec2 neighbor = glm::vec2(float(x), float(y)); // Direction in which neighbor cell lies
            glm::vec2 point = random2(uvInt + neighbor); // Get the Voronoi centerpoint for the neighboring cell

            float pointHeight = noise2D(point);

            glm::vec2 diff = neighbor + point - uvfract; // Distance between fragment coord and neighbor’s Voronoi point

            float dist = glm::length(diff);

            if(dist < minDist){
                minDist = dist;
                minDist2 = minDist;
                float heightScalar = glm::smoothstep(0.5f, 0.7f, 1-dist);
                islandTop = pointHeight * heightScalar;
            }
        }
    }

    minDist += 0.1 * fbm(uv * 80.f);
    float radius = fbm(uv) * 0.05f + 0.2f; //gives you random int btween 0 and 1


    float islandBottom = 0;


    if(minDist < radius) {
        islandTop *= 3.f * glm::mix(0.2f, 1.f, (1 - minDist));//* glm::smoothstep(0.f, 1.f, (1 - minDist) * 10);
        islandTop = glm::clamp(0.2f, 2.7f * glm::mix(0.2f, 1.f, 1.f), islandTop);
        islandTop = glm::mix(0.5f, 0.7f, islandTop);
        islandBottom = glm::mix(0.5f, 0.7f, minDist2 * 10);
        islandTop += perlinNoise2D(uv)/20;

        islandTop = 200 + glm::floor(30.0f * islandTop);
        islandBottom = 185 + glm::floor(30.0f * islandBottom);
    } else {
        islandBottom = islandTop;
    }

    return glm::vec2(islandBottom, islandTop);
}


float calcYOffsetIslands(glm::vec2 v) {
    float h = perlinNoise2D(v/120.f);
    h = std::abs(h);
    h = glm::max(0.0f, h - 0.1f);

    return 130 + (glm::floor(0 + h * 75));
}

float calcYOffsetMountains(glm::vec2 v) {
    float h = 0; // perlinNoise(v/64.);
    float amp = 0.5;
    float freq = 128;
    for(int i = 0; i < 4; i++) {

        glm::vec2 offset = glm::vec2(fbm(v/256.0f), fbm(v/300.0f) + (1000));
        float h1 = perlinNoise2D((v + offset)/freq);
        h1 = 1. - abs(h1);
        h1 = pow(h1, 1.5);
        h += h1 * amp;

        amp *= 0.5;
        freq *= 0.5;
    }

    return glm::floor(130.f + h * 100);
}

float calcYOffsetHills(glm::vec2 v) {
    return glm::floor(115.f + 0.5 * (perlinNoise2D(v / 80.f) + 1) * 50);
}
// make second new biome
// make floating island biome
// mix biomes
float calcSpikyBiome(glm::vec2 v) {
    float height = fbm2(v / 20.f);
    // float random = (float) rand() / RAND_MAX;

    float noiseOffsetFactor = 1.f - abs(perlinNoise2D(v / 80.f));
    height = height * noiseOffsetFactor;
    return glm::floor(50.f + height * 50.f);
}
float calcWaterMountains(glm::vec2 v) {
    float height = fbm2(v / 80.f);
    float random = (float) rand() / RAND_MAX;

    float noiseOffsetFactor = 1.f - abs(perlinNoise2D(v / 80.f));
    height = height * noiseOffsetFactor;

    /*
    height = 20.f + height * 80.f;
    // the higher it is, the greater the prob. we will plateau it

    /*
     *    if (height > 200.f) {
        if (random > 0.2f) {
            height = 200.f;
        }
    } else if (height > 180.f) {
        if (random > 0.4f) {
            height = 180.f;
        }
    } else

    if (height > 160.f) {
        if (random > 0.5f) {
            height = 160.f;
        }
    }
    height = glm::floor(glm::max(128.f, glm::min(height, 160.f)));

    return height;
    */

    if (height > 2.f) { // peaks // 0.9
        //height = pow(height, 1.5f);
        height = height * 50.f; // 50
        //    height += 20.f;
    } else if (height > 0.75f){ // 0.75
        height = height * 30.f; // 30
        // height += 20.f;
    } else {
        height = height * 10.f; // 10
    }
    height = 20.f + height;

    return glm::floor(glm::max(128.f, height));

}

float tianziNoise(glm::vec2 uv) {
    uv += fbm(uv);
    uv /= 40.0; // Now the space is 10x10 instead of 1x1. Change this to any number you want.
    glm::vec2 uvInt = glm::floor(uv);
    glm::vec2 uvfract = glm::fract(uv);

    float returnH = 1.0;

    float minDist = 1.0; // Minimum distance initialized to glm::max.
    for(int y = -1; y <= 1; ++y) {
        for(int x = -1; x <= 1; ++x) {
            glm::vec2 neighbor = glm::vec2(float(x), float(y)); // Direction in which neighbor cell lies
            glm::vec2 point = random2(uvInt + neighbor); // Get the Voronoi centerpoint for the neighboring cell

            float pointHeight = noise2D(point);

            glm::vec2 diff = neighbor + point - uvfract; // Distance between fragment coord and neighbor’s Voronoi point

            float dist = glm::length(diff);

            if(dist < minDist){
                minDist = dist;
                float heightScalar = glm::smoothstep(0.2f, 0.7f, (1-dist));
                returnH = pointHeight * heightScalar;
            }
        }
    }

    minDist += 0.1 * fbm(uv * 80.f);
    float radius = fbm(uv) * 0.2f + 0.2f; //gives you random int btween 0 and 1



    if(minDist < radius) {
        returnH *= 3.f * glm::mix(0.2f, 1.f, (1 - minDist));//* glm::smoothstep(0.f, 1.f, (1 - minDist) * 10);
        returnH = glm::clamp(0.2f, 2.7f * glm::mix(0.2f, 1.f, 1.f), returnH);
        returnH = glm::mix(0.5f, 0.7f, returnH);
        returnH += perlinNoise2D(uv)/20;
        returnH = 138 + glm::floor(80.0f * returnH);
        return returnH;
    } else {
        return 139 + fbm(uv) * 0.9 * worleyNoise2(uv);
    }


}

float worleyNoise2(glm::vec2 uv) {
    uv *= 1.2f; // Now the space is 10x10 instead of 1x1. Change this to any number you want.
    glm::vec2 uvInt = glm::floor(uv);
    glm::vec2 uvfract = glm::fract(uv);

    float returnH = 1.0;

    float minDist = 1.0; // Minimum distance initialized to glm::max.
    for(int y = -1; y <= 1; ++y) {
        for(int x = -1; x <= 1; ++x) {
            glm::vec2 neighbor = glm::vec2(float(x), float(y)); // Direction in which neighbor cell lies
            glm::vec2 point = random2(uvInt + neighbor); // Get the Voronoi centerpoint for the neighboring cell

            float pointHeight = noise2D(point);

            glm::vec2 diff = neighbor + point - uvfract; // Distance between fragment coord and neighbor’s Voronoi point

            float dist = glm::length(diff);

            if(dist < minDist){
                minDist = dist;
                float heightScalar = glm::smoothstep(0.2f, 0.7f, (1-dist));
                returnH = pointHeight * heightScalar;
            }
        }
    }

    minDist += 0.1 * fbm(uv * 80.f);
    float radius = fbm(uv) * 0.3f + 0.2f; //gives you random int btween 0 and 1

    if(minDist < radius) {
        returnH *= 3.f * glm::mix(0.2f, 1.f, (1 - minDist));//* glm::smoothstep(0.f, 1.f, (1 - minDist) * 10);
        returnH = glm::clamp(0.2f, 2.7f * glm::mix(0.2f, 1.f, 1.f), returnH);
        returnH = glm::mix(0.5f, 0.7f, returnH);
        returnH += perlinNoise2D(uv)/20;
        returnH = glm::floor(80.0f * returnH);
        return returnH;
    } else {
        return 0;
    }

}

}



