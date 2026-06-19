// Copyright (c) 2026 Thomas Edvalson
// SPDX-License-Identifier: MIT

#pragma once

#include <memory>
#include <vector>

#include <twink/base_tween.hpp>

namespace twink {

class TweenManager {
public:
    TweenManager();
    TweenManager(const TweenManager&) = delete;
    TweenManager& operator=(const TweenManager&) = delete;
    TweenManager(TweenManager&&) noexcept = default;
    TweenManager& operator=(TweenManager&&) noexcept = default;

    static void setAutoRemove(BaseTween& object, bool value);
    static void setAutoStart(BaseTween& object, bool value);

    TweenManager& add(BaseTween& object);
    TweenManager& add(std::unique_ptr<BaseTween> object);
    void killAll();
    void ensureCapacity(int minCapacity);
    void pause();
    void resume();
    void update(float deltaSeconds);
    int size() const;

    int getRunningTweensCount() const;
    int getRunningTimelinesCount() const;
    const std::vector<BaseTween*>& getObjects() const;

private:
    std::vector<BaseTween*> objects_;
    std::vector<std::unique_ptr<BaseTween>> ownedObjects_;
    bool isPaused_ = false;
};

} // namespace twink
