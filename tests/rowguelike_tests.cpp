#include "rowguelike.hpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace rwe ;

#define TEST_ASSERT(expr) \
    do { \
        if (!(expr)) { \
            std::cerr << "Test failed: " << #expr << "\n" \
                      << "  File: " << __FILE__ << "\n" \
                      << "  Line: " << __LINE__ << "\n"; \
            std::exit(EXIT_FAILURE); \
        } else \
            std::cout << "[ OK ] Line: " << __LINE__ << " Code: '" << #expr << "'\n"; \
    } while (0)

// -----

int main()
{
    puts("tests started");

    //
    printf("Actors count: %hhu\n", Setup::Actors);
    printf("Components size: %lu\n", sizeof(Components));
    printf("Engine size: %lu\n", sizeof(RWE));

    puts("");

    // Engine
    TEST_ASSERT(RWE.canSpawn());
    auto b = RWE.make(Actor::Move | Actor::Control);
    b.position(3, 1);
    b.text("*");
    b.spawn();

    // TEST_ASSERT(b.entity == 0);
    TEST_ASSERT(RWE.getPosition(0).x == 3);
    TEST_ASSERT(RWE.getPosition(0).y == 1);
    TEST_ASSERT(strncmp(RWE.getText(0).line[0], "*", 1) == 0);

    auto b1 = RWE.make(Actor::Input);
    b1.inputHandler(+[](const EntityId &, const RawInput &) {});
    b1.spawn();

    RWE.runLoop();

    puts("");
    puts("tests completed");
}
