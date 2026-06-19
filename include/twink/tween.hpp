// Copyright (c) 2026 Thomas Edvalson
// SPDX-License-Identifier: MIT

#pragma once

#include <memory>
#include <vector>

#include <twink/base_tween.hpp>
#include <twink/easing.hpp>
#include <twink/tween_accessor.hpp>

namespace twink {

class Tween final : public BaseTween {
public:
    static constexpr int ACCESSOR_READ = 0;
    static constexpr int ACCESSOR_WRITE = 1;

    static void setCombinedAttributesLimit(int limit);
    static void setWaypointsLimit(int limit);
    static const char* getVersion();

    static Tween& to(Tweenable& target, int property, float durationSeconds);
    static Tween& from(Tweenable& target, int property, float durationSeconds);
    static Tween& set(Tweenable& target, int property);
    static Tween& call(TweenCallback& callback);
    static Tween& mark();

    template <class T, class Accessor = T>
    static Tween& to(T& target, int property, float durationSeconds, Accessor accessor = Accessor{}) {
        auto wrapper = std::make_unique<AccessorTweenable<T, Accessor>>(target, accessor);
        Tween& tween = to(static_cast<Tweenable&>(*wrapper), property, durationSeconds);
        tween.ownedTarget_ = std::move(wrapper);
        return tween;
    }

    template <class T, class Accessor = T>
    static Tween& from(T& target, int property, float durationSeconds, Accessor accessor = Accessor{}) {
        auto wrapper = std::make_unique<AccessorTweenable<T, Accessor>>(target, accessor);
        Tween& tween = from(static_cast<Tweenable&>(*wrapper), property, durationSeconds);
        tween.ownedTarget_ = std::move(wrapper);
        return tween;
    }

    template <class T, class Accessor = T>
    static Tween& set(T& target, int property, Accessor accessor = Accessor{}) {
        auto wrapper = std::make_unique<AccessorTweenable<T, Accessor>>(target, accessor);
        Tween& tween = set(static_cast<Tweenable&>(*wrapper), property);
        tween.ownedTarget_ = std::move(wrapper);
        return tween;
    }

    Tween();
    ~Tween() override = default;

    int getTweenCount() const override;
    int getTimelineCount() const override;

    Tween& build() override;
    void free() override;

    Tween& ease(TweenEquation& equation);
    Tween& target(float value);
    Tween& target(float value1, float value2);
    Tween& target(float value1, float value2, float value3);
    Tween& target(const float* values, int count);
    Tween& targetRelative(float value);
    Tween& targetRelative(float value1, float value2);
    Tween& targetRelative(float value1, float value2, float value3);
    Tween& targetRelative(const float* values, int count);
    Tween& waypoint(float value);
    Tween& waypoint(float value1, float value2);
    Tween& waypoint(float value1, float value2, float value3);
    Tween& waypoint(const float* values, int count);
    Tween& path(TweenPath& path);

    int getType() const;
    TweenEquation* getEasing() const;
    float* getTargetValues();
    int getCombinedAttributesCount() const;
    Tweenable* getTarget() const;

protected:
    void reset() override;
    void forceStartValues() override;
    void forceEndValues() override;
    void initializeOverride() override;
    void updateOverride(int step, int lastStep, bool isIterationStep, float delta) override;

private:
    static std::unique_ptr<Tween> make();
    void setup(Tweenable* target, int property, float durationSeconds);

    static int combinedAttributesLimit_;
    static int waypointsLimit_;

    Tweenable* targetObject_ = nullptr;
    std::unique_ptr<Tweenable> ownedTarget_;
    int type_ = -1;
    TweenEquation* equation_ = nullptr;
    TweenPath* pathAlgorithm_ = nullptr;
    bool isFrom_ = false;
    bool isRelative_ = false;
    int combinedAttributesCount_ = 0;
    int waypointsCount_ = 0;
    std::vector<float> startValues_;
    std::vector<float> targetValues_;
    std::vector<float> waypoints_;
    std::vector<float> accessorBuffer_;
    std::vector<float> pathBuffer_;
};

} // namespace twink
