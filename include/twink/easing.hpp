// Copyright (c) 2026 Thomas Edvalson
// SPDX-License-Identifier: MIT

#pragma once

namespace twink {

class TweenEquation {
public:
    virtual ~TweenEquation() = default;
    virtual float compute(float t) const = 0;
    virtual const char* toString() const = 0;
};

class TweenPath {
public:
    virtual ~TweenPath() = default;
    virtual float compute(float t, const float* points, int pointsCount) const = 0;
};

class TweenEquations {
public:
    static TweenEquation& easeInQuad;
    static TweenEquation& easeOutQuad;
    static TweenEquation& easeInOutQuad;
    static TweenEquation& easeInOutLinear;
    static TweenEquation& easeInBack;
    static TweenEquation& easeOutBack;
    static TweenEquation& easeInOutBack;
    static TweenEquation& easeInBounce;
    static TweenEquation& easeOutBounce;
    static TweenEquation& easeInOutBounce;
    static TweenEquation& easeInCirc;
    static TweenEquation& easeOutCirc;
    static TweenEquation& easeInOutCirc;
    static TweenEquation& easeInCubic;
    static TweenEquation& easeOutCubic;
    static TweenEquation& easeInOutCubic;
    static TweenEquation& easeInElastic;
    static TweenEquation& easeOutElastic;
    static TweenEquation& easeInOutElastic;
    static TweenEquation& easeInExpo;
    static TweenEquation& easeOutExpo;
    static TweenEquation& easeInOutExpo;
    static TweenEquation& easeInQuart;
    static TweenEquation& easeOutQuart;
    static TweenEquation& easeInOutQuart;
    static TweenEquation& easeInQuint;
    static TweenEquation& easeOutQuint;
    static TweenEquation& easeInOutQuint;
    static TweenEquation& easeInSine;
    static TweenEquation& easeOutSine;
    static TweenEquation& easeInOutSine;
};

class TweenPaths {
public:
    static TweenPath& linear;
    static TweenPath& catmullRom;
    static TweenPath& popOutPath;
};

} // namespace twink
