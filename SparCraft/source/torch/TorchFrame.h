#pragma once

#include <cassert>
#include <cstdio>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <iostream>

#include "TorchRefcount.h"

#ifdef _MSC_VER
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef int int32_t;
#endif

//TODO Check types !

namespace replayer {

struct Order {
    int32_t first_frame; // first frame number where order appeared

    int32_t type;       // see BWAPI::Orders::Enum
    int32_t targetId;
    int32_t targetX, targetY;

    bool operator==(const Order& o) const {
        return type == o.type && targetId == o.targetId
                && targetX == o.targetX && targetY == o.targetY;
    }
};

struct Unit {
    int32_t id, x, y;
    int32_t health, shield, energy;
    int32_t maxCD, groundCD, airCD;
    bool idle, visible;
    int32_t type, armor, shieldArmor, size;

    int32_t groundATK, airATK;
    int32_t groundDmgType, airDmgType;
    int32_t groundRange, airRange;

    std::vector<Order> orders;

    double velocityX, velocityY;
};

std::ostream& operator<<(std::ostream& out, const Unit& o);
std::istream& operator>>(std::istream& in, Unit& o);

struct Bullet {
    int32_t type, x, y;
};

std::ostream& operator<<(std::ostream& out, const Bullet& o);
std::istream& operator>>(std::istream& in, Bullet& o);

struct Action {         // corresponds to a torchcraft message
    std::vector<int32_t> action;
    int32_t uid;
    int32_t aid;
};

class Frame : public RefCounted {
    public:
    std::unordered_map<int32_t, std::vector<Unit>> units;
    std::unordered_map<int32_t, std::vector<Action>> actions;
    std::vector<Bullet> bullets;
    int reward;
    int is_terminal;

    Frame() : RefCounted() {
        reward = 0;
        is_terminal = 0;
    }
    Frame(const Frame& o) : RefCounted(),
        units(o.units), actions(o.actions), bullets(o.bullets)
    {
        reward = o.reward;
        is_terminal = o.is_terminal;
    }

    void filter(int32_t x, int32_t y, Frame& o) const {
        auto inRadius = [x, y](int32_t ux, int32_t uy) {
            return (x/8-ux)*(x/8-ux) + (y/8-uy)*(y/8-uy) <= 20 * 4 * 20 * 4;
        };

        for(auto& player : units) {
            o.units[player.first] = std::vector<Unit>();
            for(auto& unit : player.second) {
                if (inRadius(unit.x, unit.y)){
                    o.units[player.first].push_back(unit);
                }
            }
        }
        for(auto& bullet : bullets) {
            if (inRadius(bullet.x, bullet.y)) {
                o.bullets.push_back(bullet);
            }
        }
    }

    void combine(const Frame& next_frame) {
        // For units, accumulate presence and commands
        for(auto& player : next_frame.units) {
            assert (units.count(player.first) != 0);

            // Build dictionary of uid -> position in current frame unit vector
            std::unordered_map<int32_t, int32_t> idx;
            for(unsigned i = 0; i < units[player.first].size(); i++) {
                idx[units[player.first][i].id] = i;
            }
            // Iterate over units in next frame
            for(auto& unit : player.second) {
                if (idx.count(unit.id) == 0) {
                    // Unit wasn't in current frame, add it
                    units[player.first].push_back(unit);
                } else {
                    int32_t i = idx[unit.id];
                    // Take unit state from next frame but accumulate orders
                    // so as to have a vector of all the orders taken
                    std::vector<Order> ords
                        = std::move(units[player.first][i].orders);
                    ords.reserve(ords.size() + unit.orders.size());
                    for (auto& ord : unit.orders) {
                        if (ords.empty() || !(ord == ords.back())) {
                            ords.push_back(ord);
                        }
                    }
                    units[player.first][i] = unit;
                    units[player.first][i].orders = std::move(ords);
                }
            }
        }
        // For other stuff, simply keep that of next_frame
        actions = next_frame.actions;
        bullets = next_frame.bullets;
        reward = next_frame.reward;
        is_terminal = next_frame.is_terminal;
    }
};
std::ostream& operator<<(std::ostream& out, const Frame& o);
std::istream& operator>>(std::istream& in, Frame& o);

} // replayer