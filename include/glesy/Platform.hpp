#pragma once

#include "glesy/Api.h"
#include "glesy/IWindow.hpp"
#include "glesy/IDisplay.hpp"

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
    initialize(const IWindow& window, GLuint flags);

    [[nodiscard]] bool
    initialize(const IDisplay& display, const IWindow& window, GLuint flags);

    [[nodiscard]] bool
    process(const IWindow& window, void* userData) const;

private:
    [[nodiscard]] bool
    initialize(EGLNativeWindowType window, GLuint flags);

    [[nodiscard]] EGLConfig
    chooseConfig(GLuint flags) const;

    [[nodiscard]] bool
    setupDisplay();

    [[nodiscard]] bool
    setupDisplay(EGLNativeDisplayType display);

    [[nodiscard]] bool
    createSurface(EGLNativeWindowType window, EGLConfig config);

    [[nodiscard]] bool
    createContext(EGLConfig config);

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
