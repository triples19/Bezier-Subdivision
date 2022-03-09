#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// 一些颜色常量
const glm::vec3 curveColor(0.1f, 0.8f, 0.8f);
const glm::vec3 pointColor(0.8f, 0.5f, 0.1f);
const glm::vec3 lineColor(0.8f, 0.3f, 0.8f);
const glm::vec3 lineColor1(0.4f, 0.4f, 0.4f);
const glm::vec3 lineColor2(0.7f, 0.9f, 0.4f);
const glm::vec3 t0Color(1.0f, 1.0f, 1.0f);

// 控制点的大小和顶点数
const float pointRadius = 0.06f;
const int pointVertCount = 10;

// Shader 结构体
struct Shader {
    uint32_t ID;
    glm::mat4 model = glm::identity<glm::mat4>();
    glm::mat4 view = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0f, 0.0f, -10.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 9.0f / 16.0f, 0.0f, 100.0f);
    glm::vec3 color = glm::vec3(0.5f, 1.0f, 0.5f);
    float depth;

    void use() {
        glUseProgram(ID);
        glUniformMatrix4fv(glGetUniformLocation(ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(glGetUniformLocation(ID, "color"), 1, glm::value_ptr(color));
        glUniform1fv(glGetUniformLocation(ID, "depth"), 1, &depth);
    }
};

// 共用的 Shader
static Shader shader;

constexpr double pi() { return std::acos(-1); }

// 插值
glm::vec2 lerp(const glm::vec2 &a, const glm::vec2 &b, float t) {
    return (1 - t) * a + t * b;
}

uint32_t loadShader(const char *path, GLenum shaderType) {
    std::string code;
    std::ifstream file;

    file.open(path);
    std::stringstream stream;
    stream << file.rdbuf();
    file.close();
    code = stream.str();

    auto code_cstr = code.c_str();

    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &code_cstr, NULL);
    glCompileShader(shader);

    // check for compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << infoLog << std::endl;
    }
    return shader;
}

uint32_t createShaderProgram(uint32_t vert, uint32_t frag) {
    uint32_t id = glCreateProgram();
    glAttachShader(id, vert);
    glAttachShader(id, frag);
    glLinkProgram(id);
    glDeleteShader(vert);
    glDeleteShader(frag);
    return id;
}