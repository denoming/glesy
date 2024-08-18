#pragma once

#include "glesy/Api.h"
#include "glesy/NativeWindow.hpp"

#include <functional>

namespace glesy {

class Platform {
public:
    using PrepareFunc = std::function<bool(void* data)>;
    using UpdateFunc = std::function<void(void* data)>;
    using DrawFunc = std::function<void(void* data)>;
    using ShutdownFunc = std::function<void(void* data)>;

    [[nodiscard]] EGLDisplay
    display() const;

    [[nodiscard]] EGLSurface
    surface() const;

    [[nodiscard]] EGLContext
    context() const;

    void
    registerPrepareFunc(PrepareFunc callback);

    void
    registerUpdateFunc(UpdateFunc callback);

    void
    registerDrawFunc(DrawFunc callback);

    void
    registerShutdownFunc(ShutdownFunc callback);

    [[nodiscard]] bool
    initialize(const NativeWindow& window, GLuint flags);

    [[nodiscard]] bool
    process(const NativeWindow& window, void* userData) const;

private:
    [[nodiscard]] bool
    callPrepareFunc(void* userData) const;

    void
    callUpdateFunc(void* userData) const;

    void
    callDrawFunc(void* userData) const;

    void
    callShutdownFunc(void* userData) const;

    void
    swapBuffers() const;

private:
    EGLDisplay _display{};
    EGLSurface _surface{};
    EGLContext _context{};
    PrepareFunc _prepareFunc;
    UpdateFunc _updateFunc;
    DrawFunc _drawFunc;
    ShutdownFunc _shutdownFunc;
};

} // namespace glesy
