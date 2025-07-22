#pragma once

#include "rowguelike.hpp"
#include <assert.h>
#include <stdlib.h>

using namespace rwe ;

// constexpr int8_t kGridWidth = 16;
// constexpr int8_t kGridHeight = 2;

constexpr int8_t maxElements{32};

bool isOutOfBounds(int8_t x, int8_t y)
{
    if (x < 0)
        return true;
    if (y < 0)
        return true;
    if (x >= Setup::ScreenWidth)
        return true;
    if (y >= Setup::ScreenHeight)
        return true;

    return false;
}

// Forward declarations
Optional<EntityId> spawnFood();
void snakeCollider(const EntityId &receiver, const EntityId peer);
void snakeTimer(const EntityId &receiver);

// -----

struct Snake
{
    EntityId segments[maxElements]; // head is segments[0]
    uint8_t segmentsSize{0};
    Optional<EntityId> food{0};

    void grow() {
        auto &engine = Engine::get();
        EntityId last = segments[segmentsSize - 1];
        auto &pos = engine.getPosition(last);
        auto newSegment = engine.make(/*Actor::Move |*/ /*Actor::Control*/)
                              .text("o")
                              .position(pos.x, pos.y)
                              .speed(0, 0, true)
                              // .collider(1, snakeCollider)
                              .hitpoints(1)
                              .spawn();
        if (newSegment.has_value()) {
            segments[segmentsSize] = (newSegment.value());
            segmentsSize++;
            assert(segmentsSize < maxElements);
        }
    }

    void moveSegments() {
        auto &engine = Engine::get();

        // Move tail segments to follow the one before them
        for (int i = (int) segmentsSize - 1; i > 0; --i) {
            auto &currPos = engine.getPosition(segments[i]);
            auto &prevPos = engine.getPosition(segments[i - 1]);
            currPos.x = prevPos.x;
            currPos.y = prevPos.y;
        }

        // Move head by its speed
        auto &headSpeed = engine.getSpeed(segments[0]);
        auto &headPos = engine.getPosition(segments[0]);
        headPos.x += headSpeed.vx;
        headPos.y += headSpeed.vy;

        // Check out of bounds
        if (isOutOfBounds(headPos.x, headPos.y))
            reset();
    }

    void reset() {
        auto &engine = Engine::get();

        segmentsSize = 0;

        engine = Engine();

        SharedData::Element e;
        e.ptr = this;
        RWE.sharedData.setElement(0, e);

        A::Background().spawn();

        spawnInitial();

        RWE.make(Actor::Timer).timer(2, snakeTimer).spawn();
    }

    void spawnInitial() {
        auto &engine = Engine::get();

        // Spawn snake head
        auto head = engine
                        .make(/*Actor::Move |*/ Actor::Collider | /*Actor::Control |*/ Actor::Health)
                        .text("@")
                        .position(Setup::ScreenWidth / 2, Setup::ScreenHeight / 2)
                        .speed(1, 0, true) // moving right
                        .collider(1, snakeCollider)
                        .inputHandler(NonInvertingControl)
                        .hitpoints(1)
                        .spawn();

        if (head.has_value()) {
            segments[segmentsSize] = head.value();
            segmentsSize++;
            assert(segmentsSize < maxElements);
        }

        // Spawn tail segments
        for (int i = 1; i <= 2; ++i) {
            auto tail = engine.make(/*Actor::Move |*/ Actor::Collider | Actor::Health)
                            .text("o")
                            .position(Setup::ScreenWidth / 2 - i, Setup::ScreenHeight / 2)
                            // .speed(0, 0)
                            .collider(1, snakeCollider)
                            .hitpoints(1)
                            .spawn();

            if (tail.has_value()) {
                segments[segmentsSize] = tail.value();
                segmentsSize++;
                assert(segmentsSize < maxElements);
            }
        }

        // Spawn food
        auto maybeFood = spawnFood();
        if (maybeFood.has_value()) {
            food = maybeFood.value();
        }
    }
};

// Snake snake; // global instance

void snakeCollider(const EntityId &receiver, const EntityId peer)
{
    Snake *ptr = static_cast<Snake *>(RWE.sharedData.getElement(0).ptr);

    auto &engine = Engine::get();
    auto &receiverPos = engine.getPosition(receiver);
    auto &peerPos = engine.getPosition(peer);

    if (receiver == peer)
        return; // ignore self

    if (TEST_HIT) {
        // If snake head hits food
        auto &peerCollider = engine.getCollider(peer);
        if (peerCollider.value == 2) { // food
            // Grow snake
            ptr->grow();

            if (ptr->food.has_value())
                RWE.remove(ptr->food.value());

            ptr->food = spawnFood();
            return;
        }

        // If snake head hits its own body or out of bounds, reset game
        for (auto segment : ptr->segments) {
            if (segment == receiver)
                continue;
            if (peer == segment) {
                ptr->reset();
                return;
            }
        }
    }

    if (isOutOfBounds(receiverPos.x, receiverPos.y)) {
        ptr->reset();
        return;
    }
}

Optional<EntityId> spawnFood()
{
    auto &engine = Engine::get();

    int8_t fx, fy;
    // Naive spawn - no check if overlapping snake for brevity
    fx = rand() % Setup::ScreenWidth;
    fy = rand() % Setup::ScreenHeight;

    return engine.make(Actor::Text | Actor::Collider)
        .text("*")
        .position(fx, fy)
        .collider(2, COLLIDER_FN{}) // empty collider
        .spawn();
}

// The timer function will run every frame to move the snake segments
void snakeTimer(const EntityId &receiver)
{
    // Retrieve snake instance from sharedData.ptr in element 0
    Snake *ptr = static_cast<Snake *>(RWE.sharedData.getElement(0).ptr);
    if (ptr) {
        ptr->moveSegments();
    }
}

// -----

void setupExampleSnake(){
    static Snake snake{};
    snake.reset();
}
