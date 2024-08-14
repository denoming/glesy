#pragma once

#include "GlesyExport.hpp"

#include <memory>

namespace glesy {

class GLESY_EXPORT Context {
public:
    Context();

    ~Context();

private:
    std::unique_ptr<class ContextImpl> _impl;
};

} // namespace glesy
