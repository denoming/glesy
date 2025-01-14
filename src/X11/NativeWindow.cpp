#include "glesy/X11/NativeWindow.hpp"

#include <X11/Xutil.h>

#include <stdexcept>

namespace glesy::x11 {

NativeWindow::NativeWindow(NativeDisplay& display)
    : _display{display}
{
}

NativeWindow::~NativeWindow()
{
    if (_window != None) {
        XDestroyWindow(_display.getNativeHandle(), _window);
    }
}

EGLNativeWindowType
NativeWindow::getHandle() const
{
    return _window;
}

void
NativeWindow::create(const std::string_view title,
                     const unsigned int width,
                     const unsigned int height)
{
    XSetWindowAttributes swaAttr;
    swaAttr.event_mask = ExposureMask | PointerMotionMask | KeyPressMask;

    Display* const display = _display.getNativeHandle();
    _window = XCreateWindow(display,
                            DefaultRootWindow(display),
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
    _wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, _window, &_wmDeleteMessage, 1);

    XSetWindowAttributes overAttr;
    overAttr.override_redirect = false;
    XChangeWindowAttributes(display, _window, CWOverrideRedirect, &overAttr);

    XWMHints hints;
    hints.input = true;
    hints.flags = InputHint;
    XSetWMHints(display, _window, &hints);

    /* Make the window visible on the screen */
    XMapWindow(display, _window);
    XStoreName(display, _window, title.data());

    /* Get identifiers for the provided atom name strings */
    _wmState = XInternAtom(display, "_NET_WM_STATE", false);

    XEvent event{};
    event.type = ClientMessage;
    event.xclient.window = _window;
    event.xclient.message_type = _wmState;
    event.xclient.format = 32;
    event.xclient.data.l[0] = 1;
    event.xclient.data.l[1] = false;
    XSendEvent(display, DefaultRootWindow(display), false, SubstructureNotifyMask, &event);
}

void
NativeWindow::destroy()
{
    if (_window != None) {
        XDestroyWindow(_display.getNativeHandle(), _window);
        _window = None;
    }
}

bool
NativeWindow::awaitClosing() const
{
    bool userInterrupt{};
    XEvent event{};
    while (XPending(_display.getNativeHandle())) {
        XNextEvent(_display.getNativeHandle(), &event);
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
