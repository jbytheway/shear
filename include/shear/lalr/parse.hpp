#ifndef SHEAR__LALR__PARSE_HPP
#define SHEAR__LALR__PARSE_HPP

namespace shear { namespace lalr {

template<typename RootSymbol>
class parse {
  public:
    parse(const boost::shared_ptr<RootSymbol>& root) :
      root_(root)
    {}

    operator bool() const { return root_; }
    const boost::shared_ptr<RootSymbol>& root() const { return root_; }
  private:
    boost::shared_ptr<RootSymbol> root_;
};

}}

#endif // SHEAR__LALR__PARSE_HPP

