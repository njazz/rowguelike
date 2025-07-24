/// @file pong.hpp
/// Basic pong game

#pragma once

#include "rowguelike.hpp"

using namespace rwe ;

// === Constants ===
constexpr char BALL_CHAR[] = "*";
constexpr char PADDLE_CHAR[] = "|";

// === Globals ===
static EntityId ball;
static EntityId leftPaddle;
static EntityId rightPaddle;

// === Collision Logic ===
static auto collider = COLLIDER_FN
{
    if (TEST_HIT)
        if (receiver == ball && (peer == leftPaddle || peer == rightPaddle)) {
            auto &speed = Engine::get().getSpeed(ball);
            speed.vx = -speed.vx;
        }
};

// === Input Logic (W/S keys) ===
static auto inputhandler = INPUT_FN
{
    auto &pos = Engine::get().getPosition(receiver);
    if (rawInput.up && pos.y > 0)
        pos.y -= 1;
    else if (rawInput.down && pos.y < Setup::ScreenHeight - 1)
        pos.y += 1;
};

// === Ball Timer: Moves the ball, handles bounce and score ===
static auto ball_timer = TIMER_FN
{
    auto &pos = Engine::get().getPosition(receiver);
    auto &speed = Engine::get().getSpeed(receiver);

    // Bounce off top/bottom
    if (pos.y + speed.vy < 0 || pos.y + speed.vy >= Setup::ScreenHeight)
        speed.vy = -speed.vy;

    // Score logic: reset ball if out of bounds
    if (pos.x + speed.vx < 0 || pos.x + speed.vx >= Setup::ScreenWidth) {
        pos.x = Setup::ScreenWidth / 2;
        pos.y = Setup::ScreenHeight / 2;
        speed.vx = -1;
        speed.vy = 1;
        return;
    }

    // Move ball
    pos.x += speed.vx;
    pos.y += speed.vy;
};

// === Right Paddle AI Timer ===
static auto right_timer = TIMER_FN
{
    auto &ballPos = Engine::get().getPosition(ball);
    auto &paddlePos = Engine::get().getPosition(receiver);

    if (ballPos.y < paddlePos.y)
        paddlePos.y -= 1;
    else if (ballPos.y > paddlePos.y)
        paddlePos.y += 1;
};

// === Game Setup ===
void setupPong()
{
    auto background = A::Background().spawn();

    // Ball
    Engine::get()
        .make(Actor::Text | Actor::Move | Actor::Timer | Actor::Collider)
        .text(BALL_CHAR)
        .position(Setup::ScreenWidth / 2, Setup::ScreenHeight / 2)
        .speed(-1, 1)
        .collider(1, collider)
        .timer(5, ball_timer)
        .spawnToId(ball);

    // Left Paddle (player)
    Engine::get()
        .make(Actor::Text | Actor::Input | Actor::Control | Actor::Collider)
        .text(PADDLE_CHAR)
        .position(0, Setup::ScreenHeight / 2)
        .input(inputhandler)
        .collider(1, collider)
        .spawnToId(leftPaddle);

    // Right Paddle (AI)
    Engine::get()
        .make(Actor::Text | Actor::Timer | Actor::Collider)
        .text(PADDLE_CHAR)
        .position(Setup::ScreenWidth - 1, Setup::ScreenHeight / 2)
        .collider(1, collider)
        .timer(3, right_timer)
        .spawnToId(rightPaddle);
}
