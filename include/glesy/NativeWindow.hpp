#pragma once

#include "glesy/Api.h"

namespace glesy {

class NativeWindow {
public:
    virtual ~NativeWindow() = default;

    [[nodiscard]] virtual EGLNativeDisplayType
    getNativeDisplay() const
        = 0;

    [[nodiscard]] virtual EGLNativeWindowType
    getNativeWindow() const
        = 0;

    [[nodiscard]] virtual bool
    awaitClosing() const
        = 0;
};

} // namespace glesy
