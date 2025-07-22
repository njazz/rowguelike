# rowguelike

```"Just because you can, doesn’t mean you should"```
    
  
## 📟 The Game Engine for small LCD text screens. "As not seen on TV"
  
![C++ CI](https://github.com/njazz/rowguelike/actions/workflows/cmake-multi-platform.yml/badge.svg)  
![Arduino](https://github.com/njazz/rowguelike/actions/workflows/arduino-builds.yml/badge.svg)  
  
**Targets:** Arduino, \*nix Terminal  
**Version:** 0.0.1 proof-of-concept
  


  
Available as Arduino library or Platform.io library
  
## 👾 Getting started

The game is defined by a set of 'Actors'
- Create an actor builder from the Engine singleton, optionally providing flags:  

    ```Engine::get().make(Actor::Move | Actor::Control) ```
- Continue with custom values for Actor's Components:  

    ``` .text("Text").position(3,0) ```
- Call ```spawn()``` to make an instance of Actor from ActorBuilder

Full example:

```c++
Engine::get().make()
	.text("Hello world")
	.position(0,0)
	.tag(1)
	.timer(10, TIMER_FN{
			auto id = Engine::get().byTag(1);
			auto p = Engine::get().getPosition(id);

			Engine.get().make()
				.position(p)
				.speed(1,0)
				.spawn();
		})
	.spawn();



```

## 💻 API

*“Give it to an LLM. It might not help, but it’ll sound confident.”*

```c++
// Component classes
struct Components::Position { int8_t x,y,lookAt };
struct Components::Speed { int8_t vx,vy,rotation };
struct Components::Collider { int8_t value; ColliderFn colliderFn };
struct Components::InputHandler { InputHandlerFn inputHandlerFn };
struct Components::Text { const char *line[2] };
struct Components::Timer { uint8_t currentFrame, frameCount; TimerFn fn };

// Engine getters:
Components::Position & Engine::getPosition(EntityId id) { return components.position[id]; }
Components::Speed & Engine::getSpeed(EntityId id) { return components.speed[id]; }
Components::Hitpoints & Engine::getHitpoints(EntityId id) { return components.hitpoints[id]; }

Components::Collider & Engine::getCollider(EntityId id) { return components.collider[id]; }
Components::Text & Engine::getText(EntityId id) { return components.text[id]; }
Components::InputHandler & Engine::getInputHandler(EntityId id) { return components.inputHandler[id]; }
Components::Timer & Engine::getTimer(EntityId id) { return components.timer[id]; }

// Input class
struct RawInput { bool left, right, up, down, select; };

// Actor::* is an ActorFlag

// make() accepts optional ActorFlags (bitmask) and returns ActionBuilder
// the final method for ActionBuilder is spawn()
Engine::get().make(); 

// Use remove() to de-spawn actor:
Engine::remove(EntityId);

// Render text at position
ActorFlags Actor::Text;		
ActorBuilder ActorBuilder::text(const char* t);

// Use speed at each frame to change position
ActorFlags Actor::Move;		
ActorBuilder ActorBuilder::position(int8_t x, int8_t y)
ActorBuilder ActorBuilder::speed(int8_t vx, int8_t vy)

// The actor is directly moved by input values
ActorFlags Actor::Control;	

// Checks hitpoints value and removes actor if HP==0
ActorFlags Actor::Health;	
ActorBuilder ActorBuilder::hitpoints(int8_t hp)

// Performs collision check / action function with this actor
ActorFlags Actor::Collider; 
ActorBuilder ActorBuilder::collider(int8_t value, ColliderFn fn)

// Runs input handler function
ActorFlags Actor::Input;	
ActorBuilder ActorBuilder::inputHandler(InputHandlerFn fn)

// Runs timer each N frames
ActorFlags Actor::Timer; 	
ActorBuilder ActorBuilder::timer(uint8_t count, TimerFn fn)

// Spawn from ActorBuilder:
Optional<EntityId> ActorBuilder::spawn() const;
void ActorBuilder::spawnToId(EntityId &id)

// Function types
using ColliderFn = void (*)(const EntityId &receiver, const EntityId peer);
using InputHandlerFn = void (*)(const EntityId &receiver, const RawInput &input);
using TimerFn = void (*)(const EntityId &receiver);

// Macros to define lambdas like MACRO_NAME_FN { ... }
#define COLLIDER_FN +[](const EntityId &receiver, const EntityId peer)
#define INPUTHANDLER_FN +[](const EntityId &receiver, const RawInput &rawInput)
#define TIMER_FN +[](const EntityId &receiver)

// Use 'if (TEST_HIT) {}' inside COLLIDER_FN
#define TEST_HIT TestHit(receiver, peer)

// NB: The frame is NOT cleared by the engine so you would probably need an Actor for the background

// Pre-built ActorsBuilders in 'A' namespace:
Engine::ActorBuilder A::Background()                       // Fills screen with spaces
Engine::ActorBuilder A::PlayerChar(const char *c = "#")    // Movable player displayed as char

// Accessing Actors
// NB: store a custom reference on Actor construction like ... .make().tag(10) to reuse it later
// NB: this will silently return a dummy value if id is not available
void Engine::setTag(EntityId id, Tag tag)
Actor & Engine::getActorByTag(const Tag tag)

// Get Actor by entity id
Actor & Engine::getActor(EntityId id)

// Shared data
struct SharedData
{
    union Element {
        uint8_t uint8[4];
        int8_t int8[4];
        uint16_t uint16[2];
        int16_t int16[2];
        int32_t uint32;
        int32_t int32;
        void* ptr;
    };
    const char *constStrings[Setup::SharedStrings];
    
    // NB: no direct access to elements array here
    
    Element getElement(int index) const;
    void setElement(int index, const Element &in);
};
SharedData Engine::sharedData;

// Other macros
#define RWE Engine::get() // Engine singleton

```
