#pragma once

#include "glesy/NativeWindow.hpp"

#include <X11/Xlib.h>

#include <string_view>

namespace glesy::x11 {

class Window final : public NativeWindow {
public:
    Window();

    ~Window() override;

    explicit Window(std::string_view display);

    [[nodiscard]] EGLNativeDisplayType
    getNativeDisplay() const override;

    [[nodiscard]] EGLNativeWindowType
    getNativeWindow() const override;

    void
    create(std::string_view title, unsigned int width, unsigned int height);

    void
    destroy();

    [[nodiscard]] bool
    awaitClosing() const override;

private:
    ::Display* _display{};
    ::Window _window{};
    Atom _wmDeleteMessage{};
    Atom _wmState{};
};

} // namespace glesy::x11
