// C++ Effects library
// Maciej Pirog, Huawei Edinburgh Research Centre, maciej.pirog@huawei.com
// License: MIT

// Example: Exception handlers via effect handlers

#include <functional>
#include <iostream>
#include <vector>

#include "cpp-effects/cpp-effects.h"

using namespace CppEffects;

// ------------------------
// Exceptions and a handler
// ------------------------

// There is actually one exception, called Error. We define one
// handler, which handles an exception by returning a value specified
// in the constructor.

struct Error : Command<void> { };

template <typename T>
[[noreturn]] T error()
{
  OneShot::InvokeCmd(Error{});
  exit(-1); // This will never happen!
}

template <typename T>
class WithDefault : public Handler<T, T, Error> {
public:
  WithDefault(const T& t) : defaultVal(t) { }
private:
  const T defaultVal;
  T CommandClause(Error, std::unique_ptr<Resumption<void, T>>) override
  {
    return defaultVal;
  }
  T ReturnClause(T a) override
  {
    return a;
  }
};

// ------------------
// Particular example
// ------------------

// We multiply numbers in a vector, but if any of the terms is 0, we
// can break the loop using an exception, as we know that the product
// is 0 as well.

int product(const std::vector<int>& v)
{
  return OneShot::HandleWith(
    [&v]() {
      int r = 1;
      for (auto i : v) { 
        if (i == 0) { error<void>(); }
        r *= i;
      }
      return r;
    },
    std::make_shared<WithDefault<int>>(0));
}

int main()
{
  std::cout << product({1, 2, 3, 4, 5}) << std::endl;
  std::cout << product({1, 2, 0, 4, 5}) << std::endl;

  // Output:
  // 120
  // 0
}
