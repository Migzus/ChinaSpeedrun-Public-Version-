#pragma once

#include "Mathf.h"

namespace cs {
    struct CameraComponent
    {
        CameraComponent();
        float angle, aspect, close, distant;
        Matrix4x4 proj, view;
        Vector3 forward, up, right;
        Vector3 position;
    };
}