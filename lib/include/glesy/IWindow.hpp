#pragma once

#include "glesy/Api.h"

namespace glesy {

class IWindow {
public:
    virtual ~IWindow() = default;

    [[nodiscard]] virtual EGLNativeWindowType
    getHandle() const
        = 0;

    [[nodiscard]] virtual bool
    awaitClosing() const
        = 0;
};

} // namespace glesy
