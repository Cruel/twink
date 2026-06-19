// Copyright (c) 2026 Thomas Edvalson
// SPDX-License-Identifier: MIT

#pragma once

namespace twink {

class Tweenable {
public:
    virtual ~Tweenable() = default;
    virtual int getValues(int property, float* values) = 0;
    virtual void setValues(int property, const float* values) = 0;
};

template <class T>
class TweenAccessor {
public:
    virtual ~TweenAccessor() = default;
    virtual int getValues(T& target, int property, float* values) = 0;
    virtual void setValues(T& target, int property, const float* values) = 0;
};

template <class T, class Accessor>
class AccessorTweenable final : public Tweenable {
public:
    AccessorTweenable(T& target, Accessor accessor = Accessor{})
        : target_(&target), accessor_(accessor) {}

    int getValues(int property, float* values) override {
        return accessor_.getValues(*target_, property, values);
    }

    void setValues(int property, const float* values) override {
        accessor_.setValues(*target_, property, values);
    }

private:
    T* target_;
    Accessor accessor_;
};

} // namespace twink
