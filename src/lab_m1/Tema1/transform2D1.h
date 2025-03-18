#pragma once

#include "utils/glm_utils.h"

namespace transform2D1
{
    // Translate matrix
    inline glm::mat3 Translate(float translateX, float translateY)
    {
        return glm::mat3(
            1, 0, 0,
            0, 1, 0,
            translateX, translateY, 1
        );
    }

    // Scale matrix
    inline glm::mat3 Scale(float scaleX, float scaleY)
    {
        return glm::mat3(
            scaleX, 0, 0,
            0, scaleY, 0,
            0, 0, 1
        );
    }

    // Rotate matrix
    inline glm::mat3 Rotate(float radians)
    {
        float cosTheta = cos(radians);
        float sinTheta = sin(radians);

        return glm::mat3(
            cosTheta, -sinTheta, 0,
            sinTheta, cosTheta, 0,
            0, 0, 1
        );
    }

    // Shear matrix
    inline glm::mat3 ShearY(float shearFactor) {
        return glm::mat3(
            1, shearFactor, 0,
            0, 1,          0,
            0, 0,          1
        );
    }
}   // namespace m1
