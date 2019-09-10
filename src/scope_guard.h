#ifndef SCOPE_GUARD_H
#define SCOPE_GUARD_H

#include <functional>

namespace helpers {
    // A simple scope guard implementation: a function gets executed once the guard object is out of scope
    class scope_guard final{
        public:
        using Callback = std::function<void()>;

        scope_guard(const Callback &fn)
        : fn_{fn}{
        }

        ~scope_guard(){
            fn_();
        }

        private:
        scope_guard() = delete;
        scope_guard(scope_guard &&) = delete;
        scope_guard &operator =(scope_guard &&) = delete;
        scope_guard(const scope_guard &) = delete;
        scope_guard &operator =(const scope_guard &) = delete;

        const Callback &fn_;
    };
}

#endif