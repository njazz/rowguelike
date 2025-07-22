#pragma once

#include "rowguelike.hpp"

using namespace rwe ;

// === Constants ===
constexpr int8_t SCREEN_WIDTH = Setup::ScreenWidth;
constexpr int8_t SCREEN_HEIGHT = Setup::ScreenHeight;

// === Forward declarations ===
void SpawnEnemy();
void ShootBullet(const EntityId &receiver, const RawInput &input);

// === Setup Entry Point ===
void setupHorizontalSpace()
{
    // Background
    A::Background().spawn();

    // Spawn the player on the left at middle row (row 0)
    auto player = A::PlayerChar(">")
                      .position(0, 0)
                      .control()
                      .inputHandler(INPUTHANDLER_FN { ShootBullet(receiver, rawInput); })
                      .hitpoints(1)
                      .spawn();

    // Spawn enemies repeatedly with a timer
    RWE.make()
        .timer(20, TIMER_FN { SpawnEnemy(); })
        .spawn();
}

// === Input handler for player shooting ===
void ShootBullet(const EntityId &receiver, const RawInput &input)
{
    if (!input.select) return;

    const auto &pos = RWE.getPosition(receiver);

    // Spawn a bullet going to the right
    RWE.make()
        .text("-")
        .position(pos.x + 1, pos.y)
        .speed(1, 0) // move right
        .collider(
            2,
            COLLIDER_FN {
                if (TEST_HIT) {
                    RWE.remove(receiver); // destroy bullet
                    RWE.remove(peer);     // destroy enemy
                }
            })
        .timer(
            1,
            TIMER_FN {
                // Cleanup off-screen bullet
                auto &p = RWE.getPosition(receiver);
                if (p.x >= (SCREEN_WIDTH - 1))
                    RWE.remove(receiver);
            })
        .spawn();
}

// === Spawns an enemy moving left on a random row ===
void SpawnEnemy()
{
    // Alternate rows to simulate random pattern
    static bool toggle = false;
    toggle = !toggle;
    int8_t y = toggle ? 0 : 1;

    RWE.make()
        .text("@")
        .position(SCREEN_WIDTH - 1, y)
        .speed(-1, 0) // move left
        .collider(
            1,
            COLLIDER_FN {
                if (TEST_HIT) {
                    RWE.remove(receiver); // destroy enemy
                    RWE.remove(peer);     // destroy bullet
                }
            })
        .timer(
            1,
            TIMER_FN {
                // Despawn if off screen
                auto &p = RWE.getPosition(receiver);
                if (p.x < 1) {
                    RWE.remove(receiver);

                    //
                    RWE = Engine();

                    RWE.make() //
                        .text("   Game Over    ", "                ")
                        .timer(
                            20,
                            TIMER_FN {
                                RWE = Engine();
                                setupHorizontalSpace();
                            })
                        .spawn();
                }
            })
        .spawn();
}
