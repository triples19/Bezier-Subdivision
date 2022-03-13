#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "common.hpp"

// 渲染一组折线
void renderLine(const std::vector<glm::vec2>& points, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f), float width = lineWidth) {
    glLineWidth(width);
    shader.color = color;
    shader.use();
    std::vector<uint32_t> indecies;

    for (int i = 0; i < points.size(); i++) {
        if (i != points.size() - 1) {
            indecies.push_back(i);
            if (i < points.size()) {
                indecies.push_back(i + 1);
            }
        }
    }

    uint32_t VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * points.size(), points.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indecies.size(), indecies.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

    glDrawElements(GL_LINES, indecies.size(), GL_UNSIGNED_INT, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

template <size_t N>
void renderLine(const std::array<glm::vec2, N>& points, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f)) {
    renderLine(std::vector<glm::vec2>(points.begin(), points.end()), color);
}