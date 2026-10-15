// unordered_lockless_map: entries deleted and created again reuse the freed slots (FreeData links them into the empty list through
// empty_list::m_Value, which is m_Next/m_Count - a struct there once left a garbage link and the next creations asserted), and deleting
// an entry clears its own bit only (the bit array's mask is 64 bit).
//   from this folder: cl /std:c++20 /EHsc /I.. /I../../.. xcontainer_unordered_lockless_map_test.cpp && xcontainer_unordered_lockless_map_test.exe
#include <stdexcept>                     // xcontainer_small_vector.h uses std::out_of_range
#include "source/xcontainer.h"

#include <cstdio>
#include <cstdlib>

#define CHECK(X) do { if (!(X)) { std::printf("FAILED line %d: %s\n", __LINE__, #X); std::exit(1); } } while (0)

int main()
{
    xcontainer::unordered_lockless_map<std::uint64_t, int> Map;
    Map.Initialize(256);

    for (std::uint64_t k = 1; k <= 100; ++k) Map.Insert(k, [&](int& V) { V = int(k); });
    CHECK(Map.size() == 100);

    // Delete a run that covers bit positions above 31 of a word, then create more than were deleted
    for (std::uint64_t k = 20; k <= 60; ++k) CHECK(Map.FindForDelete(k, [](int&) {}));
    CHECK(Map.size() == 59);
    for (std::uint64_t k = 1000; k < 1100; ++k) Map.Insert(k, [&](int& V) { V = int(k); });
    CHECK(Map.size() == 159);

    // Every entry that should be there is, with its value; the deleted ones are not
    for (std::uint64_t k = 1; k <= 100; ++k)
    {
        int  Got    = -1;
        bool bFound = Map.FindAsReadOnly(k, [&](const int& V) { Got = V; });
        CHECK(bFound == (k < 20 || k > 60));
        if (bFound) CHECK(Got == int(k));
    }
    for (std::uint64_t k = 1000; k < 1100; ++k)
    {
        int Got = -1;
        CHECK(Map.FindAsReadOnly(k, [&](const int& V) { Got = V; }));
        CHECK(Got == int(k));
    }

    std::printf("ok\n");
    return 0;
}
