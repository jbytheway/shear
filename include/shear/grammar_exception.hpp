#ifndef SHEAR__GRAMMAR_EXCEPTION_HPP
#define SHEAR__GRAMMAR_EXCEPTION_HPP

#include <shear/runtime/production.hpp>
#include <shear/runtime/non_terminal.hpp>

namespace shear {

class grammar_exception : public std::exception {
  public:
    virtual const char* what() const throw() {
      return "grammar exception";
    }
};

class grammar_loop_exception : public grammar_exception {
  public:
    grammar_loop_exception(const runtime::production::ptr&)
    {}

    virtual const char* what() const throw() {
      return "loop in grammar";
    }
};

class non_productive_non_terminals_exception : public grammar_exception {
  public:
    template<typename Range>
    non_productive_non_terminals_exception(const Range& r)
    {
      message_ = "the following non-terminals are non-productive "
        "(no sequence of productions expands them to tokens):\n";
      BOOST_FOREACH(const runtime::non_terminal& nt, r) {
        message_ += nt.human_readable_name() + " ";
      }
    }

    ~non_productive_non_terminals_exception() throw() {}

    virtual const char* what() const throw() {
      return message_.c_str();
    }
  private:
    std::string message_;
};

class non_reachable_non_terminals_exception : public grammar_exception {
  public:
    template<typename Range>
    non_reachable_non_terminals_exception(const Range&)
    {}

    virtual const char* what() const throw() {
      return "some non-terminals are non-reachable (they cannot be reached by any sequence of productions from the root non-terminal)";
    }
};

}

#endif // SHEAR__GRAMMAR_EXCEPTION_HPP

