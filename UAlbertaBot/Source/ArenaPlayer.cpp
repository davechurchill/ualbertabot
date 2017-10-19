#include "ArenaPlayer.h"

using namespace UAlbertaBot;

ArenaPlayer::ArenaPlayer()
    : _name("ArenaPlayer_Base")
{
    
}

const std::string & ArenaPlayer::getName() const
{
    return _name;
}