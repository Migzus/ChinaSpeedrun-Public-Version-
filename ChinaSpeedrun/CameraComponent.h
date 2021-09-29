#pragma once

#include "Mathf.h"

namespace cs
{
    enum class ProjectionView
    {
        PERSPECTIVE,
        ORTHOGRAPHIC
    };

    struct CameraComponent
    {
        float angle, aspect, close, distant;
        ProjectionView projection;
        Vector3 forward, up, right;
        Vector3 position;
        Matrix4x4 proj, view;

        CameraComponent();
    };
}