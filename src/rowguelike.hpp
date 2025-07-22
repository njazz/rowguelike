#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

namespace rwe {

// ------------------------------------------------------------------------------

#ifndef RW_SETUP_SCREEN_WIDTH
#define RW_SETUP_SCREEN_WIDTH 16
#endif

#ifndef RW_SETUP_SCREEN_HEIGHT
#define RW_SETUP_SCREEN_HEIGHT 2
#endif

#ifndef RW_SETUP_ACTORS
#define RW_SETUP_ACTORS 64
#endif

#ifndef RW_SETUP_TAGS
#define RW_SETUP_TAGS 32
#endif

#ifndef RW_SETUP_SHARED_NUMBERS
#define RW_SETUP_SHARED_NUMBERS 8
#endif

#ifndef RW_SETUP_SHARED_STRINGS
#define RW_SETUP_SHARED_STRINGS 4
#endif

#ifndef RW_SETUP_MAX_VIEWPORT_SCALE
#define RW_SETUP_MAX_VIEWPORT_SCALE 0
#endif

#ifndef RW_HIGH_RESOLUTION_POSITION
#define RW_HIGH_RESOLUTION_POSITION false
#endif

// ------------------------------------------------------------------------------

struct Setup {
    static constexpr uint8_t ScreenWidth { RW_SETUP_SCREEN_WIDTH };
    static constexpr uint8_t ScreenHeight { RW_SETUP_SCREEN_HEIGHT };

    static constexpr uint8_t Actors { RW_SETUP_ACTORS };
    static constexpr uint8_t Tags { RW_SETUP_TAGS };
    static constexpr uint8_t SharedNumbers { RW_SETUP_SHARED_NUMBERS };
    static constexpr uint8_t SharedStrings { RW_SETUP_SHARED_STRINGS };

    // Bit offset for higher resolution
    // NB: for position using int8_t this is valid in range 0..3
    // for high-resolution position mode it's 0..9
    static constexpr uint8_t MaxViewportScale { RW_SETUP_MAX_VIEWPORT_SCALE };
};

// ------------------------------------------------------------------------------

// Mark T as allowed
template <typename T>
struct OptionalAllowed {
    static constexpr bool value = false;
};

#define RW_ALLOW_OPTIONAL(_type_)           \
    template <>                             \
    struct OptionalAllowed<_type_> {        \
        static constexpr bool value = true; \
    };

RW_ALLOW_OPTIONAL(uint8_t);
RW_ALLOW_OPTIONAL(int8_t);
RW_ALLOW_OPTIONAL(uint16_t);
RW_ALLOW_OPTIONAL(int16_t);
RW_ALLOW_OPTIONAL(const char*);

template <typename T>
struct Optional {
protected:
    Optional() = default;

public:
    Optional(const T& v) { assign(v); }

    Optional& operator=(const T& v)
    {
        assign(v);
        return *this;
    }

    static Optional Nullopt() { return Optional(); }

    void reset() { has_value_ = false; }

    bool has_value() const { return has_value_; }

    explicit operator bool() const { return has_value_; }

    T& value() { return value_; }

    const T& value() const { return value_; }

    T& operator*() { return value_; }

    const T& operator*() const { return value_; }

    T* operator->() { return &value_; }

    const T* operator->() const { return &value_; }

private:
    T value_;
    bool has_value_ = false;

    void assign(const T& v)
    {
        static_assert(OptionalAllowed<T>::value,
            "Optional<T> requires explicitly allowed trivial type");

        value_ = v;
        has_value_ = true;
    }
};

// ------------------------------------------------------------------------------

/// Entity Id provided by engine
using EntityId = uint8_t;

struct DrawContext;

template <typename T>
struct _RawInputT {
    T left {};
    T right {};
    T up {};
    T down {};

    T select {};

    template <typename U>
    _RawInputT& operator=(_RawInputT<U> rhs)
    {
        left = rhs.left;
        right = rhs.right;
        up = rhs.up;
        down = rhs.down;

        select = rhs.select;

        return *this;
    }
};

struct MomentaryValue {
    bool last { false };
    bool current { false };

    void set(bool b)
    {
        last = current;
        current = b;
    }
    bool get() const { return current && current != last; }

    void operator=(const bool& b) { set(b); }
    operator bool() const { return get(); }
};

using RawInput = _RawInputT<bool>;
using MomentaryInput = _RawInputT<MomentaryValue>;

// ------------------------------------------------------------------------------
using ActorFlags = uint8_t;

/// Constructible by engine's spawn() method
struct Actor {
    ActorFlags flags {};

    static constexpr ActorFlags Move { 0x1 << 0 };
    static constexpr ActorFlags Control { 0x1 << 1 };
    static constexpr ActorFlags Health { 0x1 << 2 };
    static constexpr ActorFlags Collider { 0x1 << 3 };

    static constexpr ActorFlags Text { 0x1 << 4 };
    static constexpr ActorFlags Input { 0x1 << 5 };
    static constexpr ActorFlags Timer { 0x1 << 6 };
};

// ------------------------------------------------------------------------------

using ColliderFn = void (*)(const EntityId& receiver, const EntityId peer);
using InputHandlerFn = void (*)(const EntityId& receiver, const RawInput& input);
using TimerFn = void (*)(const EntityId& receiver);

#define COLLIDER_FN +[](const ::rwe::EntityId &receiver, const ::rwe::EntityId peer)
#define INPUTHANDLER_FN +[](const ::rwe::EntityId &receiver, const ::rwe::RawInput &rawInput)
#define TIMER_FN +[](const ::rwe::EntityId &receiver)

struct Components {
    struct Position {
        int8_t x {}, y {};
        int8_t lookAt {};
        bool operator==(const Position& rhs) const { return x == rhs.x && y == rhs.y; }
    };
    struct Speed {
        int8_t vx {}, vy {};
        int8_t rotation {};
    };
    struct Hitpoints {
        int8_t hp {};
    };

    struct Collider {
        int8_t value {};
        ColliderFn colliderFn { nullptr };
    };
    struct InputHandler {
        InputHandlerFn inputHandlerFn { nullptr };
    };
    struct Text {
        const char* line[2];
    };
    struct Timer {
        uint8_t currentFrame {};

        uint8_t frameCount {};
        TimerFn fn { nullptr };
    };

    Position position[Setup::Actors] {};
    Speed speed[Setup::Actors] {};
    Hitpoints hitpoints[Setup::Actors] {};

    //
    Collider collider[Setup::Actors] {};
    InputHandler inputHandler[Setup::Actors] {};
    Text text[Setup::Actors] {};
    Timer timer[Setup::Actors] {};
};

// --------------------------------------------------------------------------------

struct CustomCharacter {
    uint8_t data[8] { 0, 64, 0, 64, 0, 64, 0, 64 };

    CustomCharacter(uint8_t* src)
    {
        for (int i = 0; i < 8; i++) {
            data[i] = src[i];
        }
    }
};

struct DrawContext {
    void* ctx { nullptr };

    void (*peerClearAll)(void* ctx) { nullptr };
    void (*peerAddText)(void* ctx, int8_t x, int8_t y, const char* txt) { nullptr };

    void (*peerDefineChar)(void* ctx, uint8_t idx, const CustomCharacter c) { nullptr };
    void (*peerAddChar)(void* ctx, int8_t x, int8_t y, const uint8_t id) { nullptr };

    /// NB: must be set by 'frontend', is '8' for LiquidCrystal library
    uint8_t customCharacters { 0 };

    /// NB: extra flag
    bool disableDirectBufferDraw { false };

    /// NB: buffer is directly accessible for simple 'frontend'
    char buffer[Setup::ScreenHeight][Setup::ScreenWidth + 2];

    uint16_t updateFlags[Setup::ScreenWidth / 16][Setup::ScreenHeight] {};

    void defineChar(uint8_t idx, const CustomCharacter c)
    {
        if (!ctx)
            return;
        peerDefineChar(ctx, idx, c);
    }

    void addChar(int8_t x, int8_t y, const uint8_t id)
    {
        if (!ctx)
            return;
        peerAddChar(ctx, x, y, id);
    }

    DrawContext()
    {
        clearAll();
    }

    void clearAll()
    {
        for (int y = 0; y < Setup::ScreenHeight; y++) {
            for (int x = 0; x < Setup::ScreenWidth; x++) {
                buffer[y][x] = ' ';
            }
            buffer[y][Setup::ScreenWidth] = 0;
        }
    }
    void addText(int8_t x, int8_t y, const char* txt)
    {
        if (!txt)
            return;

        if (x < 0)
            x = 0;
        if (x > 15)
            x = 15;
        if (y < 0)
            y = 0;
        if (y > 1)
            y = 1;

        size_t len = strlen(txt);
        if (len > (16 - x))
            len = 16 - x;

        for (int i = 0; i < static_cast<int>(len); i++) {
            buffer[y][x + i] = txt[i];
        }
    }

    void _begin()
    {
        // reset update flags
        for (int x = 0; x < (Setup::ScreenWidth / 16); x++)
            for (int y = 0; y < (Setup::ScreenHeight); y++)
                updateFlags[x][y] = 0;
    }
    void _end()
    {
        // TODO
        // calculate elements to repaint
        // call peer functions
    }
};

struct SharedData {
    union Element {
        uint8_t uint8[4];
        int8_t int8[4];
        uint16_t uint16[2];
        int16_t int16[2];
        int32_t uint32;
        int32_t int32;
        void* ptr;
    };

protected:
    uint8_t _raw_bytes[Setup::SharedNumbers * sizeof(Element)];

public:
    const char* constStrings[Setup::SharedStrings];

    Element getElement(int index) const
    {
        Element out;
        memcpy(&out, &_raw_bytes[index * sizeof(Element)], sizeof(Element));
        return out;
    }

    void setElement(int index, const Element& in)
    {
        memcpy(&_raw_bytes[index * sizeof(Element)], &in, sizeof(Element));
    }
};

// --------------------------------------------------------------------------------

struct Engine {
    using Tag = uint8_t;

protected:
    Components _components {};
    Actor _actors[Setup::Actors];
    Tag _tags[Setup::Tags];

    /// Dummy value storage
    static union {
        Actor actor;
    } _dummyValues;

public:
    struct ActorBuilder {
        Engine& _obj;
        ActorFlags _flags;
        Optional<Engine::Tag> _tag { Optional<Engine::Tag>::Nullopt() };

    protected:
        friend class Engine;
        ActorBuilder(Engine& e, ActorFlags f)
            : _obj(e)
            , _flags(f)
        {
        }

        //
        Components::Position _position;
        Components::Speed _speed;
        Components::Hitpoints _hitpoints;
        Components::Collider _collider;
        Components::Text _text;
        Components::InputHandler _inputHandler;
        Components::Timer _timer;

    public:
        ActorBuilder position(int8_t x, int8_t y)
        {
            auto& p = _position;
            p.x = x;
            p.y = y;
            return *this;
        }
        ActorBuilder control()
        {
            _flags |= Actor::Control;
            return *this;
        }
        ActorBuilder speed(int8_t vx, int8_t vy, bool noFlag = false)
        {
            if (!noFlag)
                _flags |= Actor::Move;

            auto& p = _speed;
            p.vx = vx;
            p.vy = vy;
            return *this;
        }
        ActorBuilder hitpoints(int8_t hp)
        {
            _flags |= Actor::Health;

            auto& p = _hitpoints;
            p.hp = hp;
            return *this;
        }

        ActorBuilder collider(int8_t value, ColliderFn fn)
        {
            _flags |= Actor::Collider;

            auto& p = _collider;
            p.value = value;
            p.colliderFn = fn;
            return *this;
        }
        ActorBuilder text(const char* l0, const char* l1 = nullptr)
        {
            _flags |= Actor::Text;

            auto& p = _text;
            p.line[0] = l0;
            p.line[1] = l1;
            return *this;
        }
        ActorBuilder inputHandler(InputHandlerFn fn)
        {
            _flags |= Actor::Input;

            auto& p = _inputHandler;
            p.inputHandlerFn = fn;
            return *this;
        }
        ActorBuilder timer(uint8_t count, TimerFn fn)
        {
            _flags |= Actor::Timer;

            auto& p = _timer;
            p.currentFrame = 0;
            p.frameCount = count;
            p.fn = fn;
            return *this;
        }
        ActorBuilder tag(Tag tag)
        {
            _tag = tag;
            return *this;
        }

        Optional<EntityId> spawn() const { return _obj._spawn(*this); }

        void spawnToId(EntityId& id)
        {
            auto r = spawn();
            if (r.has_value())
                id = r.value();
        }
    };

protected:
    Optional<EntityId> _spawn(ActorBuilder b)
    {
        if (!canSpawn()) {
            return Optional<EntityId>::Nullopt();
        };

        auto optEntityId = getFreeEntityId();
        if (!optEntityId.has_value())
            return 0;

        const auto& entityId = optEntityId.value();

        _actors[entityId].flags = b._flags;

        getPosition(entityId) = b._position;
        getSpeed(entityId) = b._speed;
        getHitpoints(entityId) = b._hitpoints;
        getCollider(entityId) = b._collider;
        getText(entityId) = b._text;
        getInputHandler(entityId) = b._inputHandler;
        getTimer(entityId) = b._timer;

        if (b._tag.has_value())
            setTag(entityId, b._tag.value());

        return entityId;
    }

public:
    RawInput rawInput {};
    DrawContext drawContext {};
    SharedData sharedData {};

    struct ViewportScroll {
        uint16_t scrollX {0};
        uint16_t scrollY {0};
    };
    ViewportScroll viewportScroll {};

    Engine()
    {
        for (int i = 0; i < Setup::Actors; i++)
            _actors[i].flags = 0;
    }

    // ---
    // Component getters

    Components::Position& getPosition(EntityId id) { return _components.position[id]; }
    Components::Speed& getSpeed(EntityId id) { return _components.speed[id]; }
    Components::Hitpoints& getHitpoints(EntityId id) { return _components.hitpoints[id]; }

    Components::Collider& getCollider(EntityId id) { return _components.collider[id]; }
    Components::Text& getText(EntityId id) { return _components.text[id]; }
    Components::InputHandler& getInputHandler(EntityId id) { return _components.inputHandler[id]; }
    Components::Timer& getTimer(EntityId id) { return _components.timer[id]; }

    /// Get Actor by entity id, returns dummy if fails
    Actor& getActor(EntityId id)
    {
        if (id >= Setup::Actors)
            return _dummyValues.actor;
        return _actors[id];
    }

    // ---
    void setTag(EntityId id, Tag tag)
    {
        if (tag >= Setup::Tags)
            return;
        _tags[tag] = id;
    }

    Actor& getActorByTag(const Tag tag)
    {
        if (tag >= Setup::Tags)
            return _dummyValues.actor;

        return getActor(_tags[tag]);
    }

    // ---

    /// find free id
    bool canSpawn() const
    {
        for (auto& e : _actors)
            if (e.flags == 0)
                return true;

        return false;
    }

    /// find available
    // EntityId getFreeEntityId()
    // {
    //     for (auto i = 0; i < Setup::Actors; i++) {
    //         if (_actors[i].flags == 0)
    //             return i;
    //     }

    //     return 0;
    // }

    Optional<EntityId> getFreeEntityId()
    {
        for (auto i = 0; i < Setup::Actors; i++) {
            if (_actors[i].flags == 0)
                return i;
        }

        return Optional<EntityId>::Nullopt();
    }

    ActorBuilder make(ActorFlags f = 0) { return ActorBuilder { *this, f }; }

    /// remove(Actor) : set class to zero @ entityId
    void remove(EntityId id) { _actors[id].flags = 0; }

    // --------------------------------------------------------------------------------
    // Systems

    void inputSystem()
    {
        // iterate actors
        // if class != 0
        // if controllable
        // forward input
        for (int i = 0; i < Setup::Actors; i++) {
            if (_actors[i].flags != 0) {
                // control: change speed directly
                if (_actors[i].flags & Actor::Control) {
                    auto& p = getSpeed(i);
                    if (rawInput.left)
                        p.vx = -1;
                    if (rawInput.right)
                        p.vx = 1;
                    if (rawInput.up)
                        p.vy = -1;
                    if (rawInput.down)
                        p.vy = 1;

                    if (!(rawInput.left || rawInput.right || rawInput.up || rawInput.down)) {
                        p.vx = 0;
                        p.vy = 0;
                    }
                }

                // input handler: forward
                if (_actors[i].flags & Actor::Input) {
                    getInputHandler(i).inputHandlerFn(i, rawInput);
                }
            }
        }
    }

    void movementSystem()
    {
        // iterate actors
        // if class != 0
        // if moveable : += speed
        for (int i = 0; i < Setup::Actors; i++) {
            if (_actors[i].flags != 0) {
                if (_actors[i].flags & Actor::Move) {
                    auto& p = _components.position[i];

                    p.x = int8_t(p.x) + _components.speed[i].vx;
                    p.y = int8_t(p.y) + _components.speed[i].vy;

                    if (p.x >= 16)
                        p.x = 15;
                    if (p.x < 0)
                        p.x = 0;
                    if (p.y >= 2)
                        p.y = 1;
                    if (p.y < 0)
                        p.y = 0;
                }
            }
        }
    }

    void collisionSystem()
    {
        // iterate actors
        // if class != 0
        // if colliding:
        //   iterate actors
        //   if same pos: call onCollision on both
        for (int i = 0; i < Setup::Actors; i++) {
            if (_actors[i].flags != 0) {
                if (_actors[i].flags & Actor::Collider) {
                    //
                    for (int j = i + 1; j < Setup::Actors; j++) {
                        if (_actors[j].flags != 0) {
                            if (_actors[j].flags & Actor::Collider) {
                                // call collider from components
                                auto& p = getCollider(i);
                                p.colliderFn(i, j);

                                auto& p2 = getCollider(j);
                                p2.colliderFn(j, i);
                            }
                        }
                    }
                }
            }
        }
    }

    void lifetimeSystem()
    {
        // iterate actors
        // if class != 0
        // if withhealth
        // if hp == 0 : set class == 0
        for (int i = 0; i < Setup::Actors; i++) {
            if (_actors[i].flags != 0) {
                if (_actors[i].flags & Actor::Health) {
                    if (_components.hitpoints[i].hp == 0) {
                        _actors[i].flags = 0;
                    }
                }
            }
        }
    }

    void timerSystem()
    {
        for (int i = 0; i < Setup::Actors; i++) {
            if (_actors[i].flags != 0) {
                if (_actors[i].flags & Actor::Timer) {
                    // timer fn here:
                    auto& p = getTimer(i);
                    p.currentFrame++;
                    if (p.currentFrame >= p.frameCount) {
                        p.currentFrame = 0;
                        p.fn(i);
                    }
                }
            }
        }
    }

    void renderSystem()
    {
        // provide drawcontext
        // iterate - draw each
        for (int i = 0; i < Setup::Actors; i++) {
            if (_actors[i].flags != 0) {
                if (_actors[i].flags & Actor::Text) {
                    auto& pos = getPosition(i);
                    auto& p = getText(i);

                    if (p.line[0])
                        drawContext.addText(pos.x, pos.y, p.line[0]);
                    if (p.line[1])
                        drawContext.addText(pos.x, pos.y + 1, p.line[1]);
                }
            }
        }
    }

    // ----------------------------------------

    void runLoop()
    {
        inputSystem();
        movementSystem();
        collisionSystem();
        lifetimeSystem();
        timerSystem();
        renderSystem();
    }

    // ----------------------------------------

    static Engine& get()
    {
        static Engine ret {};
        return ret;
    }
};

// static Engine &a16{Engine::get()};

/// macro for Engine singleton
#define RWE ::rwe::Engine::get()

// --------------------------------------------------------------------------------
// Collision functions:

#define COLLIDER_FN +[](const ::rwe::EntityId &receiver, const ::rwe::EntityId peer)

static inline bool TestHit(const EntityId& receiver, const EntityId peer)
{
    auto& pR = RWE.getPosition(receiver);
    auto& pP = RWE.getPosition(peer);
    return pR == pP;
}

#define TEST_HIT TestHit(receiver, peer)

static inline void HitReceiver(const EntityId& receiver, const EntityId peer)
{
    if (TEST_HIT) {
        auto peerHitValue = RWE.getCollider(peer).value;
        auto& receiverHp = RWE.getHitpoints(receiver);
        if (receiverHp.hp > peerHitValue)
            receiverHp.hp -= peerHitValue;
        else
            receiverHp.hp = 0;
    }
}

static inline void HitPeer(const EntityId& receiver, const EntityId peer)
{
    if (TEST_HIT) {
        auto hitValue = RWE.getCollider(receiver).value;
        auto& peerHp = RWE.getHitpoints(peer);
        if (peerHp.hp > hitValue)
            peerHp.hp -= hitValue;
        else
            peerHp.hp = 0;
    }
}

// --------------------------------------------------------------------------------
// Input handler functions

/// Disallow speed inversion in any axis
static inline void NonInvertingControl(const EntityId& receiver, const RawInput& input)
{
    auto& engine = Engine::get();
    auto& speed = engine.getSpeed(receiver);

    // Change direction on input, but prevent reverse direction
    if (input.left && speed.vx != 1) {
        speed.vx = -1;
        speed.vy = 0;
    } else if (input.right && speed.vx != -1) {
        speed.vx = 1;
        speed.vy = 0;
    } else if (input.up && speed.vy != 1) {
        speed.vx = 0;
        speed.vy = -1;
    } else if (input.down && speed.vy != -1) {
        speed.vx = 0;
        speed.vy = 1;
    }
}

// --------------------------------------------------------------------------------
// Timer functions

template <TimerFn fn>
void TimerOnce()
{
    return TIMER_FN
    {
        fn(receiver);
        Engine::get().remove(receiver);
    };
}

#define TIMER_ONCE(x) ::rwe::TimerOnce<x>()

// --------------------------------------------------------------------------------
// Pre-made actors

namespace A {

    /// clear background 16x2
    static inline Engine::ActorBuilder Background()
    {
        return Engine::get() //
            .make()
            .text("                ", "                ");
    }

    /// Movable player character
    static inline Engine::ActorBuilder PlayerChar(const char* c = "#")
    {
        return Engine::get() //
            .make(Actor::Move | Actor::Control)
            .text(c);
    }

} // namespace A

} // namespace rwe
