#ifndef TERRAINNOISE_H
#define TERRAINNOISE_H
#include <glm_includes.h>

namespace terrainNoise {
float noise2D(glm::vec2 p);

float noise3D(glm::vec2 p);

glm::vec2 random2(glm::vec2 p);

glm::vec3 random3(glm::vec3 p);

float interpNoise2D(float x, float y);

float interpNoise3D(float x, float y, float z);

float fbm(glm::vec2 v);

float fbm2(glm::vec2 v);

float calcDryness(glm::vec2 v);

float calcTemp(glm::vec2 v);

float calcHeight(glm::vec2 v);

float calcMoistness(glm::vec2 v);

float surflet2D(glm::vec2 P, glm::vec2 gridPoint);

float surflet3D(glm::vec3 P, glm::vec3 gridPoint);

float perlinNoise2D(glm::vec2 uv);

float perlinNoise3D(glm::vec3 p);

glm::vec2 floatingIslandNoise(glm::vec3 p);

float calcYOffsetIslands(glm::vec2 v);

float calcYOffsetMountains(glm::vec2 v);

float calcYOffsetHills(glm::vec2 v);

float calcSpikyBiome(glm::vec2 v);

float calcWaterMountains(glm::vec2 blockPos);

float tianziNoise(glm::vec2 uv);

float worleyNoise2(glm::vec2 uv);

bool isCave(glm::vec3);

bool isIsland(glm::vec3);


}

#endif // TERRAINNOISE_H
