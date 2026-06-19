// Copyright (c) 2026 Thomas Edvalson
// SPDX-License-Identifier: MIT

#include <algorithm>

#include <twink/tween_manager.hpp>

namespace twink {

TweenManager::TweenManager() {
    objects_.reserve(20);
    ownedObjects_.reserve(20);
}

void TweenManager::setAutoRemove(BaseTween& object, bool value) { object.isAutoRemoveEnabled = value; }
void TweenManager::setAutoStart(BaseTween& object, bool value) { object.isAutoStartEnabled = value; }

TweenManager& TweenManager::add(BaseTween& object) {
    if (std::find(objects_.begin(), objects_.end(), &object) == objects_.end()) objects_.push_back(&object);
    if (object.isAutoStartEnabled) object.start();
    return *this;
}

TweenManager& TweenManager::add(std::unique_ptr<BaseTween> object) {
    BaseTween& ref = *object;
    ownedObjects_.push_back(std::move(object));
    return add(ref);
}

void TweenManager::killAll() {
    for (BaseTween* object : objects_) object->kill();
}

void TweenManager::ensureCapacity(int minCapacity) {
    objects_.reserve(static_cast<std::size_t>(minCapacity));
    ownedObjects_.reserve(static_cast<std::size_t>(minCapacity));
}

void TweenManager::pause() { isPaused_ = true; }
void TweenManager::resume() { isPaused_ = false; }

void TweenManager::update(float deltaSeconds) {
    objects_.erase(
        std::remove_if(objects_.begin(), objects_.end(), [this](BaseTween* object) {
            if (!object->isFinished() || !object->isAutoRemoveEnabled) return false;
            const bool isOwned = std::any_of(ownedObjects_.begin(), ownedObjects_.end(), [object](const auto& owned) {
                return owned.get() == object;
            });
            if (!isOwned) object->free();
            return true;
        }),
        objects_.end());
    ownedObjects_.erase(std::remove_if(ownedObjects_.begin(), ownedObjects_.end(), [](const auto& object) {
                            return object->isFinished() && object->isAutoRemoveEnabled;
                        }),
                        ownedObjects_.end());

    if (isPaused_) return;

    if (deltaSeconds >= 0.0f) {
        for (BaseTween* object : objects_) object->update(deltaSeconds);
    } else {
        for (auto it = objects_.rbegin(); it != objects_.rend(); ++it) (*it)->update(deltaSeconds);
    }
}

int TweenManager::size() const { return static_cast<int>(objects_.size()); }

int TweenManager::getRunningTweensCount() const {
    int count = 0;
    for (const BaseTween* object : objects_) count += object->getTweenCount();
    return count;
}

int TweenManager::getRunningTimelinesCount() const {
    int count = 0;
    for (const BaseTween* object : objects_) count += object->getTimelineCount();
    return count;
}

const std::vector<BaseTween*>& TweenManager::getObjects() const { return objects_; }

} // namespace twink
