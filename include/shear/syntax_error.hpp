#ifndef SHEAR__SYNTAX_ERROR_HPP
#define SHEAR__SYNTAX_ERROR_HPP

#include <ostream>

#include <shear/location.hpp>

namespace shear {

class syntax_error {
  public:
    syntax_error(const location& l, std::string m) :
      location_(l)
    {
      swap(m, message_);
    }

    location loc() const { return location_; }
    const std::string& message() const { return message_; }
  private:
    location location_;
    std::string message_;
};

inline std::ostream& operator<<(std::ostream& o, const syntax_error& e) {
  o << e.loc() << ": " << e.message();
  return o;
}

}

#endif // SHEAR__SYNTAX_ERROR_HPP

