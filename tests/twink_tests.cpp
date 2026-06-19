#include <cmath>
#include <cstdlib>
#include <iostream>

#include <twink/twink.hpp>

namespace {

struct Transform {
    float x = 0.0f;
    float y = 0.0f;
};

struct TransformAccessor {
    enum Property { X, Position };

    int getValues(Transform& target, int property, float* values) {
        if (property == X) {
            values[0] = target.x;
            return 1;
        }
        if (property == Position) {
            values[0] = target.x;
            values[1] = target.y;
            return 2;
        }
        return 0;
    }

    void setValues(Transform& target, int property, const float* values) {
        if (property == X) {
            target.x = values[0];
        } else if (property == Position) {
            target.x = values[0];
            target.y = values[1];
        }
    }
};

void require(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "failed: " << message << '\n';
        std::exit(1);
    }
}

void requireNear(float actual, float expected, const char* message) {
    require(std::fabs(actual - expected) < 0.0001f, message);
}

} // namespace

int main() {
    {
        twink::TweenManager manager;
        Transform t;
        twink::Tween::to<Transform, TransformAccessor>(t, TransformAccessor::X, 1.0f)
            .target(10.0f)
            .ease(twink::TweenEquations::easeInOutLinear)
            .start(manager);
        manager.update(0.5f);
        requireNear(t.x, 5.0f, "midpoint after half duration");
        manager.update(0.5f);
        requireNear(t.x, 10.0f, "final value after full duration");
    }

    {
        twink::TweenManager manager;
        Transform t;
        int completeCount = 0;
        twink::Tween::to<Transform, TransformAccessor>(t, TransformAccessor::X, 0.25f)
            .target(1.0f)
            .ease(twink::TweenEquations::easeInOutLinear)
            .setCallback(twink::TweenCallback::Complete, [&completeCount](twink::BaseTween*) { ++completeCount; })
            .start(manager);
        manager.update(1.0f);
        manager.update(0.0f);
        require(completeCount == 1, "complete callback fires once");
        require(manager.size() == 0, "manager removes completed tweens");
    }

    {
        twink::TweenManager manager;
        Transform t;
        twink::Tween::to<Transform, TransformAccessor>(t, TransformAccessor::Position, 1.0f)
            .target(10.0f, 20.0f)
            .ease(twink::TweenEquations::easeInOutLinear)
            .start(manager);
        manager.update(0.5f);
        requireNear(t.x, 5.0f, "multi-value x updates");
        requireNear(t.y, 10.0f, "multi-value y updates");
    }

    {
        twink::TweenManager manager;
        Transform t;
        twink::Tween::to<Transform, TransformAccessor>(t, TransformAccessor::X, 1.0f)
            .target(10.0f)
            .ease(twink::TweenEquations::easeInOutLinear)
            .delay(0.5f)
            .start(manager);
        manager.update(0.25f);
        requireNear(t.x, 0.0f, "delay holds start value");
        manager.update(0.25f);
        manager.update(0.5f);
        requireNear(t.x, 5.0f, "delay then tween progresses");
    }

    {
        twink::TweenManager manager;
        Transform t;
        twink::Tween::to<Transform, TransformAccessor>(t, TransformAccessor::X, 1.0f)
            .target(10.0f)
            .ease(twink::TweenEquations::easeInOutLinear)
            .repeatYoyo(1, 0.0f)
            .start(manager);
        manager.update(1.0f);
        requireNear(t.x, 10.0f, "yoyo reaches end");
        manager.update(0.5f);
        requireNear(t.x, 5.0f, "yoyo returns through midpoint");
        manager.update(0.5f);
        requireNear(t.x, 0.0f, "yoyo returns to start");
    }

    std::cout << "twink tests passed\n";
}
