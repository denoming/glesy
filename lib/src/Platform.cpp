#include "glesy/Platform.hpp"

#include <spdlog/spdlog.h>

#include <string_view>

namespace {

/**
 * Check whether EGL_KHR_create_context extension is supported.
 * If so, return EGL_OPENGL_ES3_BIT_KHR instead of EGL_OPENGL_ES2_BIT
 * @param display The particular display to check for
 * @return
 */
EGLint
getContextRenderType(EGLDisplay display)
{
#ifdef EGL_KHR_create_context
    // Check whether EGL_KHR_create_context is in the extension string
    if (const char* extensions = eglQueryString(display, EGL_EXTENSIONS);
        extensions != nullptr and std::string_view{extensions}.contains("EGL_KHR_create_context")) {
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
Platform::configure(const IWindow& window, const GLuint flags)
{
    if (not setupDisplay()) {
        SPDLOG_ERROR("Unable to get EGL display");
        return false;
    }
    return setup(window.getHandle(), flags);
}

bool
Platform::configure(const IDisplay& display, const IWindow& window, const GLuint flags)
{
    if (not setupDisplay(display.getHandle())) {
        SPDLOG_ERROR("Unable to get EGL display");
        return false;
    }
    return setup(window.getHandle(), flags);
}

bool
Platform::configure(EGLint& width, EGLint& height, GLuint flags)
{
    if (not setupDisplay()) {
        SPDLOG_ERROR("Unable to get EGL display");
        return false;
    }
    return setup(width, height, flags);
}

bool
Platform::configure(const IDisplay& display, EGLint& width, EGLint& height, GLuint flags)
{
    if (not setupDisplay(display.getHandle())) {
        SPDLOG_ERROR("Unable to get EGL display");
        return false;
    }
    return setup(width, height, flags);
}

bool
Platform::process(const IWindow& window, void* userData) const
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

bool
Platform::setup(EGLNativeWindowType window, const GLuint flags)
{
    EGLint majorVersion{}, minorVersion{};
    if (eglInitialize(_display, &majorVersion, &minorVersion) == EGL_FALSE) {
        SPDLOG_ERROR("Unable to initialize EGL: error<{}>", eglGetError());
        return false;
    }

    // clang-format off
    EGLConfig config = chooseConfig({
        EGL_RED_SIZE, 5,
        EGL_GREEN_SIZE, 6,
        EGL_BLUE_SIZE, 5,
        EGL_ALPHA_SIZE, (flags & ES_WINDOW_ALPHA) ? 8 : EGL_DONT_CARE,
        EGL_DEPTH_SIZE, (flags & ES_WINDOW_DEPTH) ? 8 : EGL_DONT_CARE,
        EGL_STENCIL_SIZE, (flags & ES_WINDOW_STENCIL) ? 8 : EGL_DONT_CARE,
        EGL_SAMPLE_BUFFERS, (flags & ES_WINDOW_MULTISAMPLE) ? 1 : 0,
        EGL_RENDERABLE_TYPE, getContextRenderType(_display),
        EGL_NONE
    });
    // clang-format on
    if (not config) {
        SPDLOG_ERROR("Unable to get EGL config");
        return false;
    }

    if (not createWindowSurface(window, config)) {
        SPDLOG_ERROR("Unable to create EGL window surface");
        return false;
    }

    if (not createContext(config)) {
        SPDLOG_ERROR("Unable to set current EGL context");
        return false;
    }

    return true;
}

bool
Platform::setup(EGLint& width, EGLint& height, GLuint flags)
{
    EGLint majorVersion{}, minorVersion{};
    if (eglInitialize(_display, &majorVersion, &minorVersion) == EGL_FALSE) {
        SPDLOG_ERROR("Unable to initialize EGL: error<{}>", eglGetError());
        return false;
    }

    // clang-format off
    EGLConfig config = chooseConfig({
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_RED_SIZE, 5,
        EGL_GREEN_SIZE, 6,
        EGL_BLUE_SIZE, 5,
        EGL_ALPHA_SIZE, (flags & ES_WINDOW_ALPHA) ? 8 : EGL_DONT_CARE,
        EGL_DEPTH_SIZE, (flags & ES_WINDOW_DEPTH) ? 8 : EGL_DONT_CARE,
        EGL_STENCIL_SIZE, (flags & ES_WINDOW_STENCIL) ? 8 : EGL_DONT_CARE,
        EGL_SAMPLE_BUFFERS, (flags & ES_WINDOW_MULTISAMPLE) ? 1 : 0,
        EGL_RENDERABLE_TYPE, getContextRenderType(_display),
        EGL_NONE
    });
    // clang-format on
    if (not config) {
        SPDLOG_ERROR("Unable to get EGL config");
        return false;
    }

    if (not createPbufferSurface(width, height, config)) {
        SPDLOG_ERROR("Unable to create EGL pbuffer surface");
        return false;
    }

    if (not createContext(config)) {
        SPDLOG_ERROR("Unable to set current EGL context");
        return false;
    }

    return true;
}

EGLConfig
Platform::chooseConfig(const std::initializer_list<EGLint> attributes) const
{
    EGLConfig output{};

    EGLint numConfigs{};
    if (eglChooseConfig(_display, std::data(attributes), &output, 1, &numConfigs) == EGL_FALSE) {
        SPDLOG_ERROR("Unable to get EGL config: error<{}>", eglGetError());
        return nullptr;
    }
    if (numConfigs < 1) {
        SPDLOG_ERROR("Invalid number of given EGL configs");
        return nullptr;
    }

    return output;
}

bool
Platform::setupDisplay()
{
    if (_display = eglGetDisplay(EGL_DEFAULT_DISPLAY); _display == EGL_NO_DISPLAY) {
        SPDLOG_ERROR("Unable to get display");
        return false;
    }
    return true;
}

bool
Platform::setupDisplay(EGLNativeDisplayType display)
{
    if (_display = eglGetDisplay(display); _display == EGL_NO_DISPLAY) {
        SPDLOG_ERROR("Unable to get display");
        return false;
    }
    return true;
}

bool
Platform::createWindowSurface(const EGLNativeWindowType window, EGLConfig config)
{
    _surface = eglCreateWindowSurface(_display, config, window, nullptr);
    if (_surface == EGL_NO_SURFACE) {
        SPDLOG_ERROR("Unable to create EGL window surface: error<{}>", eglGetError());
        return false;
    }
    return true;
}

bool
Platform::createPbufferSurface(EGLint& width, EGLint& height, EGLConfig config)
{
    // clang-format off
    const EGLint attribList[] = {
        EGL_WIDTH, width,
        EGL_HEIGHT, height,
        EGL_LARGEST_PBUFFER, EGL_TRUE,
        EGL_NONE
    };
    // clang-format on

    _surface = eglCreatePbufferSurface(_display, config, attribList);
    if (_surface == EGL_NO_SURFACE) {
        SPDLOG_ERROR("Unable to create EGL pbuffer surface: error<{}>", eglGetError());
        return false;
    }

    if (eglQuerySurface(_display, _surface, EGL_WIDTH, &width) != EGL_FALSE) {
        SPDLOG_ERROR("Unable to retrieve pbuffer surface width: error<{}>", eglGetError());
    }
    if (eglQuerySurface(_display, _surface, EGL_HEIGHT, &height) != EGL_FALSE) {
        SPDLOG_ERROR("Unable to retrieve pbuffer surface height: error<{}>", eglGetError());
    }

    return true;
}

bool
Platform::createContext(EGLConfig config)
{
    constexpr EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    _context = eglCreateContext(_display, config, EGL_NO_CONTEXT, contextAttribs);
    if (_context == EGL_NO_CONTEXT) {
        SPDLOG_ERROR("Unable to create EGL context: error<{}>", eglGetError());
        return false;
    }

    if (eglMakeCurrent(_display, _surface, _surface, _context) == EGL_FALSE) {
        SPDLOG_ERROR("Unable to set current EGL context: error<{}>", eglGetError());
        return false;
    }

    return true;
}

bool
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
