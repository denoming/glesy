#include "glesy/X11/NativeDisplay.hpp"

#include <stdexcept>

namespace glesy::x11 {

NativeDisplay::NativeDisplay()
{
    if (_display = XOpenDisplay(nullptr); _display == nullptr) {
        throw std::runtime_error("Unable to open X display");
    }
}

NativeDisplay::NativeDisplay(std::string_view display)
{
    if (_display = XOpenDisplay(display.data()); _display == nullptr) {
        throw std::runtime_error("Unable to open X display");
    }
}

NativeDisplay::~NativeDisplay()
{
    if (_display != nullptr) {
        XCloseDisplay(_display);
    }
}

Display*
NativeDisplay::getNativeHandle() const
{
    return _display;
}

EGLNativeDisplayType
NativeDisplay::getHandle() const
{
    return _display;
}

} // namespace glesy::x11