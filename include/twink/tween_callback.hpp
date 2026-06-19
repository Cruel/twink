// Copyright (c) 2026 Thomas Edvalson
// SPDX-License-Identifier: MIT

#pragma once

#include <functional>

namespace twink {

class BaseTween;

using TweenCallbackFunction = std::function<void(BaseTween*)>;

class TweenCallback {
public:
    enum Type {
        Begin = 0x01,
        Start = 0x02,
        End = 0x04,
        Complete = 0x08,
        BackBegin = 0x10,
        BackStart = 0x20,
        BackEnd = 0x40,
        BackComplete = 0x80,
        AnyForward = 0x0F,
        AnyBackward = 0xF0,
        Any = 0xFF,

        BEGIN = Begin,
        START = Start,
        END = End,
        COMPLETE = Complete,
        BACK_BEGIN = BackBegin,
        BACK_START = BackStart,
        BACK_END = BackEnd,
        BACK_COMPLETE = BackComplete,
        ANY_FORWARD = AnyForward,
        ANY_BACKWARD = AnyBackward,
        ANY = Any
    };

    virtual ~TweenCallback() = default;
    virtual void onEvent(int type, BaseTween* source) = 0;
};

} // namespace twink
