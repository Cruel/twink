// Copyright (c) 2026 Thomas Edvalson
// SPDX-License-Identifier: MIT

#include <cstdlib>

#include <twink/base_tween.hpp>
#include <twink/tween_manager.hpp>

namespace twink {

void BaseTween::reset() {
    step_ = -2;
    repeatCount_ = 0;
    isIterationStep_ = false;
    isYoyoFlag_ = false;
    delayStart_ = 0.0f;
    duration_ = 0.0f;
    repeatDelay_ = 0.0f;
    currentTime_ = 0.0f;
    deltaTime_ = 0.0f;
    isStartedFlag_ = false;
    isInitializedFlag_ = false;
    isFinishedFlag_ = false;
    isKilledFlag_ = false;
    isPausedFlag_ = false;
    callback_ = nullptr;
    callbackTriggers_ = TweenCallback::Complete;
    userData_ = nullptr;
    callbacks_.clear();
    isAutoRemoveEnabled = true;
    isAutoStartEnabled = true;
}

BaseTween& BaseTween::build() { return *this; }

BaseTween& BaseTween::start() {
    build();
    currentTime_ = 0.0f;
    isStartedFlag_ = true;
    return *this;
}

BaseTween& BaseTween::start(TweenManager& manager) {
    manager.add(*this);
    return *this;
}

BaseTween& BaseTween::delay(float seconds) {
    delayStart_ += seconds;
    return *this;
}

void BaseTween::kill() { isKilledFlag_ = true; }
void BaseTween::free() {}
void BaseTween::pause() { isPausedFlag_ = true; }
void BaseTween::resume() { isPausedFlag_ = false; }

BaseTween& BaseTween::repeat(int count, float delaySeconds) {
    if (!isStartedFlag_) {
        repeatCount_ = count;
        repeatDelay_ = delaySeconds >= 0.0f ? delaySeconds : 0.0f;
        isYoyoFlag_ = false;
    }
    return *this;
}

BaseTween& BaseTween::repeatYoyo(int count, float delaySeconds) {
    if (!isStartedFlag_) {
        repeatCount_ = count;
        repeatDelay_ = delaySeconds >= 0.0f ? delaySeconds : 0.0f;
        isYoyoFlag_ = true;
    }
    return *this;
}

BaseTween& BaseTween::setCallback(TweenCallback* callback) {
    callback_ = callback;
    return *this;
}

BaseTween& BaseTween::setCallback(int type, const TweenCallbackFunction& callback) {
    callbacks_[type] = callback;
    return *this;
}

BaseTween& BaseTween::setCallbackTriggers(int flags) {
    callbackTriggers_ = flags;
    return *this;
}

BaseTween& BaseTween::setUserData(void* data) {
    userData_ = data;
    return *this;
}

float BaseTween::getDelay() const { return delayStart_; }
float BaseTween::getDuration() const { return duration_; }
int BaseTween::getRepeatCount() const { return repeatCount_; }
float BaseTween::getRepeatDelay() const { return repeatDelay_; }
float BaseTween::getFullDuration() const {
    if (repeatCount_ < 0) return -1.0f;
    return delayStart_ + duration_ + (repeatDelay_ + duration_) * repeatCount_;
}
void* BaseTween::getUserData() const { return userData_; }
int BaseTween::getStep() const { return step_; }
float BaseTween::getCurrentTime() const { return currentTime_; }
bool BaseTween::isStarted() const { return isStartedFlag_; }
bool BaseTween::isInitialized() const { return isInitializedFlag_; }
bool BaseTween::isFinished() const { return isFinishedFlag_ || isKilledFlag_; }
bool BaseTween::isYoyo() const { return isYoyoFlag_; }
bool BaseTween::isPaused() const { return isPausedFlag_; }

void BaseTween::initializeOverride() {}
void BaseTween::updateOverride(int, int, bool, float) {}

void BaseTween::callCallback(int type) {
    auto it = callbacks_.find(type);
    if (it != callbacks_.end()) it->second(this);
    if (callback_ != nullptr && (callbackTriggers_ & type) != 0) callback_->onEvent(type, this);
}

bool BaseTween::isReverse(int step) const {
    return isYoyoFlag_ && std::abs(step % 4) == 2;
}

bool BaseTween::isValid(int step) const {
    return (step >= 0 && step <= repeatCount_ * 2) || repeatCount_ < 0;
}

void BaseTween::update(float deltaSeconds) {
    if (!isStartedFlag_ || isPausedFlag_ || isKilledFlag_) return;

    deltaTime_ = deltaSeconds;
    if (!isInitializedFlag_) initialize();
    if (isInitializedFlag_) {
        testRelaunch();
        updateStep();
        testCompletion();
    }
    currentTime_ += deltaTime_;
    deltaTime_ = 0.0f;
}

void BaseTween::initialize() {
    if (currentTime_ + deltaTime_ >= delayStart_) {
        initializeOverride();
        isInitializedFlag_ = true;
        isIterationStep_ = true;
        step_ = 0;
        deltaTime_ -= delayStart_ - currentTime_;
        currentTime_ = 0.0f;
        callCallback(TweenCallback::Begin);
        callCallback(TweenCallback::Start);
    }
}

void BaseTween::testRelaunch() {
    if (!isIterationStep_ && repeatCount_ >= 0 && step_ < 0 && currentTime_ + deltaTime_ >= 0.0f) {
        isIterationStep_ = true;
        step_ = 0;
        const float delta = -currentTime_;
        deltaTime_ -= delta;
        currentTime_ = 0.0f;
        callCallback(TweenCallback::Begin);
        callCallback(TweenCallback::Start);
        updateOverride(step_, step_ - 1, isIterationStep_, delta);
    } else if (!isIterationStep_ && repeatCount_ >= 0 && step_ > repeatCount_ * 2 && currentTime_ + deltaTime_ < 0.0f) {
        isIterationStep_ = true;
        step_ = repeatCount_ * 2;
        const float delta = -currentTime_;
        deltaTime_ -= delta;
        currentTime_ = duration_;
        callCallback(TweenCallback::BackBegin);
        callCallback(TweenCallback::BackStart);
        updateOverride(step_, step_ + 1, isIterationStep_, delta);
    }
}

void BaseTween::updateStep() {
    while (isValid(step_)) {
        if (!isIterationStep_ && currentTime_ + deltaTime_ <= 0.0f) {
            isIterationStep_ = true;
            step_ -= 1;
            const float delta = -currentTime_;
            deltaTime_ -= delta;
            currentTime_ = duration_;
            if (isReverse(step_)) forceStartValues();
            else forceEndValues();
            callCallback(TweenCallback::BackStart);
            updateOverride(step_, step_ + 1, isIterationStep_, delta);
        } else if (!isIterationStep_ && currentTime_ + deltaTime_ >= repeatDelay_) {
            isIterationStep_ = true;
            step_ += 1;
            const float delta = repeatDelay_ - currentTime_;
            deltaTime_ -= delta;
            currentTime_ = 0.0f;
            if (isReverse(step_)) forceEndValues();
            else forceStartValues();
            callCallback(TweenCallback::Start);
            updateOverride(step_, step_ - 1, isIterationStep_, delta);
        } else if (isIterationStep_ && currentTime_ + deltaTime_ < 0.0f) {
            isIterationStep_ = false;
            step_ -= 1;
            const float delta = -currentTime_;
            deltaTime_ -= delta;
            currentTime_ = 0.0f;
            updateOverride(step_, step_ + 1, isIterationStep_, delta);
            callCallback(TweenCallback::BackEnd);
            if (step_ < 0 && repeatCount_ >= 0) callCallback(TweenCallback::BackComplete);
            else currentTime_ = repeatDelay_;
        } else if (isIterationStep_ && currentTime_ + deltaTime_ > duration_) {
            isIterationStep_ = false;
            step_ += 1;
            const float delta = duration_ - currentTime_;
            deltaTime_ -= delta;
            currentTime_ = duration_;
            updateOverride(step_, step_ - 1, isIterationStep_, delta);
            callCallback(TweenCallback::End);
            if (step_ > repeatCount_ * 2 && repeatCount_ >= 0) callCallback(TweenCallback::Complete);
            currentTime_ = 0.0f;
        } else if (isIterationStep_) {
            const float delta = deltaTime_;
            deltaTime_ -= delta;
            currentTime_ += delta;
            updateOverride(step_, step_, isIterationStep_, delta);
            break;
        } else {
            const float delta = deltaTime_;
            deltaTime_ -= delta;
            currentTime_ += delta;
            break;
        }
    }
}

void BaseTween::testCompletion() {
    isFinishedFlag_ = repeatCount_ >= 0 && (step_ > repeatCount_ * 2 || step_ < 0);
}

} // namespace twink
