// ── Bubble.cpp ─────────────────────────────────────────────
#include "Bubble.h"
#include <cmath>
#include <cstdio>
#include <algorithm>

// ════════════════════════════════════════════════════════════
//  RenderComp
// ════════════════════════════════════════════════════════════
bool Bubble::RenderComp::tickPop(float dt) {
    popScale -= Bubble::POP_SPEED * dt;
    return popScale > 0.f;  // true = still animating
}

void Bubble::RenderComp::draw(float x, float y, float radius,
                               BubbleType type, int scoreValue) const {
    float r  = radius * popScale;
    int   cx = (int)x, cy = (int)y;

    DrawCircle(cx, cy, r, color);

    // Type-specific ring decoration
    if      (type == BubbleType::SLOW) DrawCircleLines(cx, cy, r+4, SKYBLUE);
    else if (type == BubbleType::POP)  DrawCircleLines(cx, cy, r+4, PURPLE);

    // Glossy sheen
    DrawCircle((int)(x - r*.25f), (int)(y - r*.28f), r*.35f, {255,255,255,70});

    // Darker outline
    Color out = {(unsigned char)(color.r/2),
                 (unsigned char)(color.g/2),
                 (unsigned char)(color.b/2), 200};
    DrawCircleLines(cx, cy, r, out);

    // Label (score or type name)
    if (!popping && r > 22.f) {
        const char* label = nullptr;
        if      (type == BubbleType::SLOW) label = "SLOW";
        else if (type == BubbleType::POP)  label = "POP";

        char buf[8];
        if (!label) {
            if (scoreValue >= 0) snprintf(buf, sizeof(buf), "+%d", scoreValue);
            else                 snprintf(buf, sizeof(buf), "%d",  scoreValue);
            label = buf;
        }
        int fs = std::max(10, (int)(r * .45f));
        int tw = MeasureText(label, fs);
        DrawText(label, cx - tw/2, cy - fs/2, fs, WHITE);
    }
}

void Bubble::RenderComp::reset(Color c) {
    color    = c;
    popScale = 1.f;
    popping  = false;
}

// ════════════════════════════════════════════════════════════
//  MetaComp
// ════════════════════════════════════════════════════════════
void Bubble::MetaComp::reset(BubbleType t, int sv) {
    type       = t;
    scoreValue = sv;
}

// ════════════════════════════════════════════════════════════
//  Bubble
// ════════════════════════════════════════════════════════════
void Bubble::init(float x, float y, float radius, Color color,
                  float vx, float vy, BubbleType type, int scoreValue) {
    revive(x, y, radius, vx, vy);  // Entity::revive sets position + caps velocity
    rend.reset(color);
    meta.reset(type, scoreValue);
}

void Bubble::update(float dt) {
    if (!alive) return;

    if (rend.popping) {
        if (!rend.tickPop(dt)) alive = false;
        return;
    }

    x += vx * dt;
    y += vy * dt;
    wrapAround();  // Entity::wrapAround uses Config::SCREEN_W/H
}

void Bubble::draw() const {
    if (!alive) return;
    rend.draw(x, y, radius, meta.type, meta.scoreValue);  // delegate to RenderComp
}

bool Bubble::isClicked(float mx, float my) const {
    if (!alive || rend.popping) return false;
    float dx = mx - x, dy = my - y;
    return (dx*dx + dy*dy) <= (radius * radius);
}

void Bubble::pop() {
    if (!rend.popping) rend.popping = true;
}

void Bubble::makeNormal(Color c, int sv) {
    meta.reset(BubbleType::NORMAL, sv);
    rend.color = c;
}

void Bubble::boostSpeed(float factor) {
    setVelocity(vx * factor, vy * factor);  // Entity::setVelocity caps speed
}
