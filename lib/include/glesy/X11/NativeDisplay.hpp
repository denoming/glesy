#pragma once

#include "glesy/IDisplay.hpp"

#include <X11/Xlib.h>

#include <string_view>

namespace glesy::x11 {

class NativeDisplay final : public IDisplay {
public:
    NativeDisplay();

    explicit NativeDisplay(std::string_view display);

    ~NativeDisplay() override;

    [[nodiscard]] Display*
    getNativeHandle() const;

    [[nodiscard]] EGLNativeDisplayType
    getHandle() const override;

private:
    Display* _display{};
};

} // namespace glesy::x11