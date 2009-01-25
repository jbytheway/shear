#ifndef SHEAR__AUTOMATA__NFA_HPP
#define SHEAR__AUTOMATA__NFA_HPP

#include <boost/call_traits.hpp>
#include <boost/type_traits/is_unsigned.hpp>
#include <boost/type_traits/make_unsigned.hpp>

#include <shear/core.hpp>

namespace shear { namespace automata {

class nfa {
  public:
    class state;

    class transition {
      public:
        typedef boost::shared_ptr<transition> ptr;

        transition(const boost::shared_ptr<state>& dest): destination_(dest) {}

        virtual ~transition() = 0;

        const boost::shared_ptr<state>& destination() const {
          return destination_;
        }

        virtual bool is_epsilon_transition() const = 0;
        virtual void get_match(size_t&) const = 0;
        virtual bool matches(size_t) const = 0;
      protected:
        boost::shared_ptr<state> destination_;
    };

    class state {
      public:
        typedef boost::shared_ptr<state> ptr;

        virtual ~state() = 0;

        template<typename T>
        transition::ptr add_transition(const ptr& dest, const T match) {
          transition::ptr p(new match_transition<T>(dest, match));
          transitions_.push_back(p);
          return p;
        }

        transition::ptr add_transition(const ptr& dest) {
          transition::ptr p(new epsilon_transition(dest));
          transitions_.push_back(p);
          return p;
        }

        const std::vector<transition::ptr>& transitions() const {
          return transitions_;
        }
      private:
        std::vector<transition::ptr> transitions_;
    };

    nfa(const state::ptr& start_state) : start_state_(start_state) {}

    const state::ptr& start_state() const { return start_state_; }
  private:
    class epsilon_transition : public transition {
      public:
        epsilon_transition(const state::ptr& dest) : transition(dest) {}

        virtual bool is_epsilon_transition() const { return true; }
        virtual void get_match(size_t&) const { abort(); }
        virtual bool matches(size_t) const { return false; }
    };

    template<typename T>
    class match_transition : public transition {
      public:
        match_transition(
            const state::ptr& dest,
            typename boost::call_traits<T>::param_type match
          ) : transition(dest), match_(match)
        {}

        virtual bool is_epsilon_transition() const { return false; }
        virtual void get_match(size_t& m) const { m = match_; }
        virtual bool matches(size_t m) const {
          return matches_impl(m, typename boost::is_unsigned<T>::type());
        }

        bool matches_impl(size_t m, boost::true_type) const {
          return m == match_;
        }
        bool matches_impl(size_t m, boost::false_type) const {
          return match_ >= 0 &&
            m == typename boost::make_unsigned<T>::type(match_);
        }
      private:
        T match_;
    };

    state::ptr start_state_;
};

inline nfa::transition::~transition() {}
inline nfa::state::~state() {}

}}

#endif // SHEAR__AUTOMATA__NFA_HPP

