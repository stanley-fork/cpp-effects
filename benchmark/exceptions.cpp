// C++ Effects library
// Maciej Pirog, Huawei Edinburgh Research Centre, maciej.pirog@huawei.com
// License: MIT

// Benchmark: Compare native exceptions with exceptions via effect handlers

#include <chrono>
#include <functional>
#include <iostream>

#include "cpp-effects/cpp-effects.h"
#include "cpp-effects/clause-modifiers.h"

using namespace CppEffects;

volatile int64_t sum = 0;
volatile int64_t esum = 0;

volatile int64_t INC = 1;

volatile int64_t i;
volatile int64_t mod;

// ------
// Native
// ------

void testNative(int max, int mod_)
{
  mod = mod_;
  for (i = 0; i < max; i += INC)
  {
    try {
      if (i % mod == 0) { throw 0; }
      sum++;
    } catch (int) {
      esum++;
    }
  }
}

// --------
// Handlers
// --------

struct Error : Command<void> { };

class Catch : public Handler<void, void, Error> {
  void ReturnClause () override { }
  void CommandClause(Error, std::unique_ptr<Resumption<void, void>>) override { esum++; }
};

void testHandlers(int max, int mod_)
{
  mod = mod_;
  for (i = 0; i < max; i += INC)
  {
    OneShot::Handle<Catch>([](){
      if (i % mod == 0) { OneShot::InvokeCmd(Error{}); }
      sum++;
    });
  }
}

// -----------------
// NoResume handlers
// -----------------

class CatchNR : public Handler<void, void, NoResume<Error>> {
  void ReturnClause () override { }
  void CommandClause(Error) override { esum++; }
};

void testHandlersNR(int max, int mod_)
{
  mod = mod_;
  for (i = 0; i < max; i += INC)
  {
    OneShot::Handle<CatchNR>([](){
      if (i % mod == 0) { OneShot::InvokeCmd(Error{}); }
      sum++;
    });
  }
}

// ----
// Main
// ----

int main()
{
std::cout << "--- exception handler + throw exception ---" << std::endl;

std::cout << "native:       " << std::flush;

auto begin = std::chrono::high_resolution_clock::now();
testNative(100000, 1);
auto end = std::chrono::high_resolution_clock::now();
std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count() << "ns" << std::endl;

std::cout << "handlers:     " << std::flush;

auto begin2 = std::chrono::high_resolution_clock::now();
testHandlers(100000, 1);
auto end2 = std::chrono::high_resolution_clock::now();
std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(end2-begin2).count() << "ns" << std::endl;

std::cout << "handlers-n-r: " << std::flush;

auto begin3 = std::chrono::high_resolution_clock::now();
testHandlersNR(100000, 1);
auto end3 = std::chrono::high_resolution_clock::now();
std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(end3-begin3).count() << "ns" << std::endl;
}