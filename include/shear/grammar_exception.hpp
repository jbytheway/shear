#ifndef SHEAR__GRAMMAR_EXCEPTION_HPP
#define SHEAR__GRAMMAR_EXCEPTION_HPP

#include <shear/runtime/production.hpp>

namespace shear {

class grammar_exception : std::exception {
};

class grammar_loop_exception : grammar_exception {
  public:
    grammar_loop_exception(const runtime::production::ptr&)
    {}
};

}

#endif // SHEAR__GRAMMAR_EXCEPTION_HPP

