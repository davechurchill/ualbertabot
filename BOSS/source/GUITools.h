#pragma once

#include "Common.h"

#ifdef BOSS_ENABLE_GUI

#include "Position.hpp"
#include "freeglut.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_gfxPrimitives.h>

namespace BOSS
{
    namespace GUITools
    {
        const int FLIP_VERTICAL = 1;
        const int FLIP_HORIZONTAL = 2;

        void DrawLine(const Position & p1, const Position & p2, const float thickness, const GLfloat * rgba);
        void DrawString(const Position & p, const std::string & text, const GLfloat * rgba);
        void DrawStringLarge(const Position & p, const std::string & text, const GLfloat * rgba);
        void DrawStringWithShadow(const Position & p, const std::string & text, const GLfloat * rgba);
        void DrawStringLargeWithShadow(const Position & p, const std::string & text, const GLfloat * rgba);
        void DrawCircle(const Position & p, float r, int num_segments);
        void DrawTexturedRect(const Position & tl, const Position & br, const int & textureID, const GLfloat * rgba); 
        void DrawRect(const Position & tl, const Position & br, const GLfloat * rgba); 
        void DrawRectGradient(const Position & tl, const Position & br, const GLfloat * rgbaLeft, const GLfloat * rgbaRight); 
        void SetColor(const GLfloat * src, GLfloat * dest);
        void SetColor(const GLfloat * src, GLfloat * dest);

        SDL_Surface *flip_surface( SDL_Surface *surface, int flags );
        Uint32 get_pixel32( SDL_Surface *surface, int x, int y );
        void put_pixel32( SDL_Surface *surface, int x, int y, Uint32 pixel );
    }
}

#endif