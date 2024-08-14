#include "glesy/Context.hpp"

#include "ContextImpl.hpp"

namespace glesy {

Context::Context()
    : _impl{std::make_unique<ContextImpl>()}
{
}

Context::~Context() = default;

} // namespace glesy
