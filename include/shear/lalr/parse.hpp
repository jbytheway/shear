#ifndef SHEAR__LALR__PARSE_HPP
#define SHEAR__LALR__PARSE_HPP

#include <shear/syntax_error.hpp>

namespace shear { namespace lalr {

template<typename RootSymbol>
class parse {
  public:
    parse(
        std::vector<syntax_error> syntax_errors,
        boost::shared_ptr<RootSymbol> root = boost::shared_ptr<RootSymbol>()
      )
    {
      assert(root || !syntax_errors.empty());
      swap(syntax_errors_, syntax_errors);
      swap(root_, root);
    }

    operator bool() const { return syntax_errors_.empty(); }
    const std::vector<syntax_error>& syntax_errors() const {
      return syntax_errors_;
    }
    const boost::shared_ptr<RootSymbol>& root() const { return root_; }
  private:
    std::vector<syntax_error> syntax_errors_;
    boost::shared_ptr<RootSymbol> root_;
};

}}

#endif // SHEAR__LALR__PARSE_HPP

