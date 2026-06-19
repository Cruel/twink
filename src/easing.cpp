// Copyright (c) 2026 Thomas Edvalson
// SPDX-License-Identifier: MIT

#include <algorithm>
#include <cmath>

#include <twink/easing.hpp>

namespace twink {
namespace {

constexpr float pi = 3.14159265358979323846f;
constexpr float halfPi = 1.57079632679489661923f;
constexpr float twoPi = pi * 2.0f;

class Equation final : public TweenEquation {
public:
    using Fn = float (*)(float);
    Equation(Fn fn, const char* name) : fn_(fn), name_(name) {}
    float compute(float t) const override { return fn_(t); }
    const char* toString() const override { return name_; }

private:
    Fn fn_;
    const char* name_;
};

float linear(float t) { return t; }
float quadIn(float t) { return t * t; }
float quadOut(float t) { return -t * (t - 2.0f); }
float quadInOut(float t) {
    t *= 2.0f;
    if (t < 1.0f) return 0.5f * t * t;
    return -0.5f * ((t - 1.0f) * (t - 3.0f) - 1.0f);
}
float cubicIn(float t) { return t * t * t; }
float cubicOut(float t) { t -= 1.0f; return t * t * t + 1.0f; }
float cubicInOut(float t) {
    t *= 2.0f;
    if (t < 1.0f) return 0.5f * t * t * t;
    t -= 2.0f;
    return 0.5f * (t * t * t + 2.0f);
}
float quartIn(float t) { return t * t * t * t; }
float quartOut(float t) { t -= 1.0f; return -(t * t * t * t - 1.0f); }
float quartInOut(float t) {
    t *= 2.0f;
    if (t < 1.0f) return 0.5f * t * t * t * t;
    t -= 2.0f;
    return -0.5f * (t * t * t * t - 2.0f);
}
float quintIn(float t) { return t * t * t * t * t; }
float quintOut(float t) { t -= 1.0f; return -(t * t * t * t * t - 1.0f); }
float quintInOut(float t) {
    t *= 2.0f;
    if (t < 1.0f) return 0.5f * t * t * t * t * t;
    t -= 2.0f;
    return -0.5f * (t * t * t * t * t - 2.0f);
}
float sineIn(float t) { return -std::cos(t * halfPi) + 1.0f; }
float sineOut(float t) { return std::sin(t * halfPi); }
float sineInOut(float t) { return -0.5f * (std::cos(pi * t) - 1.0f); }
float circIn(float t) { return -std::sqrt(1.0f - t * t) - 1.0f; }
float circOut(float t) { return std::sqrt(1.0f - (t - 1.0f) * (t - 1.0f)); }
float circInOut(float t) {
    t *= 2.0f;
    if (t < 1.0f) return -0.5f * (std::sqrt(1.0f - t * t) - 1.0f);
    t -= 2.0f;
    return 0.5f * (std::sqrt(1.0f - t * t) + 1.0f);
}
float expoIn(float t) { return t == 0.0f ? 0.0f : std::pow(2.0f, 10.0f * (t - 1.0f)); }
float expoOut(float t) { return t == 1.0f ? 1.0f : -std::pow(2.0f, -10.0f * t) + 1.0f; }
float expoInOut(float t) {
    if (t == 0.0f) return 0.0f;
    if (t == 1.0f) return 1.0f;
    t *= 2.0f;
    if (t < 1.0f) return 0.5f * std::pow(2.0f, 10.0f * (t - 1.0f));
    t -= 1.0f;
    return 0.5f * (-std::pow(2.0f, -10.0f * t) + 2.0f);
}
float backIn(float t) { constexpr float s = 1.70158f; return t * t * ((s + 1.0f) * t - s); }
float backOut(float t) { constexpr float s = 1.70158f; t -= 1.0f; return t * t * ((s + 1.0f) * t + s) + 1.0f; }
float backInOut(float t) {
    float s = 1.70158f * 1.525f;
    t *= 2.0f;
    if (t < 1.0f) return 0.5f * (t * t * ((s + 1.0f) * t - s));
    t -= 2.0f;
    return 0.5f * (t * t * ((s + 1.0f) * t + s) + 2.0f);
}
float bounceOut(float t) {
    if (t < 1.0f / 2.75f) return 7.5625f * t * t;
    if (t < 2.0f / 2.75f) { t -= 1.5f / 2.75f; return 7.5625f * t * t + 0.75f; }
    if (t < 2.5f / 2.75f) { t -= 2.25f / 2.75f; return 7.5625f * t * t + 0.9375f; }
    t -= 2.625f / 2.75f;
    return 7.5625f * t * t + 0.984375f;
}
float bounceIn(float t) { return 1.0f - bounceOut(1.0f - t); }
float bounceInOut(float t) {
    if (t < 0.5f) return bounceIn(t * 2.0f) * 0.5f;
    return bounceOut(t * 2.0f - 1.0f) * 0.5f + 0.5f;
}
float elasticIn(float t) {
    if (t == 0.0f) return 0.0f;
    if (t == 1.0f) return 1.0f;
    constexpr float p = 0.3f;
    constexpr float a = 1.0f;
    constexpr float s = p / 4.0f;
    t -= 1.0f;
    return -(a * std::pow(2.0f, 10.0f * t) * std::sin((t - s) * twoPi / p));
}
float elasticOut(float t) {
    if (t == 0.0f) return 0.0f;
    if (t == 1.0f) return 1.0f;
    constexpr float p = 0.3f;
    constexpr float a = 1.0f;
    constexpr float s = p / 4.0f;
    return a * std::pow(2.0f, -10.0f * t) * std::sin((t - s) * twoPi / p) + 1.0f;
}
float elasticInOut(float t) {
    if (t == 0.0f) return 0.0f;
    t *= 2.0f;
    if (t == 2.0f) return 1.0f;
    constexpr float p = 0.3f * 1.5f;
    constexpr float a = 1.0f;
    constexpr float s = p / 4.0f;
    t -= 1.0f;
    if (t < 0.0f) return -0.5f * (a * std::pow(2.0f, 10.0f * t) * std::sin((t - s) * twoPi / p));
    return a * std::pow(2.0f, -10.0f * t) * std::sin((t - s) * twoPi / p) * 0.5f + 1.0f;
}

class LinearPath final : public TweenPath {
public:
    float compute(float t, const float* points, int pointsCount) const override {
        int segment = static_cast<int>(std::floor((pointsCount - 1) * t));
        segment = std::max(0, std::min(segment, pointsCount - 2));
        t = t * (pointsCount - 1) - segment;
        return points[segment] + t * (points[segment + 1] - points[segment]);
    }
};

class CatmullRomPath final : public TweenPath {
public:
    float compute(float t, const float* points, int pointsCount) const override {
        int segment = static_cast<int>(std::floor((pointsCount - 1) * t));
        segment = std::max(0, std::min(segment, pointsCount - 2));
        t = t * (pointsCount - 1) - segment;
        if (segment == 0) return spline(points[0], points[0], points[1], points[2], t);
        if (segment == pointsCount - 2) return spline(points[pointsCount - 3], points[pointsCount - 2], points[pointsCount - 1], points[pointsCount - 1], t);
        return spline(points[segment - 1], points[segment], points[segment + 1], points[segment + 2], t);
    }

private:
    static float spline(float a, float b, float c, float d, float t) {
        const float t1 = (c - a) * 0.5f;
        const float t2 = (d - b) * 0.5f;
        const float h1 = 2.0f * t * t * t - 3.0f * t * t + 1.0f;
        const float h2 = -2.0f * t * t * t + 3.0f * t * t;
        const float h3 = t * t * t - 2.0f * t * t + t;
        const float h4 = t * t * t - t * t;
        return b * h1 + c * h2 + t1 * h3 + t2 * h4;
    }
};

class PopOutPath final : public TweenPath {
public:
    float compute(float t, const float* points, int pointsCount) const override {
        if (pointsCount != 3) return points[pointsCount - 1];
        constexpr float popTime = 1.0f / 6.0f;
        if (t < popTime) return points[0] + t * 6.0f * points[1];
        if (t > 1.0f - popTime) return points[1] + (t - 1.0f + popTime) * 6.0f * (points[2] - points[1]);
        return points[1];
    }
};

Equation easeInQuadImpl(quadIn, "Quad.IN");
Equation easeOutQuadImpl(quadOut, "Quad.OUT");
Equation easeInOutQuadImpl(quadInOut, "Quad.INOUT");
Equation easeInOutLinearImpl(linear, "Linear.INOUT");
Equation easeInBackImpl(backIn, "Back.IN");
Equation easeOutBackImpl(backOut, "Back.OUT");
Equation easeInOutBackImpl(backInOut, "Back.INOUT");
Equation easeInBounceImpl(bounceIn, "Bounce.IN");
Equation easeOutBounceImpl(bounceOut, "Bounce.OUT");
Equation easeInOutBounceImpl(bounceInOut, "Bounce.INOUT");
Equation easeInCircImpl(circIn, "Circ.IN");
Equation easeOutCircImpl(circOut, "Circ.OUT");
Equation easeInOutCircImpl(circInOut, "Circ.INOUT");
Equation easeInCubicImpl(cubicIn, "Cubic.IN");
Equation easeOutCubicImpl(cubicOut, "Cubic.OUT");
Equation easeInOutCubicImpl(cubicInOut, "Cubic.INOUT");
Equation easeInElasticImpl(elasticIn, "Elastic.IN");
Equation easeOutElasticImpl(elasticOut, "Elastic.OUT");
Equation easeInOutElasticImpl(elasticInOut, "Elastic.INOUT");
Equation easeInExpoImpl(expoIn, "Expo.IN");
Equation easeOutExpoImpl(expoOut, "Expo.OUT");
Equation easeInOutExpoImpl(expoInOut, "Expo.INOUT");
Equation easeInQuartImpl(quartIn, "Quart.IN");
Equation easeOutQuartImpl(quartOut, "Quart.OUT");
Equation easeInOutQuartImpl(quartInOut, "Quart.INOUT");
Equation easeInQuintImpl(quintIn, "Quint.IN");
Equation easeOutQuintImpl(quintOut, "Quint.OUT");
Equation easeInOutQuintImpl(quintInOut, "Quint.INOUT");
Equation easeInSineImpl(sineIn, "Sine.IN");
Equation easeOutSineImpl(sineOut, "Sine.OUT");
Equation easeInOutSineImpl(sineInOut, "Sine.INOUT");

LinearPath linearPathImpl;
CatmullRomPath catmullRomPathImpl;
PopOutPath popOutPathImpl;

} // namespace

TweenEquation& TweenEquations::easeInQuad = easeInQuadImpl;
TweenEquation& TweenEquations::easeOutQuad = easeOutQuadImpl;
TweenEquation& TweenEquations::easeInOutQuad = easeInOutQuadImpl;
TweenEquation& TweenEquations::easeInOutLinear = easeInOutLinearImpl;
TweenEquation& TweenEquations::easeInBack = easeInBackImpl;
TweenEquation& TweenEquations::easeOutBack = easeOutBackImpl;
TweenEquation& TweenEquations::easeInOutBack = easeInOutBackImpl;
TweenEquation& TweenEquations::easeInBounce = easeInBounceImpl;
TweenEquation& TweenEquations::easeOutBounce = easeOutBounceImpl;
TweenEquation& TweenEquations::easeInOutBounce = easeInOutBounceImpl;
TweenEquation& TweenEquations::easeInCirc = easeInCircImpl;
TweenEquation& TweenEquations::easeOutCirc = easeOutCircImpl;
TweenEquation& TweenEquations::easeInOutCirc = easeInOutCircImpl;
TweenEquation& TweenEquations::easeInCubic = easeInCubicImpl;
TweenEquation& TweenEquations::easeOutCubic = easeOutCubicImpl;
TweenEquation& TweenEquations::easeInOutCubic = easeInOutCubicImpl;
TweenEquation& TweenEquations::easeInElastic = easeInElasticImpl;
TweenEquation& TweenEquations::easeOutElastic = easeOutElasticImpl;
TweenEquation& TweenEquations::easeInOutElastic = easeInOutElasticImpl;
TweenEquation& TweenEquations::easeInExpo = easeInExpoImpl;
TweenEquation& TweenEquations::easeOutExpo = easeOutExpoImpl;
TweenEquation& TweenEquations::easeInOutExpo = easeInOutExpoImpl;
TweenEquation& TweenEquations::easeInQuart = easeInQuartImpl;
TweenEquation& TweenEquations::easeOutQuart = easeOutQuartImpl;
TweenEquation& TweenEquations::easeInOutQuart = easeInOutQuartImpl;
TweenEquation& TweenEquations::easeInQuint = easeInQuintImpl;
TweenEquation& TweenEquations::easeOutQuint = easeOutQuintImpl;
TweenEquation& TweenEquations::easeInOutQuint = easeInOutQuintImpl;
TweenEquation& TweenEquations::easeInSine = easeInSineImpl;
TweenEquation& TweenEquations::easeOutSine = easeOutSineImpl;
TweenEquation& TweenEquations::easeInOutSine = easeInOutSineImpl;

TweenPath& TweenPaths::linear = linearPathImpl;
TweenPath& TweenPaths::catmullRom = catmullRomPathImpl;
TweenPath& TweenPaths::popOutPath = popOutPathImpl;

} // namespace twink
