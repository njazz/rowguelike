#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
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

#ifndef RW_MOVE_OUTSIDE_SCREEN
#define RW_MOVE_OUTSIDE_SCREEN false
#endif

#ifndef RW_PAGE_COUNT
#define RW_PAGE_COUNT 16
#endif

#ifndef RW_WITH_3D
#define RW_WITH_3D false
#endif

// ------------------------------------------------------------------------------

struct Setup {
    /// Minimal screen is a 1x1, unfortunatyely the 0x0 LCD is not supported
    static constexpr uint8_t ScreenWidth{RW_SETUP_SCREEN_WIDTH > 0 ? RW_SETUP_SCREEN_WIDTH : 1};
    static constexpr uint8_t ScreenHeight{RW_SETUP_SCREEN_HEIGHT > 0 ? RW_SETUP_SCREEN_HEIGHT : 1};

    static constexpr uint8_t LastSymbolX{ScreenWidth - 1};
    static constexpr uint8_t LastSymbolY{ScreenHeight - 1};

    static constexpr uint8_t Actors { RW_SETUP_ACTORS };
    static constexpr uint8_t Tags { RW_SETUP_TAGS };
    static constexpr uint8_t SharedNumbers { RW_SETUP_SHARED_NUMBERS };
    static constexpr uint8_t SharedStrings { RW_SETUP_SHARED_STRINGS };

    /// Bit offset for higher resolution
    /// NB: for position using int8_t this is valid in range 0..3
    /// for high-resolution position mode it's 0..9
    static constexpr uint8_t MaxViewportScale{RW_SETUP_MAX_VIEWPORT_SCALE};

    static constexpr bool MoveOutsideScreen{RW_MOVE_OUTSIDE_SCREEN};

    static constexpr uint8_t PageCount{RW_PAGE_COUNT};
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

/// Template for the control state
template<typename T>
struct _ControlStateT
{
    T left {};
    T right {};
    T up {};
    T down {};

    T select {};

    template<typename U>
    _ControlStateT &operator=(_ControlStateT<U> rhs)
    {
        left = rhs.left;
        right = rhs.right;
        up = rhs.up;
        down = rhs.down;

        select = rhs.select;

        return *this;
    }

    // Helper function
    const bool anyDirection() const { return left || right || up || down; }
    const bool anyButton() const { return anyDirection() || select; }
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

using RawControlState = _ControlStateT<bool>;
using MomentaryControlState = _ControlStateT<MomentaryValue>;

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
// Used function types

using ColliderFn = void (*)(const EntityId& receiver, const EntityId peer);
using InputFn = void (*)(const EntityId &receiver, const RawControlState &input);
using TimerFn = void (*)(const EntityId& receiver);

using VoidFn = void (*)(void);

#define COLLIDER_FN +[](const ::rwe::EntityId &receiver, const ::rwe::EntityId peer)
#define INPUT_FN +[](const ::rwe::EntityId &receiver, const ::rwe::RawControlState &rawInput)
#define TIMER_FN +[](const ::rwe::EntityId &receiver)

/// macro for non-capturing void(void) lambda
#define FN +[]()

// ------------------------------------------------------------------------------
// Components storage

struct Components {
    struct Position {
        int8_t x {}, y {};
        int8_t lookAt {};
        bool operator==(const Position &rhs) const
        {
            return x == rhs.x && y == rhs.y && lookAt == rhs.lookAt;
        }

        void randomizeX() { x = rand() % Setup::ScreenWidth; }
        void randomizeY() { y = rand() % Setup::ScreenHeight; }
        void randomize()
        {
            randomizeX();
            randomizeY();
        }
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
    struct Input
    {
        InputFn inputFn{nullptr};
    };
    struct Text {
        const char *line[Setup::ScreenHeight];

        Text()
        {
            for (int i = 0; i < Setup::ScreenHeight; i++)
                line[i] = nullptr;
        }
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
    Input input[Setup::Actors]{};
    Text text[Setup::Actors] {};
    Timer timer[Setup::Actors] {};
};

// --------------------------------------------------------------------------------
// Display classes

struct CustomCharacter {
    /// Some test pattern now
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

    void (*peerClearAll)(void *ctx){+[](void *) {

    }};
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
        if (ctx)
            peerClearAll(ctx);

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

        if (!Setup::MoveOutsideScreen) {
            if (x < 0)
                x = 0;
            if (x > Setup::LastSymbolX)
                x = Setup::LastSymbolX;
            if (y < 0)
                y = 0;
            if (y > Setup::LastSymbolY)
                y = Setup::LastSymbolY;
        }

        size_t len = strlen(txt);
        if (len > (Setup::ScreenWidth - x))
            len = Setup::ScreenWidth - x;

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
    union DummyValues {
        Actor actor;

        Components::Position _position;
        Components::Speed _speed;
        Components::Hitpoints _hitpoints;
        Components::Collider _collider;
        Components::Text _text;
        Components::Input _input;
        Components::Timer _timer;

        DummyValues() {}
    };
    static DummyValues _dummyValues;

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
        Components::Input _input;
        Components::Timer _timer;

    public:
        /// Random position on screen w/o range
        ActorBuilder &randomPosition()
        {
            auto &p = _position;
            p.x = rand() % Setup::ScreenWidth;
            p.y = rand() % Setup::ScreenHeight;
            return *this;
        }

        ActorBuilder &position(int8_t x, int8_t y)
        {
            auto& p = _position;
            p.x = x;
            p.y = y;
            return *this;
        }
        ActorBuilder &control()
        {
            _flags |= Actor::Control;
            return *this;
        }
        ActorBuilder &speed(int8_t vx, int8_t vy, bool noFlag = false)
        {
            if (!noFlag)
                _flags |= Actor::Move;

            auto& p = _speed;
            p.vx = vx;
            p.vy = vy;
            return *this;
        }
        ActorBuilder &hitpoints(int8_t hp)
        {
            _flags |= Actor::Health;

            auto& p = _hitpoints;
            p.hp = hp;
            return *this;
        }

        ActorBuilder &collider(int8_t value, ColliderFn fn)
        {
            _flags |= Actor::Collider;

            auto& p = _collider;
            p.value = value;
            p.colliderFn = fn;
            return *this;
        }
        ActorBuilder &text(const char *l0, const char *l1 = nullptr)
        {
            _flags |= Actor::Text;

            auto& p = _text;
            p.line[0] = l0;
            p.line[1] = l1;
            return *this;
        }
        ActorBuilder &textLine(const uint8_t line, const char *l0)
        {
            if (line >= Setup::ScreenHeight)
                return *this;

            _flags |= Actor::Text;

            auto &p = _text;

            p.line[line] = l0;

            return *this;
        }
        ActorBuilder &input(InputFn fn)
        {
            _flags |= Actor::Input;

            auto &p = _input;
            p.inputFn = fn;
            return *this;
        }
        ActorBuilder &timer(uint8_t count, TimerFn fn)
        {
            _flags |= Actor::Timer;

            auto& p = _timer;
            p.currentFrame = 0;
            p.frameCount = count;
            p.fn = fn;
            return *this;
        }

        /// Timer that runs each frame
        ActorBuilder &eachFrame(TimerFn fn)
        {
            _flags |= Actor::Timer;

            auto &p = _timer;
            p.currentFrame = 0;
            p.frameCount = 0;
            p.fn = fn;
            return *this;
        }

        ActorBuilder &tag(Tag tag)
        {
            _tag = tag;
            return *this;
        }

        //

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
        getInput(entityId) = b._input;
        getTimer(entityId) = b._timer;

        if (b._tag.has_value())
            setTag(entityId, b._tag.value());

        return entityId;
    }

public:
    RawControlState rawInput{};

    DrawContext drawContext{};
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
    // NB: returns dummy if id>= Setup::Actors

    Components::Position &getPosition(EntityId id)
    {
        if (id >= Setup::Actors)
            return _dummyValues._position;
        return _components.position[id];
    }
    Components::Speed &getSpeed(EntityId id)
    {
        if (id >= Setup::Actors)
            return _dummyValues._speed;
        return _components.speed[id];
    }
    Components::Hitpoints &getHitpoints(EntityId id)
    {
        if (id >= Setup::Actors)
            return _dummyValues._hitpoints;
        return _components.hitpoints[id];
    }

    Components::Collider &getCollider(EntityId id)
    {
        if (id >= Setup::Actors)
            return _dummyValues._collider;
        return _components.collider[id];
    }
    Components::Text &getText(EntityId id)
    {
        if (id >= Setup::Actors)
            return _dummyValues._text;
        return _components.text[id];
    }
    Components::Input &getInput(EntityId id)
    {
        if (id >= Setup::Actors)
            return _dummyValues._input;
        return _components.input[id];
    }
    Components::Timer &getTimer(EntityId id)
    {
        if (id >= Setup::Actors)
            return _dummyValues._timer;
        return _components.timer[id];
    }

    /// true if actor flags != 0
    bool isActiveActor(EntityId id)
    {
        if (id >= Setup::Actors)
            return false;
        return _actors[id].flags != 0;
    }

    // ---
    void setTag(EntityId id, Tag tag)
    {
        if (tag >= Setup::Tags)
            return;
        _tags[tag] = id;
    }

    // ---

    /// Get Actor by entity id, returns dummy if fails
    Actor &getActor(EntityId id)
    {
        if (id >= Setup::Actors) {
            _dummyValues.actor.flags = 0;
            return _dummyValues.actor;
        }
        return _actors[id];
    }

    Actor &getActorByTag(const Tag tag)
    {
        if (tag >= Setup::Tags) {
            _dummyValues.actor.flags = 0;
            return _dummyValues.actor;
        }

        return getActor(_tags[tag]);
    }

    /// find free id
    bool canSpawn() const
    {
        for (auto& e : _actors)
            if (e.flags == 0)
                return true;

        return false;
    }

    Optional<EntityId> getFreeEntityId()
    {
        for (auto i = 0; i < Setup::Actors; i++) {
            if (_actors[i].flags == 0)
                return i;
        }

        return Optional<EntityId>::Nullopt();
    }

    // ---

    ActorBuilder make(ActorFlags f = 0) { return ActorBuilder { *this, f }; }

    /// Make a copy of Actor if it exists / is non-zero
    ActorBuilder clone(EntityId id)
    {
        if (!isActiveActor(id))
            return make();

        auto ret = ActorBuilder{*this, getActor(id).flags};

        ret._position = getPosition(id);
        ret._speed = getSpeed(id);
        ret._hitpoints = getHitpoints(id);

        ret._collider = getCollider(id);
        ret._text = getText(id);
        ret._input = getInput(id);
        ret._timer = getTimer(id);

        return ret;
    }

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
                    getInput(i).inputFn(i, rawInput);
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

                    // NB: currently limited by the setup
                    if (!Setup::MoveOutsideScreen) {
                        if (p.x >= Setup::ScreenWidth)
                            p.x = Setup::LastSymbolX;
                        if (p.x < 0)
                            p.x = 0;
                        if (p.y >= Setup::ScreenHeight)
                            p.y = Setup::LastSymbolY;
                        if (p.y < 0)
                            p.y = 0;
                    }
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

                    for (int y = 0; y < Setup::ScreenHeight; y++) {
                        if (p.line[y])
                            drawContext.addText(pos.x, pos.y + y, p.line[y]);
                    }
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

Engine::DummyValues Engine::_dummyValues;

// static Engine &a16{Engine::get()};

/// macro for Engine singleton
#define RWE ::rwe::Engine::get()

/// shorthand for actor definition (with no flags)
#define RW_ACTOR ::rwe::Engine::get().make()

// --------------------------------------------------------------------------------
// Collision functions:

#define COLLIDER_FN +[](const ::rwe::EntityId &receiver, const ::rwe::EntityId peer)

/// Helper function for the TEST_HIT macro
static inline bool _TestHit(const EntityId &receiver, const EntityId peer)
{
    auto& pR = RWE.getPosition(receiver);
    auto& pP = RWE.getPosition(peer);
    return pR == pP;
}

#define TEST_HIT _TestHit(receiver, peer)

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

/// macros only for the uniform syntax
#define COLLIDER_HIT_RECEIVER HitReceiver
#define COLLIDER_HIT_PEER HitPeer

// --------------------------------------------------------------------------------
// Input handler functions

/// Disallow speed inversion in any axis
static inline void NonInvertingControl(const EntityId &receiver, const RawControlState &input)
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

// Basic single button handlers
// Yet removed as that gives a cluttered syntax

// /// Basic handler: on SELECT
// template<void (*fn)()>
// static inline void OnSelect()
// {
//     return +[](const EntityId &receiver, const RawControlState &input) {
//         if (input.select)
//             fn();
//     };
// }

// /// Basic handler: on UP
// template<VoidFn fn>
// static inline void OnUp(const EntityId &receiver, const RawControlState &input)
// {
//     if (input.up)
//         fn();
// }

// /// Basic handler: on DOWN
// template<VoidFn fn>
// static inline void OnDown(const EntityId &receiver, const RawControlState &input)
// {
//     if (input.down)
//         fn();
// }

// /// Basic handler: on LEFT
// template<VoidFn fn>
// static inline void OnLeft(const EntityId &receiver, const RawControlState &input)
// {
//     if (input.left)
//         fn();
// }

// /// Basic handler: on RIGHT
// template<VoidFn fn>
// static inline void OnRight(const EntityId &receiver, const RawControlState &input)
// {
//     if (input.right)
//         fn();
// }

// #define INPUT_ON_SELECT_(x) ::rwe::OnSelect<x>()

// #define INPUT_ON_UP_(x) ::rwe::OnUp<x>
// #define INPUT_ON_DOWN_(x) ::rwe::OnDown<x>
// #define INPUT_ON_LEFT_(x) ::rwe::OnLeft<x>
// #define INPUT_ON_RIGHT_(x) ::rwe::OnRight<x>

// --------------------------------------------------------------------------------
// Timer functions

/// Run the provided action once and keep actor
template <TimerFn fn>
void TimerOnce()
{
    return TIMER_FN
    {
        fn(receiver);
        Engine::get().getTimer(receiver).fn = TIMER_FN{};
    };
}

/// Run the provided action and remove the Actor
template<TimerFn fn>
void TimerOnceAndRemoveThis()
{
    return TIMER_FN
    {
        fn(receiver);
        RWE.remove(receiver);
    };
}

/// remove the receiver after timer's interval
void TimerRemoveThis(const ::rwe::EntityId &receiver)
{
    RWE.remove(receiver);
}

#define TIMER_ONCE_(x) ::rwe::TimerOnce<x>()
#define TIMER_ONCE_AND_REMOVE_THIS_(x) ::rwe::TimerOnceAndRemoveThis<x>()
#define TIMER_REMOVE_THIS ::rwe::TimerRemoveThis

// --------------------------------------------------------------------------------
// Pre-made actors

namespace A {

/// clear background ScreenWidth x ScreenHeight
static inline Engine::ActorBuilder Background(const char symbol = ' ')
{
    static char textLine[Setup::ScreenWidth];
    for (int i = 0; i < Setup::ScreenWidth; i++)
        textLine[i] = symbol;

    auto r = Engine::get() //
                 .make();

    for (int i = 0; i < Setup::ScreenHeight; i++)
        r.textLine(i, textLine);

    return r;
}

    /// Movable player character
    static inline Engine::ActorBuilder PlayerChar(const char* c = "#")
    {
        return Engine::get() //
            .make(Actor::Move | Actor::Control)
            .text(c);
    }

} // namespace A

// --------
// Page macros

typedef void (*PageFn)(Engine &);

#define PAGE_FN +[](Engine & page)

class PageManager
{
    PageFn _pages[Setup::PageCount];

    PageManager()
    {
        for (auto i = 0; i < Setup::PageCount; i++) {
            _pages[i] = PAGE_FN{};
        }
    }

public:
    void setPage(uint8_t idx, PageFn fn)
    {
        if (idx >= Setup::PageCount)
            return;

        _pages[idx] = fn;
    }

    void switchPage(uint8_t idx)
    {
        if (idx >= Setup::PageCount)
            return;

        RWE = ::rwe::Engine();
        _pages[idx](RWE);
    }

    static PageManager &get()
    {
        static PageManager obj;
        return obj;
    }
};

#define SET_PAGE_(idx, ...) PageManager::get().setPage(idx, +[](Engine & page) __VA_ARGS__)
#define SWITCH_PAGE(x) PageManager::get().switchPage(x)

//

/// Wrapper for 'if let value = Optional<T>' syntax: IF_LET(optional, { value = 0; })
#define IF_LET(__value, ...) \
    if (__value.has_value()) \
        +[](decltype(__value.value()) value) { __VA_ARGS__ };

/// Wrapper for 'if let ...' syntax with named variable: IF_LET_NAME(optional, v1, { v1 = 0; })
#define IF_LET_NAME(__value, __value_name, ...) \
    if (__value.has_value()) \
        +[](decltype(__value.value()) __value_name) { __VA_ARGS__ };

// ------------------
// 3D Engine lol

#if RW_WITH_3D

typedef int32_t fixp32; // 16.16 fixed-point

#define FIX_SHIFT 16

static inline fixp32 to_fix(int32_t x)
{
    return x << FIX_SHIFT;
}
static inline int32_t from_fix(fixp32 x)
{
    return x >> FIX_SHIFT;
}
#define TO_FIX(x) to_fix(x)
#define FROM_FIX(x) from_fix(x)

fixp32 operator"" _fp(unsigned long long x)
{
    return to_fix(static_cast<int32_t>(x));
}

#define FIX_MUL(a, b) ((fixp32) (((int64_t) (a) * (b)) >> FIX_SHIFT))
#define FIX_DIV(a, b) ((fixp32) (((int64_t) (a) << FIX_SHIFT) / (b)))

typedef struct
{
    fixp32 x, y, z;
} Vec3;

typedef struct
{
    int8_t p; // compressed for display usage
} Point;

typedef struct
{
    uint8_t v[3]; // indices into vertex array
} Triangle;

typedef struct
{
    uint8_t v[2]; // indices into vertex array
} Line;

// -----

struct Mat4x4
{
    fixp32 m[4][4]; // row-major 4x4 matrix

    static Mat4x4 Identity()
    {
        Mat4x4 out = {};
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                out.m[i][j] = 0;
            }
        }

        for (int i = 0; i < 4; ++i) {
            out.m[i][i] = TO_FIX(1);
        }
        return out;
    }

    static Mat4x4 Perspective(fixp32 fov, fixp32 aspect, fixp32 znear, fixp32 zfar)
    {
        fixp32 f = FIX_DIV(TO_FIX(1),
                           FIX_MUL(fov, FIX_DIV(TO_FIX(1), TO_FIX(2)))); // cot(fov/2) ~ 1/fov
        Mat4x4 m = {0};
        m.m[0][0] = FIX_DIV(f, aspect);
        m.m[1][1] = f;
        m.m[2][2] = FIX_DIV(zfar + znear, znear - zfar);
        m.m[2][3] = TO_FIX(-1);
        m.m[3][2] = FIX_DIV(FIX_MUL(TO_FIX(2), FIX_MUL(zfar, znear)), znear - zfar);
        return m;
    }

    Vec3 multiplyVec(Vec3 v)
    {
        auto m = this;
        fixp32 x = FIX_MUL(m->m[0][0], v.x) + FIX_MUL(m->m[0][1], v.y) + FIX_MUL(m->m[0][2], v.z)
                   + m->m[0][3];
        fixp32 y = FIX_MUL(m->m[1][0], v.x) + FIX_MUL(m->m[1][1], v.y) + FIX_MUL(m->m[1][2], v.z)
                   + m->m[1][3];
        fixp32 z = FIX_MUL(m->m[2][0], v.x) + FIX_MUL(m->m[2][1], v.y) + FIX_MUL(m->m[2][2], v.z)
                   + m->m[2][3];
        fixp32 w = FIX_MUL(m->m[3][0], v.x) + FIX_MUL(m->m[3][1], v.y) + FIX_MUL(m->m[3][2], v.z)
                   + m->m[3][3];

        if (w != 0) {
            x = FIX_DIV(x, w);
            y = FIX_DIV(y, w);
            z = FIX_DIV(z, w);
        }

        return (Vec3) {x, y, z};
    }
};

// -----

struct Engine3D
{
    uint8_t buffer[8][8];

    Vec3 vertex[16];
    Vec3 projVertex[16];

    Line line[16];
    uint8_t lineCount{0};

    uint8_t point[16];
    uint8_t pointCount{0};

    Mat4x4 projection{Mat4x4::Perspective(45, 1, 0, 100)};

    // void drawLine(Vec3 a, Vec3 b)
    // {
    //     int x0 = FROM_FIX(a.x);
    //     int y0 = FROM_FIX(a.y);
    //     int x1 = FROM_FIX(b.x);
    //     int y1 = FROM_FIX(b.y);

    //     int dx = abs(x1 - x0);
    //     int dy = abs(y1 - y0);
    //     int sx = x0 < x1 ? 1 : -1;
    //     int sy = y0 < y1 ? 1 : -1;
    //     int err = dx - dy;
    //     while (1) {
    //         // plot(x0, y0);
    //         drawToBuffer(x0, y0);
    //         if (x0 == x1 && y0 == y1)
    //             break;
    //         int e2 = 2 * err;
    //         if (e2 >= dy) {
    //             err += dy;
    //             x0 += sx;
    //         }
    //         if (e2 <= dx) {
    //             err += dx;
    //             y0 += sy;
    //         }
    //     }
    // }

    void drawLine(Vec3 a, Vec3 b)
    {
        int x0 = FROM_FIX(a.x);
        int y0 = FROM_FIX(a.y);
        int x1 = FROM_FIX(b.x);
        int y1 = FROM_FIX(b.y);

        int dx = abs(x1 - x0);
        int dy = abs(y1 - y0);
        int sx = x0 < x1 ? 1 : -1;
        int sy = y0 < y1 ? 1 : -1;
        int err = dx - dy;

        while (true) {
            drawToBuffer(x0, y0);
            if (x0 == x1 && y0 == y1)
                break;
            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x0 += sx;
            }
            if (e2 < dx) {
                err += dx;
                y0 += sy;
            }
        }
    }

    void drawPoint(Vec3 a) { drawToBuffer(FROM_FIX(a.x), FROM_FIX(a.y)); }

    void drawToBuffer(int x, int y)
    {
        // graphic screen: 20x16
        if (x > 20)
            return;
        if (y >= 16)
            return;
        if (x < 0)
            return;
        if (y < 0)
            return;

        auto idx = (x / 5) + (y / 8) * 4; //(floor(x/5) + floor(y/8)*4);
        auto bitNumber = 4 - x % 5;
        auto byteNumber = y % 8;

        buffer[idx][byteNumber] |= 0x1 << bitNumber;
    }

    /// To be called after matrix update
    void prepare()
    {
        for (int i = 0; i < 16; ++i) {
            projVertex[i] = projection.multiplyVec(vertex[i]);
        }
    }

    void clear()
    {
        for (int i = 0; i < 8; i++)
            for (int j = 0; j < 8; j++)
                buffer[i][j] = 0;
    }

    void render()
    {
        clear();

        for (int i = 0; i < lineCount; ++i) {
            const auto v1 = line[i].v[0];
            const auto v2 = line[i].v[1];

            if (v1 >= 16)
                continue;
            if (v2 >= 16)
                continue;

            // projVertex
            auto a = projVertex[v1];
            auto b = projVertex[v2];

            drawLine(a, b);
        }

        for (int i = 0; i < pointCount; ++i) {
            const auto v1 = point[i];

            if (v1 >= 16)
                continue;

            // projVertex
            auto a = projVertex[v1];

            drawPoint(a);
        }
    }

protected:
    Engine3D() {}

public:
    static Engine3D &get()
    {
        static Engine3D obj;
        return obj;
    }
};

#define R3D Engine3D::get()

#endif

} // namespace rwe
