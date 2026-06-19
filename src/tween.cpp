// Copyright (c) 2026 Thomas Edvalson
// SPDX-License-Identifier: MIT

#include <algorithm>
#include <memory>

#include <twink/tween.hpp>

namespace twink {

int Tween::combinedAttributesLimit_ = 3;
int Tween::waypointsLimit_ = 1;

void Tween::setCombinedAttributesLimit(int limit) {
    combinedAttributesLimit_ = std::max(1, limit);
}

void Tween::setWaypointsLimit(int limit) {
    waypointsLimit_ = std::max(0, limit);
}

const char* Tween::getVersion() { return "0.1.0"; }

std::unique_ptr<Tween> Tween::make() {
    auto tween = std::unique_ptr<Tween>(new Tween());
    tween->reset();
    return tween;
}

Tween& Tween::to(Tweenable& target, int property, float durationSeconds) {
    std::unique_ptr<Tween> tween = make();
    tween->setup(&target, property, durationSeconds);
    tween->ease(TweenEquations::easeInOutQuad);
    tween->path(TweenPaths::catmullRom);
    Tween& ref = *tween;
    tween.release();
    return ref;
}

Tween& Tween::from(Tweenable& target, int property, float durationSeconds) {
    Tween& tween = to(target, property, durationSeconds);
    tween.isFrom_ = true;
    return tween;
}

Tween& Tween::set(Tweenable& target, int property) {
    Tween& tween = to(target, property, 0.0f);
    return tween;
}

Tween& Tween::call(TweenCallback& callback) {
    std::unique_ptr<Tween> tween = make();
    tween->setup(nullptr, -1, 0.0f);
    tween->setCallback(&callback);
    tween->setCallbackTriggers(TweenCallback::Start);
    Tween& ref = *tween;
    tween.release();
    return ref;
}

Tween& Tween::mark() {
    std::unique_ptr<Tween> tween = make();
    tween->setup(nullptr, -1, 0.0f);
    Tween& ref = *tween;
    tween.release();
    return ref;
}

Tween::Tween()
    : startValues_(combinedAttributesLimit_),
      targetValues_(combinedAttributesLimit_),
      waypoints_(waypointsLimit_ * combinedAttributesLimit_),
      accessorBuffer_(combinedAttributesLimit_),
      pathBuffer_((2 + waypointsLimit_) * combinedAttributesLimit_) {}

void Tween::setup(Tweenable* target, int property, float durationSeconds) {
    targetObject_ = target;
    type_ = property;
    duration_ = durationSeconds;
}

void Tween::reset() {
    BaseTween::reset();
    equation_ = nullptr;
    pathAlgorithm_ = nullptr;
    isFrom_ = false;
    isRelative_ = false;
    combinedAttributesCount_ = 0;
    waypointsCount_ = 0;
    targetObject_ = nullptr;
    ownedTarget_.reset();
    type_ = -1;
    startValues_.assign(static_cast<std::size_t>(combinedAttributesLimit_), 0.0f);
    targetValues_.assign(static_cast<std::size_t>(combinedAttributesLimit_), 0.0f);
    waypoints_.assign(static_cast<std::size_t>(waypointsLimit_ * combinedAttributesLimit_), 0.0f);
    accessorBuffer_.assign(static_cast<std::size_t>(combinedAttributesLimit_), 0.0f);
    pathBuffer_.assign(static_cast<std::size_t>((2 + waypointsLimit_) * combinedAttributesLimit_), 0.0f);
}

int Tween::getTweenCount() const { return 1; }
int Tween::getTimelineCount() const { return 0; }

Tween& Tween::build() {
    if (targetObject_ != nullptr) combinedAttributesCount_ = targetObject_->getValues(type_, accessorBuffer_.data());
    combinedAttributesCount_ = std::min(combinedAttributesCount_, combinedAttributesLimit_);
    return *this;
}

void Tween::free() {
    delete this;
}

Tween& Tween::ease(TweenEquation& equation) {
    equation_ = &equation;
    return *this;
}

Tween& Tween::target(float value) {
    targetValues_[0] = value;
    return *this;
}

Tween& Tween::target(float value1, float value2) {
    targetValues_[0] = value1;
    targetValues_[1] = value2;
    return *this;
}

Tween& Tween::target(float value1, float value2, float value3) {
    targetValues_[0] = value1;
    targetValues_[1] = value2;
    targetValues_[2] = value3;
    return *this;
}

Tween& Tween::target(const float* values, int count) {
    for (int i = 0; i < count && i < combinedAttributesLimit_; ++i) targetValues_[static_cast<std::size_t>(i)] = values[i];
    return *this;
}

Tween& Tween::targetRelative(float value) {
    isRelative_ = true;
    targetValues_[0] = isInitialized() ? value + startValues_[0] : value;
    return *this;
}

Tween& Tween::targetRelative(float value1, float value2) {
    isRelative_ = true;
    targetValues_[0] = isInitialized() ? value1 + startValues_[0] : value1;
    targetValues_[1] = isInitialized() ? value2 + startValues_[1] : value2;
    return *this;
}

Tween& Tween::targetRelative(float value1, float value2, float value3) {
    isRelative_ = true;
    targetValues_[0] = isInitialized() ? value1 + startValues_[0] : value1;
    targetValues_[1] = isInitialized() ? value2 + startValues_[1] : value2;
    targetValues_[2] = isInitialized() ? value3 + startValues_[2] : value3;
    return *this;
}

Tween& Tween::targetRelative(const float* values, int count) {
    isRelative_ = true;
    for (int i = 0; i < count && i < combinedAttributesLimit_; ++i) {
        targetValues_[static_cast<std::size_t>(i)] = isInitialized() ? values[i] + startValues_[static_cast<std::size_t>(i)] : values[i];
    }
    return *this;
}

Tween& Tween::waypoint(float value) {
    if (waypointsCount_ < waypointsLimit_) {
        waypoints_[static_cast<std::size_t>(waypointsCount_)] = value;
        ++waypointsCount_;
    }
    return *this;
}

Tween& Tween::waypoint(float value1, float value2) {
    if (waypointsCount_ < waypointsLimit_) {
        waypoints_[static_cast<std::size_t>(waypointsCount_ * 2)] = value1;
        waypoints_[static_cast<std::size_t>(waypointsCount_ * 2 + 1)] = value2;
        ++waypointsCount_;
    }
    return *this;
}

Tween& Tween::waypoint(float value1, float value2, float value3) {
    if (waypointsCount_ < waypointsLimit_) {
        waypoints_[static_cast<std::size_t>(waypointsCount_ * 3)] = value1;
        waypoints_[static_cast<std::size_t>(waypointsCount_ * 3 + 1)] = value2;
        waypoints_[static_cast<std::size_t>(waypointsCount_ * 3 + 2)] = value3;
        ++waypointsCount_;
    }
    return *this;
}

Tween& Tween::waypoint(const float* values, int count) {
    if (waypointsCount_ < waypointsLimit_) {
        for (int i = 0; i < count && i < combinedAttributesLimit_; ++i) {
            waypoints_[static_cast<std::size_t>(waypointsCount_ * count + i)] = values[i];
        }
        ++waypointsCount_;
    }
    return *this;
}

Tween& Tween::path(TweenPath& path) {
    pathAlgorithm_ = &path;
    return *this;
}

int Tween::getType() const { return type_; }
TweenEquation* Tween::getEasing() const { return equation_; }
float* Tween::getTargetValues() { return targetValues_.data(); }
int Tween::getCombinedAttributesCount() const { return combinedAttributesCount_; }
Tweenable* Tween::getTarget() const { return targetObject_; }

void Tween::initializeOverride() {
    if (targetObject_ == nullptr) return;

    targetObject_->getValues(type_, startValues_.data());
    for (int i = 0; i < combinedAttributesCount_; ++i) {
        const auto index = static_cast<std::size_t>(i);
        targetValues_[index] += isRelative_ ? startValues_[index] : 0.0f;
        for (int ii = 0; ii < waypointsCount_; ++ii) {
            waypoints_[static_cast<std::size_t>(ii * combinedAttributesCount_ + i)] += isRelative_ ? startValues_[index] : 0.0f;
        }
        if (isFrom_) std::swap(startValues_[index], targetValues_[index]);
    }
}

void Tween::updateOverride(int step, int lastStep, bool isIterationStep, float delta) {
    if (equation_ == nullptr || targetObject_ == nullptr) return;

    if (!isIterationStep && step > lastStep) {
        targetObject_->setValues(type_, isReverse(lastStep) ? startValues_.data() : targetValues_.data());
        return;
    }

    if (!isIterationStep && step < lastStep) {
        targetObject_->setValues(type_, isReverse(lastStep) ? targetValues_.data() : startValues_.data());
        return;
    }

    if (duration_ < 0.00000000001f && delta > -0.00000000001f) {
        targetObject_->setValues(type_, isReverse(step) ? targetValues_.data() : startValues_.data());
        return;
    }

    if (duration_ < 0.00000000001f && delta < 0.00000000001f) {
        targetObject_->setValues(type_, isReverse(step) ? startValues_.data() : targetValues_.data());
        return;
    }

    const float time = isReverse(step) ? duration_ - getCurrentTime() : getCurrentTime();
    const float t = equation_->compute(time / duration_);

    if (waypointsCount_ == 0 || pathAlgorithm_ == nullptr) {
        for (int i = 0; i < combinedAttributesCount_; ++i) {
            const auto index = static_cast<std::size_t>(i);
            accessorBuffer_[index] = startValues_[index] + t * (targetValues_[index] - startValues_[index]);
        }
    } else {
        for (int i = 0; i < combinedAttributesCount_; ++i) {
            pathBuffer_[0] = startValues_[static_cast<std::size_t>(i)];
            pathBuffer_[static_cast<std::size_t>(1 + waypointsCount_)] = targetValues_[static_cast<std::size_t>(i)];
            for (int ii = 0; ii < waypointsCount_; ++ii) {
                pathBuffer_[static_cast<std::size_t>(ii + 1)] = waypoints_[static_cast<std::size_t>(ii * combinedAttributesCount_ + i)];
            }
            accessorBuffer_[static_cast<std::size_t>(i)] = pathAlgorithm_->compute(t, pathBuffer_.data(), waypointsCount_ + 2);
        }
    }

    targetObject_->setValues(type_, accessorBuffer_.data());
}

void Tween::forceStartValues() {
    if (targetObject_ != nullptr) targetObject_->setValues(type_, startValues_.data());
}

void Tween::forceEndValues() {
    if (targetObject_ != nullptr) targetObject_->setValues(type_, targetValues_.data());
}

} // namespace twink
