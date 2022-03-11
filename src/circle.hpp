#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include "common.hpp"

// 渲染一个圆（来当做一个点来显示）
void renderCircle(glm::vec2 center, float radius, uint32_t vertCount, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f)) {
    shader.color = color;
    shader.use();

    std::vector<glm::vec2> vertices(vertCount);
    float step = 360.0f / vertCount;
    for (uint32_t i = 0; i < vertCount; i++) {
        float angle = i * step;
        vertices[i] = glm::vec2(std::cos(angle * PI / 180.0f),
                                std::sin(angle * PI / 180.0f));
        vertices[i] *= radius;
        vertices[i] += center;
    }

    uint32_t VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * vertCount, vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDrawArrays(GL_TRIANGLE_FAN, 0, vertCount);

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}