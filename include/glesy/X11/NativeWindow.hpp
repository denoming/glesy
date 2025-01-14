#pragma once

#include "glesy/IWindow.hpp"
#include <glesy/X11/NativeDisplay.hpp>

#include <X11/Xlib.h>

#include <string_view>

namespace glesy::x11 {

class NativeWindow final : public IWindow {
public:
    explicit NativeWindow(NativeDisplay& display);

    ~NativeWindow() override;

    explicit NativeWindow(std::string_view display);

    [[nodiscard]] EGLNativeWindowType
    getHandle() const override;

    void
    create(std::string_view title, unsigned int width, unsigned int height);

    void
    destroy();

    [[nodiscard]] bool
    awaitClosing() const override;

private:
    NativeDisplay& _display;
    ::Window _window{};
    Atom _wmDeleteMessage{};
    Atom _wmState{};
};

} // namespace glesy::x11
