# twink

`twink` is a dependency-free C++17 tweening library for user-defined object properties. It provides a small tween manager, easing functions, callbacks, delays, repeats, and yoyo playback without depending on any game engine, renderer, UI toolkit, or asset system.

Users keep ownership of their domain types and define accessors that read and write float properties.

```cpp
#include <twink/twink.hpp>

struct Transform {
    float x = 0.0f;
    float y = 0.0f;
};

struct TransformAccessor {
    enum Property { Position };

    int getValues(Transform& target, int property, float* values) {
        if (property == Position) {
            values[0] = target.x;
            values[1] = target.y;
            return 2;
        }
        return 0;
    }

    void setValues(Transform& target, int property, const float* values) {
        if (property == Position) {
            target.x = values[0];
            target.y = values[1];
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
    manager.update(0.25f);
}
```

Objects can also implement `twink::Tweenable` directly, which preserves the old three-argument factory shape:

```cpp
twink::Tween::to(object, PropertyId, 0.5f).target(1.0f).start(manager);
```

## CMake

Using `FetchContent` directly from GitHub:

```cmake
include(FetchContent)

FetchContent_Declare(
    twink
    GIT_REPOSITORY https://github.com/Cruel/twink.git
    GIT_TAG main
)
FetchContent_MakeAvailable(twink)

target_link_libraries(my_app PRIVATE twink::twink)
```

Using `add_subdirectory`:

```cmake
add_subdirectory(twink)
target_link_libraries(my_app PRIVATE twink::twink)
```

Using an installed package:

```cmake
find_package(twink CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE twink::twink)
```

Options:

- `TWINK_BUILD_TESTS`
- `TWINK_BUILD_EXAMPLES`
- `TWINK_INSTALL`

## Status

Initial standalone port from NovelTea TweenEngine. Preserved pieces include basic tweens, `from`/`to`/`set`/`mark`, callbacks, delay, repeat, yoyo, pause/resume/kill, multi-value properties, easing equations, and path/waypoint support. The NovelTea clone did not contain a concrete Timeline implementation, so `timeline.hpp` is a placeholder for a future port.
