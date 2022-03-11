#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <cmath>
#include <vector>
#include <array>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "common.hpp"
#include "line.hpp"
#include "circle.hpp"

// 三次 Bezier 曲线公式
glm::vec2 calculateBezier(const std::array<glm::vec2, 4>& points, float t) {
    return std::pow(1 - t, 3.0f) * points[0] +
           3 * t * std::pow(1 - t, 2.0f) * points[1] +
           3 * (1 - t) * std::pow(t, 2.0f) * points[2] +
           std::pow(t, 3.0f) * points[3];
}

// 渲染 Bezier 曲线
void drawBezier(const std::array<glm::vec2, 4>& points) {
    // 画出控制点并连线
    for (const auto& p : points) {
        renderCircle(p, pointRadius, pointVertCount, pointColor);
    }
    renderLine(std::vector<glm::vec2>{points[0], points[1], points[2], points[3]}, lineColor);

    // 用折线来拟合 Bezier 曲线
    std::vector<glm::vec2> linePoints;
    for (float t = 0.0; t <= 1.0; t += 0.001) {
        // 调用 calculateBezier 来计算这个点的坐标
        linePoints.push_back(calculateBezier(points, t));
    }

    // 渲染折线
    renderLine(linePoints, curveColor);
}

static std::array<glm::vec2, 4> c1, c2;  // 分裂出的两组控制点

// 渲染原本的 Bezier 曲线，计算从 t0 分裂出的两条子 Bezier 曲线和它们的控制点
// 用 showSplit 来控制是否显示子曲线和控制点
void drawSplitBezier(const std::array<glm::vec2, 4>& points, float t0, bool showSplit) {
    shader.depth = 0.0f;
    drawBezier(points);

    // 插值计算出 c1 和 c2
    // lerp(a, b, t) = (1 - t) * a + t * b;
    auto temp = lerp(points[1], points[2], t0);
    c1[0] = points[0];
    c1[1] = lerp(c1[0], points[1], t0);
    c1[2] = lerp(c1[1], temp, t0);
    c2[3] = points[3];
    c2[2] = lerp(points[2], c2[3], t0);
    c2[1] = lerp(temp, c2[2], t0);
    c1[3] = c2[0] = lerp(c1[2], c2[1], t0);

    if (showSplit) {
        // 显示 c1 和 c2 以及它们的控制点
        // 调整深度，避免被原曲线遮住
        shader.depth = 1.0f;

        // 渲染 t0 点
        renderCircle(calculateBezier(points, t0), pointRadius, pointVertCount, t0Color);

        // 渲染各个控制点
        for (const auto& p : c1) renderCircle(p, pointRadius, pointVertCount, lineColor1);
        for (const auto& p : c2) renderCircle(p, pointRadius, pointVertCount, lineColor2);

        // 渲染将控制点连在一起的线段
        renderLine(c1, lineColor1);
        renderLine(c2, lineColor2);
    }
}