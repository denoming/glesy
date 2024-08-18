#include "glesy/Platform.hpp"

#include <spdlog/spdlog.h>

#include <cstring>

namespace {

/**
 * Check whether EGL_KHR_create_context extension is supported.
 * If so, return EGL_OPENGL_ES3_BIT_KHR instead of EGL_OPENGL_ES2_BIT
 * @param display The particular display to check for
 * @return
 */
EGLint
getContextRenderType(const EGLDisplay display)
{
#ifdef EGL_KHR_create_context
    // Check whether EGL_KHR_create_context is in the extension string
    if (const char* extensions = eglQueryString(display, EGL_EXTENSIONS);
        extensions != nullptr and strstr(extensions, "EGL_KHR_create_context")) {
        return EGL_OPENGL_ES3_BIT_KHR;
    }
#endif
    // extension is not supported
    return EGL_OPENGL_ES2_BIT;
}

} // namespace

namespace glesy {

EGLDisplay
Platform::display() const
{
    return _display;
}

EGLSurface
Platform::surface() const
{
    return _surface;
}

EGLContext
Platform::context() const
{
    return _context;
}

void
Platform::registerPrepareFunc(PrepareFunc callback)
{
    _prepareFunc = std::move(callback);
}

void
Platform::registerUpdateFunc(UpdateFunc callback)
{
    _updateFunc = std::move(callback);
}

void
Platform::registerDrawFunc(DrawFunc callback)
{
    _drawFunc = std::move(callback);
}

void
Platform::registerShutdownFunc(ShutdownFunc callback)
{
    _shutdownFunc = std::move(callback);
}

bool
Platform::initialize(const NativeWindow& window, GLuint flags)
{
    if (_display = eglGetDisplay(window.getNativeDisplay()); _display == EGL_NO_DISPLAY) {
        SPDLOG_ERROR("Unable to get display");
        return false;
    }

    EGLint majorVersion{};
    EGLint minorVersion{};
    if (not eglInitialize(_display, &majorVersion, &minorVersion)) {
        SPDLOG_ERROR("Unable to initialize EGL");
        return false;
    }

    EGLConfig config;
    {
        const EGLint attribList[] = {EGL_RED_SIZE,
                                     5,
                                     EGL_GREEN_SIZE,
                                     6,
                                     EGL_BLUE_SIZE,
                                     5,
                                     EGL_ALPHA_SIZE,
                                     (flags & ES_WINDOW_ALPHA) ? 8 : EGL_DONT_CARE,
                                     EGL_DEPTH_SIZE,
                                     (flags & ES_WINDOW_DEPTH) ? 8 : EGL_DONT_CARE,
                                     EGL_STENCIL_SIZE,
                                     (flags & ES_WINDOW_STENCIL) ? 8 : EGL_DONT_CARE,
                                     EGL_SAMPLE_BUFFERS,
                                     (flags & ES_WINDOW_MULTISAMPLE) ? 1 : 0,
                                     EGL_RENDERABLE_TYPE,
                                     getContextRenderType(_display),
                                     EGL_NONE};

        EGLint numConfigs = 0;
        if (not eglChooseConfig(_display, attribList, &config, 1, &numConfigs)) {
            SPDLOG_ERROR("Unable to get EGL config");
            return false;
        }
        if (numConfigs < 1) {
            SPDLOG_ERROR("Invalid number of given EGL configs");
            return false;
        }
    }

    // Create a surface
    _surface = eglCreateWindowSurface(_display, config, window.getNativeWindow(), nullptr);
    if (_surface == EGL_NO_SURFACE) {
        SPDLOG_ERROR("Unable to create EGL surface");
        return false;
    }

    // Create a GL context
    constexpr EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    _context = eglCreateContext(_display, config, EGL_NO_CONTEXT, contextAttribs);
    if (_context == EGL_NO_CONTEXT) {
        SPDLOG_ERROR("Unable to create EGL context");
        return false;
    }

    // Make the context current
    if (not eglMakeCurrent(_display, _surface, _surface, _context)) {
        SPDLOG_ERROR("Unable to set current EGL context");
        return false;
    }

    return true;
}

bool
Platform::process(const NativeWindow& window, void* userData) const
{
    if (not callPrepareFunc(userData)) {
        SPDLOG_ERROR("Preparation has failed");
        return false;
    }
    while (not window.awaitClosing()) {
        callUpdateFunc(userData);
        callDrawFunc(userData);
        swapBuffers();
    }
    callShutdownFunc(userData);
    return true;
}

[[nodiscard]] bool
Platform::callPrepareFunc(void* userData) const
{
    bool result{};
    if (_prepareFunc) {
        result = _prepareFunc(userData);
    }
    return result;
}

void
Platform::callUpdateFunc(void* userData) const
{
    if (_updateFunc) {
        _updateFunc(userData);
    }
}

void
Platform::callDrawFunc(void* userData) const
{
    if (_drawFunc) {
        _drawFunc(userData);
    }
}

void
Platform::callShutdownFunc(void* userData) const
{
    if (_shutdownFunc) {
        _shutdownFunc(userData);
    }
}

void
Platform::swapBuffers() const
{
    eglSwapBuffers(_display, _surface);
}

} // namespace glesy
