#include "glesy/X11/Window.hpp"

#include <X11/Xutil.h>

#include <stdexcept>

namespace glesy::x11 {

Window::Window()
{
    if (_display = XOpenDisplay(nullptr); _display == nullptr) {
        throw std::runtime_error("Unable to open X display");
    }
}

Window::Window(std::string_view display)
{
    if (_display = XOpenDisplay(display.data()); _display == nullptr) {
        throw std::runtime_error("Unable to open X display");
    }
}

Window::~Window()
{
    if (_window != None) {
        XDestroyWindow(_display, _window);
    }
    if (_display != nullptr) {
        XCloseDisplay(_display);
    }
}

EGLNativeDisplayType
Window::getNativeDisplay() const
{
    return _display;
}

EGLNativeWindowType
Window::getNativeWindow() const
{
    return _window;
}

void
Window::create(std::string_view title, unsigned int width, unsigned int height)
{
    XSetWindowAttributes swaAttr;
    swaAttr.event_mask = ExposureMask | PointerMotionMask | KeyPressMask;

    _window = XCreateWindow(_display,
                            DefaultRootWindow(_display),
                            0,
                            0,
                            width,
                            height,
                            0,
                            CopyFromParent,
                            InputOutput,
                            CopyFromParent,
                            CWEventMask,
                            &swaAttr);
    _wmDeleteMessage = XInternAtom(_display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(_display, _window, &_wmDeleteMessage, 1);

    XSetWindowAttributes overAttr;
    overAttr.override_redirect = false;
    XChangeWindowAttributes(_display, _window, CWOverrideRedirect, &overAttr);

    XWMHints hints;
    hints.input = true;
    hints.flags = InputHint;
    XSetWMHints(_display, _window, &hints);

    /* Make the window visible on the screen */
    XMapWindow(_display, _window);
    XStoreName(_display, _window, title.data());

    /* Get identifiers for the provided atom name strings */
    _wmState = XInternAtom(_display, "_NET_WM_STATE", false);

    XEvent event{};
    event.type = ClientMessage;
    event.xclient.window = _window;
    event.xclient.message_type = _wmState;
    event.xclient.format = 32;
    event.xclient.data.l[0] = 1;
    event.xclient.data.l[1] = false;
    XSendEvent(_display, DefaultRootWindow(_display), false, SubstructureNotifyMask, &event);
}

void
Window::destroy()
{
    if (_window != None) {
        XDestroyWindow(_display, _window);
        _window = None;
    }
}

bool
Window::awaitClosing() const
{
    bool userInterrupt{};
    XEvent event{};
    while (XPending(_display)) {
        XNextEvent(_display, &event);
        if (event.type == ClientMessage) {
            if (event.xclient.data.l[0] == _wmDeleteMessage) {
                userInterrupt = true;
            }
        }
        if (event.type == DestroyNotify) {
            userInterrupt = true;
        }
    }
    return userInterrupt;
}

} // namespace glesy::x11
