#pragma once

#include "../SparCraft.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

namespace SparCraft
{
    namespace GUITools
    {
        const int FLIP_VERTICAL = 1;
        const int FLIP_HORIZONTAL = 2;

        void DrawLine(const Position & p1, const Position & p2, const float thickness, const GLfloat * rgba);
        void DrawString(const Position & p, const std::string & text, const GLfloat * rgba);
        void DrawChar(const Position & tl, const Position & br, char ch, const GLfloat * rgba);
        void DrawCircle(const Position & p, float r, int num_segments);
        void DrawTexturedRect(const Position & tl, const Position & br, const int & textureID, const GLfloat * rgba); 
        void DrawPercentageRect(const Position & tl, const Position & br, double percentage, const GLfloat * rgba, const GLfloat * bgrgba);
        void DrawRect(const Position & tl, const Position & br, const GLfloat * rgba); 
        void DrawRectGradient(const Position & tl, const Position & br, const GLfloat * rgbaLeft, const GLfloat * rgbaRight); 
        void SetColor(const GLfloat * src, GLfloat * dest);
        void SetColor(const GLfloat * src, GLfloat * dest);
    }
}
