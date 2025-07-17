#pragma once

#include "glesy/Api.h"

namespace glesy {

class IDisplay {
public:
    virtual ~IDisplay() = default;

    [[nodiscard]] virtual EGLNativeDisplayType
    getHandle() const
        = 0;
};

} // namespace glesy