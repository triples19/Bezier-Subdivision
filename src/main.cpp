#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <array>
#include <vector>
#include <algorithm>
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glm/gtc/type_ptr.hpp>
#include "common.hpp"
#include "line.hpp"
#include "bezier.hpp"
#include "circle.hpp"

const uint32_t SCR_WIDTH = 800;                // 默认窗口宽度
const uint32_t SCR_HEIGHT = 800;               // 默认窗口高度
const float X_MAX = 5.0f;                      // 默认X坐标最大值
const float Y_MAX = 5.0f;                      // 默认Y坐标最大值
const char* WIN_TITLE = "Bezier-Subdivision";  // 窗口标题

uint32_t curScrWidth = SCR_WIDTH;    // 当前窗口宽度
uint32_t curScrHeight = SCR_HEIGHT;  // 当前窗口高度

GLFWwindow* mainWindow;

void initGLFW();
void initGLAD();
void initImGui();
void initShader();  // 用默认的 shader 来初始化 (shaders/vert.glsl & shaders/frag.glsl)
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

std::array<glm::vec2, 4> points;  // 4个控制点
float t0 = 0.5f;                  // t0
bool showSpilt = true;            // 是否显示分割出的控制点

// 初始化四个默认的控制点
void init() {
    points = {
        glm::vec2(-3.0f, 0.0f),
        glm::vec2(-3.0f, 3.0f),
        glm::vec2(3.0f, 3.0f),
        glm::vec2(3.0f, 0.0f)};
}

void renderUpdate() {
    // 设置投影矩阵来适应窗口大小
    shader.projection = glm::ortho(
        -X_MAX / SCR_WIDTH * curScrWidth,
        X_MAX / SCR_WIDTH * curScrWidth,
        -Y_MAX / SCR_HEIGHT * curScrHeight,
        Y_MAX / SCR_HEIGHT * curScrHeight, 0.1f, 100.0f);

    // 画 Bezier 曲线
    drawSplitBezier(points, t0, showSpilt);
}

glm::vec2* draggingPoint = nullptr;  // 正在拖动的控制点

// 处理鼠标点击和拖动
void processInput() {
    int lmb = glfwGetMouseButton(mainWindow, GLFW_MOUSE_BUTTON_LEFT);

    // 获取鼠标坐标并手动转换到世界坐标系
    // 注：未考虑缩放等变换
    double posX, posY;
    glfwGetCursorPos(mainWindow, &posX, &posY);
    posX = (posX / curScrWidth - 0.5) * 2 * X_MAX * curScrWidth / SCR_WIDTH;
    posY = -(posY / curScrHeight - 0.5) * 2 * Y_MAX * curScrHeight / SCR_HEIGHT;
    glm::vec2 pos(posX, posY);

    if (lmb == GLFW_PRESS) {
        // 按下&按着鼠标左键时
        if (draggingPoint) {
            // 正在拖动，则改变控制点的坐标到鼠标
            draggingPoint->x = posX;
            draggingPoint->y = posY;
        } else {
            // 未在拖动，检查鼠标是否在某个控制点附近
            auto p = std::find_if(points.begin(), points.end(), [&](const glm::vec2 p) {
                return glm::distance(p, pos) <= pointRadius * 1.5;
            });
            // 如果在某个控制点附近点下，则开始拖动
            if (p != points.end()) draggingPoint = p;
        }
    } else if (lmb == GLFW_RELEASE) {
        // 松开鼠标左键时
        if (draggingPoint) draggingPoint = nullptr;
    }
}

// 渲染 ImGui 组件
void renderImGui() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 设置窗口
    {
        ImGui::Begin("Setting");
        ImGui::Text("Control points");

        ImGui::BeginGroup();
        ImGui::SliderFloat2("p1", glm::value_ptr(points[0]), -5.0f, 5.0f);
        ImGui::SliderFloat2("p2", glm::value_ptr(points[1]), -5.0f, 5.0f);
        ImGui::SliderFloat2("p3", glm::value_ptr(points[2]), -5.0f, 5.0f);
        ImGui::SliderFloat2("p4", glm::value_ptr(points[3]), -5.0f, 5.0f);
        ImGui::EndGroup();

        ImGui::Text("Subdivision");
        ImGui::Checkbox("Show", &showSpilt);
        ImGui::SliderFloat("t0", &t0, 0.0f, 1.0f);

        ImGui::End();
    }

    // 输出窗口
    {
        ImGui::Begin("Subdivision Outputs");

        ImGui::Text("C1:");
        for (int i = 0; i < 4; i++) {
            auto p = c1[i];
            ImGui::Text("p%d (%f, %f)", i, p.x, p.y);
        }
        ImGui::Text("C2:");
        for (int i = 0; i < 4; i++) {
            auto p = c2[i];
            ImGui::Text("p%d (%f, %f)", i, p.x, p.y);
        }

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int main() {
    initGLFW();
    initGLAD();
    initImGui();
    initShader();
    init();

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    // 主循环
    while (!glfwWindowShouldClose(mainWindow)) {
        glfwPollEvents();
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderImGui();
        renderUpdate();
        processInput();

        glfwSwapBuffers(mainWindow);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(mainWindow);
    glfwTerminate();

    return 0;
}

void initGLFW() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    mainWindow = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, WIN_TITLE, nullptr, nullptr);

    glfwMakeContextCurrent(mainWindow);
    glfwSetFramebufferSizeCallback(mainWindow, framebufferSizeCallback);
    glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void initGLAD() {
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

void initImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    auto io = ImGui::GetIO();
    ImGui::StyleColorsLight();

    ImGui_ImplGlfw_InitForOpenGL(mainWindow, true);
    ImGui_ImplOpenGL3_Init();
}

void initShader() {
    uint32_t vertShader = loadShader("shaders/vert.glsl", GL_VERTEX_SHADER);
    uint32_t fragShader = loadShader("shaders/frag.glsl", GL_FRAGMENT_SHADER);
    shader.ID = createShaderProgram(vertShader, fragShader);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    curScrWidth = width;
    curScrHeight = height;
}
