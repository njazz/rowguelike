#pragma once

#ifdef ARDUINO

#include "rowguelike.hpp"

using namespace rwe;

#include <LiquidCrystal.h>

struct RowguelikeLCD
{
    static constexpr int b_select = 641;
    static constexpr int b_left = 411;
    static constexpr int b_right = 0;
    static constexpr int b_up = 100;
    static constexpr int b_down = 257;

    LiquidCrystal lcd;
    RowguelikeLCD(uint8_t p1,uint8_t p2,uint8_t p3,uint8_t p4,uint8_t p5,uint8_t p6)
        : lcd(p1, p2, p3, p4, p5, p6)
    {}

    void setup(uint8_t w = 16, uint8_t h = 2)
    {
        lcd.begin(w, h);

        //
        RWE.drawContext.ctx = &lcd;
        RWE.drawContext.customCharacters = 8;
        RWE.drawContext.peerClearAll = +[](void *ctx) {
            if (!ctx)
                return;

            // NB: run this only for custom character rendering
            if (!RWE.drawContext.disableDirectBufferDraw)
                return;

            auto lcd_ = (LiquidCrystal *) ctx;
            lcd_->clear();
        };
        RWE.drawContext.peerDefineChar = +[](void *ctx, uint8_t idx, const CustomCharacter c) {
            if (!ctx)
                return;

            auto lcd_ = (LiquidCrystal *) ctx;
            lcd_->createChar(idx, c.data);
        };
        RWE.drawContext.peerAddChar = +[](void *ctx, int8_t x, int8_t y, const uint8_t id) {
            if (!ctx)
                return;

            auto lcd_ = (LiquidCrystal *) ctx;
            lcd_->setCursor(x, y);
            lcd_->write(id);
        };
        // direct draw command for disableDirectBufferDraw==1 mode:
        RWE.drawContext.peerAddText = +[](void *ctx, int8_t x, int8_t y, const char *txt) {
            if (!ctx)
                return;

            // NB: run this only for custom character rendering mode enabled
            if (!RWE.drawContext.disableDirectBufferDraw)
                return;

            auto lcd_ = (LiquidCrystal *) ctx;
            lcd_->setCursor(x, y);
            lcd_->print(txt);
        };
    }

    // TODO: move

    struct Momentary
    {
        bool last{false};
        bool current{false};
        void set(bool b)
        {
            last = current;
            current = b;
        }
        bool get() const { return current && current != last; }
    };

    Momentary m_select{};
    Momentary m_up{};
    Momentary m_down{};
    Momentary m_left{};
    Momentary m_right{};

    void loop(uint16_t delayTime = 150)
    {
        auto v = analogRead(0);

        m_select.set(v == b_select);
        m_up.set(v == b_up);
        m_down.set(v == b_down);
        m_left.set(v == b_left);
        m_right.set(v == b_right);

        RWE.rawInput.select = m_select.get();
        RWE.rawInput.up = m_up.get();
        RWE.rawInput.down = m_down.get();
        RWE.rawInput.left = m_left.get();
        RWE.rawInput.right = m_right.get();

        // put your main code here, to run repeatedly:
        RWE.runLoop();

        if (!RWE.drawContext.disableDirectBufferDraw) {
            lcd.setCursor(0, 0);
            lcd.print(RWE.drawContext.buffer[0]);
            lcd.setCursor(0, 1);
            lcd.print(RWE.drawContext.buffer[1]);
        }

        delay(delayTime);
    }
};

#endif
