#include "rowguelike.hpp"

using namespace rwe ;

void setupPages()
{
    SET_PAGE_(0, {
        page.make() //
            .text("First Page     >")
            .input(INPUT_FN {
                if (rawInput.right)
                    SWITCH_PAGE_(1);
            })
            .spawn();
    });

    SET_PAGE_(1, {
        page.make() //
            .text("< Second Page")
            .input(INPUT_FN {
                if (rawInput.left)
                    SWITCH_PAGE_(0);
            })
            .spawn();
    });

    SWITCH_PAGE_(0);
}
