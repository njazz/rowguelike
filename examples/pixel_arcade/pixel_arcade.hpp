/// @file pages.hpp
/// Simple text pages example
///
/// Made with Andreï

#include "rowguelike.hpp"

#include <stdio.h>

using namespace rwe;

uint8_t symbol[8][8] = {
    {// 0
     0b00000,
     0b00000,
     0b00000,
     0b00000,
     0b00000,
     0b00000,
     0b00000,
     0b11111},
    {// 1
     0b00100,
     0b01110,
     0b10101,
     0b01010,
     0b00100,
     0b01010,
     0b01010,
     0b11111},
    {// 2
     0b00100,
     0b01110,
     0b10101,
     0b01110,
     0b00100,
     0b01010,
     0b10001,
     0b00000},
    {// 3
     0b00000,
     0b00000,
     0b00000,
     0b00001,
     0b00001,
     0b00001,
     0b00001,
     0b11111},
    {// 4
     0b00000,
     0b00000,
     0b00000,
     0b00010,
     0b00010,
     0b00010,
     0b00010,
     0b11111},
    {// 5
     0b00000,
     0b00000,
     0b00000,
     0b00100,
     0b00100,
     0b00100,
     0b00100,
     0b11111},
    {// 6
     0b00000,
     0b00000,
     0b00000,
     0b01000,
     0b01000,
     0b01000,
     0b01000,
     0b11111},
    {// 7
     0b00000,
     0b00000,
     0b00000,
     0b10000,
     0b10000,
     0b10000,
     0b10000,
     0b11111},
};

static bool playerJump{false};
static uint8_t playerCounter{0};

static uint16_t barrierCounter{0};

static uint8_t score{0};

void pageGame(Engine &page)
{
    score = 0;
    barrierCounter = 0;

    page.drawContext.clearAll();
    page.drawContext.disableDirectBufferDraw = true;

    RWE.drawContext.clearAll();

    if (1) {
        for (int i = 0; i < 8; i++)
            page.drawContext.defineChar(i, CustomCharacter{symbol[i]});

        // background & barrier
        page.make() //
            .text("")
            .position(0, 0)
            .eachFrame(TIMER_FN {
                // RWE.drawContext.clearAll();

                for (auto x = 0; x < 16; x++) {
                    for (auto y = 1; y < 2; y++) {
                        auto index = 0;

                        auto posX = (Setup::ScreenWidth - 1) - barrierCounter / 5;
                        if (x == posX) {
                            index = 3;
                            index += barrierCounter % 5;
                        }

                        RWE.drawContext.addChar(x, y, index);
                        // RWE.drawContext.addText(x, y, "_");

                        auto playerId = RWE.getIdByTag(1);
                        if (playerId.has_value()) {
                            auto &p = RWE.getPosition(playerId.value());
                            if (p.x == posX && p.y == 1) {
                                RWE.drawContext.disableDirectBufferDraw = false;
                                SWITCH_PAGE(2);
                            }
                            if (p.x == posX && p.y == 0 && index == 5) {
                                score++;
                            }
                        }
                    }
                }

                barrierCounter++;
                if (barrierCounter >= Setup::ScreenWidth * 5)
                    barrierCounter = 0;
                // score++;
            })
            .spawn();

        // player
        page.make() //
            .text("")
            .position(2, 1)
            .tag(1)
            .eachFrame(TIMER_FN {
                auto &p = RWE.getPosition(receiver);
                RWE.drawContext.addChar(p.x, p.y, playerJump ? 2 : 1);

                playerCounter++;
                if (playerCounter > 20) {
                    playerCounter = 0;
                    playerJump = false;
                    RWE.drawContext.addText(p.x, p.y, " ");
                    p.y = 1;
                }
            })
            .input(INPUT_FN {
                auto &p = RWE.getPosition(receiver);

                if (rawInput.up) {
                    RWE.drawContext.addText(p.x, p.y, " ");

                    p.y = 0;

                    playerJump = true;
                    playerCounter = 0;
                }

                if (rawInput.down) {
                    RWE.drawContext.addText(p.x, p.y, " ");

                    p.y = 1;
                }

                if (rawInput.right) {
                    RWE.drawContext.addText(p.x, p.y, " ");

                    if (p.x < 8)
                        p.x++;
                }

                if (rawInput.left) {
                    RWE.drawContext.addText(p.x, p.y, " ");

                    if (p.x > 1)
                        p.x--;
                }
            })
            .spawn();

        // score
        page.make()
            .timer(
                3,
                TIMER_FN {
                    const auto u8_to_str3 = +[](uint8_t value, char out[4]) {
                        snprintf(out, 4, "%03u", value);
                    };

                    static char buf[4];
                    u8_to_str3(score, buf);

                    RWE.drawContext.addText(0, 0, buf);
                })
            .spawn();
    };
}

void setupPixelArcade()
{
    // not supported:
    if (RWE.drawContext.customCharacters == 0) {
        RWE.make().text(" Not supported   ").spawn();
        return;
    }

    SET_PAGE_(0, {
        page.make()
            .text(" Press SELECT ", "   to play")
            .input(INPUT_FN {
                if (rawInput.select) {
                    SWITCH_PAGE(1);
                }
            })
            .spawn();
    });

    SET_PAGE_(1, { pageGame(page); });

    SET_PAGE_(2, {
        static char buf[12];
        for (int i = 0; i < 12; i++)
            buf[i] = ' ';

        {
            snprintf(buf, 11, "score: %03u", score);
        }
        page.make().text(" Game over ", buf).timer(25, TIMER_FN { SWITCH_PAGE(0); }).spawn();
    });

    SWITCH_PAGE(0);
}
