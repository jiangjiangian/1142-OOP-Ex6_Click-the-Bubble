// File: EventBus.h — 事件總線 (Observer pattern) 的輕量實作
// ── EventBus.h ─────────────────────────────────────────────
// EventBus is a Singleton; call EventBus::get() anywhere.
// Usage:
//   EventBus::get().subscribe<ScoreChangedEvent>([](const ScoreChangedEvent& e){...});
//   EventBus::get().publish(ScoreChangedEvent{newScore, delta});
// ──────────────────────────────────────────────────────────
#ifndef EVENTBUS_H
#define EVENTBUS_H

#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <memory>

// ── Type-erased handler base ─────────────────────────────────
class IHandlerBase {
public:
    virtual ~IHandlerBase() = default;
};

template<typename T>
class TypedHandler : public IHandlerBase {
public:
    explicit TypedHandler(std::function<void(const T&)> fn) : fn(std::move(fn)) {}
    void call(const T& e) const { fn(e); }
private:
    std::function<void(const T&)> fn;
};

// ── EventBus Singleton ───────────────────────────────────────
// Low coupling: publishers and subscribers never reference each other directly.
// Both sides only know about EventBus and the event struct type.
class EventBus {
public:
    static EventBus& get() {
        static EventBus instance;
        return instance;
    }

    // Delete copy/move — Singleton
    EventBus(const EventBus&)            = delete;
    EventBus& operator=(const EventBus&) = delete;

    template<typename T>
    void subscribe(std::function<void(const T&)> fn) {
        handlers[std::type_index(typeid(T))]
            .push_back(std::make_shared<TypedHandler<T>>(std::move(fn)));
    }

    template<typename T>
    void publish(const T& event) {
        auto it = handlers.find(std::type_index(typeid(T)));
        if (it == handlers.end()) return;
        for (const auto& h : it->second)
            static_cast<TypedHandler<T>*>(h.get())->call(event);
    }

    // Call on game restart to avoid stale lambda captures
    void clear() { handlers.clear(); }

private:
    EventBus() = default;
    std::unordered_map<
        std::type_index,
        std::vector<std::shared_ptr<IHandlerBase>>
    > handlers;
};

#endif // EVENTBUS_H
