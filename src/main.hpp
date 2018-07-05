#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};

struct StirlingInstance {
    StirlingInstance();

private:
};