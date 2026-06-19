// Copyright (c) 2026 Thomas Edvalson
// SPDX-License-Identifier: MIT

#pragma once

#include <map>

#include <twink/tween_callback.hpp>

namespace twink {

class TweenManager;

class BaseTween {
public:
    virtual ~BaseTween() = default;

    virtual int getTweenCount() const = 0;
    virtual int getTimelineCount() const = 0;

    virtual BaseTween& build();
    BaseTween& start();
    BaseTween& start(TweenManager& manager);
    BaseTween& delay(float seconds);
    void kill();
    virtual void free();
    void pause();
    void resume();
    BaseTween& repeat(int count, float delaySeconds);
    BaseTween& repeatYoyo(int count, float delaySeconds);
    BaseTween& setCallback(TweenCallback* callback);
    BaseTween& setCallback(int type, const TweenCallbackFunction& callback);
    BaseTween& setCallbackTriggers(int flags);
    BaseTween& setUserData(void* data);

    float getDelay() const;
    float getDuration() const;
    int getRepeatCount() const;
    float getRepeatDelay() const;
    float getFullDuration() const;
    void* getUserData() const;
    int getStep() const;
    float getCurrentTime() const;
    bool isStarted() const;
    bool isInitialized() const;
    bool isFinished() const;
    bool isYoyo() const;
    bool isPaused() const;

    void update(float deltaSeconds);

    bool isAutoRemoveEnabled = true;
    bool isAutoStartEnabled = true;

protected:
    virtual void reset();
    virtual void forceStartValues() = 0;
    virtual void forceEndValues() = 0;
    virtual void initializeOverride();
    virtual void updateOverride(int step, int lastStep, bool isIterationStep, float delta);

    void callCallback(int type);
    bool isReverse(int step) const;
    bool isValid(int step) const;

    float delayStart_ = 0.0f;
    float duration_ = 0.0f;

private:
    void initialize();
    void testRelaunch();
    void updateStep();
    void testCompletion();

    int step_ = -2;
    int repeatCount_ = 0;
    bool isIterationStep_ = false;
    bool isYoyoFlag_ = false;

    float repeatDelay_ = 0.0f;
    float currentTime_ = 0.0f;
    float deltaTime_ = 0.0f;
    bool isStartedFlag_ = false;
    bool isInitializedFlag_ = false;
    bool isFinishedFlag_ = false;
    bool isKilledFlag_ = false;
    bool isPausedFlag_ = false;

    TweenCallback* callback_ = nullptr;
    int callbackTriggers_ = TweenCallback::Complete;
    void* userData_ = nullptr;
    std::map<int, TweenCallbackFunction> callbacks_;
};

} // namespace twink
