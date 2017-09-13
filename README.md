# interval_tree
## A static C++ 1D interval tree implementation in the style of STL/`std::`

This is a simple implementation of a 1D [centered interval tree](https://en.wikipedia.org/wiki/Interval_tree#Centered_interval_tree), an asymptotically optimal datastructure for supporting intersection tests on a point against a set of intervals or an interval against a set of intervals.

The implementation is simple and should be fairly clear.  It is cross-platform, and compiles have been tested with GCC, Clang, ICC, and MSVC.  It adds itself into the `std::` namespace for easy use (N.B. this is technically undefined behavior; if you don't like it, you can put it into any other namespace; you'll need to accompany it with `using namespace std;`).

Future work:

- Doesn't support dynamically adding or removing intervals from the set.  This is usually unnecessary, and the tree quality after doing it is degraded.
- Doesn't do rebalancing (it is irrelevant for the current, static implementation, but if the dynamic feature is added, it becomes important).
- Tree only supports one dimension.
- Your contributions to implement these (or other) features would be greatly appreciated.