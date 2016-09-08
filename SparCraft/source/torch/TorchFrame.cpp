#include "TorchFrame.h"

std::ostream& replayer::operator<<(std::ostream& out, const replayer::Unit& o) {
    out << o.id << " " << o.x << " " << o.y << " " << o.health << " "
        << o.shield << " " << o.energy << " " << o.maxCD << " "
        << o.groundCD << " " << o.airCD << " " << o.idle << " "
        << o.visible << " " << o.type << " " << o.armor << " "
        << o.shieldArmor << " " << o.size << " "
        << o.groundATK << " " << o.airATK << " " << o.groundDmgType << " "
        << o.airDmgType << " " << o.groundRange << " " << o.airRange << " ";

    out << o.orders.size() << " ";
    for (auto& c : o.orders) {
        out << c.first_frame << " "
            << c.type << " " << c.targetId << " "
            << c.targetX << " " << c.targetY << " ";
    }

    out << o.velocityX << " " << o.velocityY;
    return out;
}

std::istream& replayer::operator>>(std::istream& in, replayer::Unit& o) {
    in >> o.id >> o.x >> o.y >> o.health >> o.shield >> o.energy
        >> o.maxCD >> o.groundCD >> o.airCD >> o.idle >> o.visible >> o.type
        >> o.armor >> o.shieldArmor
        >> o.size >> o.groundATK >> o.airATK >> o.groundDmgType >> o.airDmgType
        >> o.groundRange >> o.airRange;

    int32_t n_orders;
    in >> n_orders;
    o.orders = std::vector<replayer::Order>(n_orders);
    for (int i = 0; i < n_orders; i++) {
        in >> o.orders[i].first_frame
            >> o.orders[i].type >> o.orders[i].targetId
            >> o.orders[i].targetX >> o.orders[i].targetY;
    }

    in >> o.velocityX >> o.velocityY;
    return in;
}

std::ostream& replayer::operator<<(std::ostream& out,
        const replayer::Bullet& o) {
    out << o.type << " " << o.x << " " << o.y;
    return out;
}

std::istream& replayer::operator>>(std::istream& in,
        replayer::Bullet& o) {
    in >> o.type >> o.x >> o.y;
    return in;
}

std::ostream& replayer::operator<<(std::ostream& out,
        const replayer::Frame& o) {
    out << o.units.size() << " ";
    for (auto& v : o.units) {
        out << v.first << " " << v.second.size() << " ";
        for (auto& u : v.second) {
            out << u << " ";
        }
    }
    out << o.actions.size() << " ";
    for (auto& v : o.actions) {
        out << v.first << " " << v.second.size() << " ";
        for (auto& u : v.second) {
            out << u.uid << " " << u.aid << " " << u.action.size() << " ";
            for(auto& a : u.action) {
                out << a << " ";
            }
        }
    }
    out << o.bullets.size() << " ";
    for (auto& b : o.bullets) {
        out << b << " ";
    }
    out << o.reward << " " << o.is_terminal;
    return out;
}

std::istream& replayer::operator>>(std::istream& in,
        replayer::Frame& o) {
    int32_t nPlayer, nUnits, nBullets, nActions;
    in >> nPlayer;
    for (int32_t i = 0; i < nPlayer; i++) {
        int32_t idPlayer;
        in >> idPlayer >> nUnits;
        o.units[idPlayer] = std::vector<replayer::Unit>(nUnits);
        for (int32_t j = 0; j < nUnits; j++) {
            in >> o.units[idPlayer][j];
        }
    }
    in >> nPlayer;
    for (int32_t i = 0; i < nPlayer; i++) {
        int32_t idPlayer;
        in >> idPlayer >> nActions;
        o.actions[idPlayer] = std::vector<replayer::Action>(nActions);
        for (int32_t j = 0; j < nActions; j++) {
            in >> o.actions[idPlayer][j].uid >> o.actions[idPlayer][j].aid;
            size_t sizeA;
            in >> sizeA;
            o.actions[idPlayer][j].action.resize(sizeA);

            for(size_t k = 0; k < sizeA; k++) {
                in >> o.actions[idPlayer][j].action[k];
            }
        }
    }
    in >> nBullets;
    o.bullets.resize(nBullets);
    for (int32_t i = 0; i < nBullets; i++) {
        in >> o.bullets[i];
    }
    in >> o.reward >> o.is_terminal;
    return in;
}