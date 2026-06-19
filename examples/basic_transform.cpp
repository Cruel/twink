#include <iostream>

#include <twink/twink.hpp>

struct Transform {
    float x = 0.0f;
    float y = 0.0f;
    float rotation = 0.0f;
};

struct TransformAccessor {
    enum Property {
        Position,
        Rotation
    };

    int getValues(Transform& target, int property, float* values) {
        switch (property) {
        case Position:
            values[0] = target.x;
            values[1] = target.y;
            return 2;
        case Rotation:
            values[0] = target.rotation;
            return 1;
        default:
            return 0;
        }
    }

    void setValues(Transform& target, int property, const float* values) {
        switch (property) {
        case Position:
            target.x = values[0];
            target.y = values[1];
            break;
        case Rotation:
            target.rotation = values[0];
            break;
        default:
            break;
        }
    }
};

int main() {
    twink::TweenManager manager;
    Transform transform;

    twink::Tween::to<Transform, TransformAccessor>(transform, TransformAccessor::Position, 0.5f)
        .target(100.0f, 200.0f)
        .ease(twink::TweenEquations::easeInOutLinear)
        .start(manager);

    manager.update(0.25f);
    std::cout << transform.x << ", " << transform.y << '\n';
    manager.update(0.25f);
    std::cout << transform.x << ", " << transform.y << '\n';
}
