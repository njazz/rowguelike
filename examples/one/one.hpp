/// @file one.hpp
/// first test of a very basic game

#include "rowguelike.hpp"

using namespace rwe ;

void setupOne()
{
    auto character = A::PlayerChar("#")
                         //
                         .position(7, 0)
                         .hitpoints(9)
                         .input(INPUT_FN {
                             if (rawInput.select) {
                                 auto &p = RWE.getPosition(receiver);

                                 RWE //
                                     .make(Actor ::Move)
                                     .text("-")
                                     .position(p.x, p.y)
                                     .speed(1, 0)
                                     .hitpoints(1)
                                     .timer(
                                         2,
                                         TIMER_FN {
                                             auto p = RWE.getPosition(receiver);
                                             if (p.x >= 15) {
                                                 RWE.remove(receiver);
                                             }
                                         })
                                     .collider(
                                         1,
                                         COLLIDER_FN {
                                             HitPeer(receiver, peer);

                                             if (TEST_HIT) {
                                                 if (RWE.getPosition(peer).x <= 15)
                                                     RWE.getPosition(peer).x -= 1;
                                             }
                                         })
                                     .spawn();

                                 ;
                             }
                         })

        //

        ;

    auto target = RWE //
                      .make()
                      .text("9")
                      .position(15, 0)
                      .collider(1, HitPeer)
                      .hitpoints(9)
                      .timer(
                          8, TIMER_FN {
                              auto &t = RWE.getText(receiver);
                              auto &hp = RWE.getHitpoints(receiver);
                              if (hp.hp == 9)
                                  t.line[0] = "9";
                              if (hp.hp == 8)
                                  t.line[0] = "8";
                              if (hp.hp == 7)
                                  t.line[0] = "7";
                              if (hp.hp == 6)
                                  t.line[0] = "6";
                              if (hp.hp == 5)
                                  t.line[0] = "5";
                              if (hp.hp == 4)
                                  t.line[0] = "4";
                              if (hp.hp == 3)
                                  t.line[0] = "3";
                              if (hp.hp == 2)
                                  t.line[0] = "2";
                              if (hp.hp == 1)
                                  t.line[0] = "1";

                              auto &p = RWE.getPosition(receiver);
                              RWE //
                                  .make(Actor ::Move)
                                  .text("<-")
                                  .position(p.x, p.y)
                                  .speed(-1, 0)
                                  .hitpoints(1)
                                  .timer(
                                      2,
                                      TIMER_FN {
                                          auto p = RWE.getPosition(receiver);
                                          if (p.x <= 1) {
                                              RWE.remove(receiver);
                                          }
                                      })
                                  .collider(
                                      1,
                                      COLLIDER_FN {
                                          HitPeer(receiver, peer);

                                          if (TEST_HIT) {
                                              if (RWE.getPosition(peer).x <= 15)
                                                  RWE.getPosition(peer).x -= 1;
                                          }
                                      })
                                  .spawn();
                          });

    A::Background().spawn();
    character.spawn();
    target.spawn();
}
