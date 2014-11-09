#include "GUITools.h"
#include "GUI.h"

namespace BOSS
{
namespace GUITools
{
    void DrawString(const Position & p, const std::string & text, const GLfloat * rgba) 
    {
        glPushMatrix();
            glColor4fv(rgba);
            glRasterPos2i(p.x(), p.y());
            glutBitmapString(GLUT_BITMAP_HELVETICA_10, (const unsigned char*)text.c_str());
        glPopMatrix();
    }

    void DrawStringLarge(const Position & p, const std::string & text, const GLfloat * rgba) 
    {
        glPushMatrix();
            glColor4fv(rgba);
            glRasterPos2i(p.x(), p.y());
            glutBitmapString(GLUT_BITMAP_9_BY_15, (const unsigned char*)text.c_str());
        glPopMatrix();
    }

    void DrawStringWithShadow(const Position & p, const std::string & text, const GLfloat * rgba) 
    {
        const GLfloat black[4] = {0.0f, 0.0f, 0.0f, 1.0f};
        DrawString(p + Position(1,1), text, black);
        DrawString(p, text, rgba);
    }

    void DrawStringLargeWithShadow(const Position & p, const std::string & text, const GLfloat * rgba) 
    {
        const GLfloat black[4] = {0.0f, 0.0f, 0.0f, 1.0f};
        DrawStringLarge(p + Position(1,1), text, black);
        DrawStringLarge(p, text, rgba);
    }

    void DrawLine(const Position & p1, const Position & p2, const float thickness, const GLfloat * rgba)
    {
        glPushMatrix();
            glLineWidth(thickness); 
            glColor4fv(rgba);
            glBegin(GL_LINES);
                glVertex2i(p1.x(),p1.y());
                glVertex2i(p2.x(),p2.y());
            glEnd();
        glPopMatrix();
    }

    void DrawCircle(const Position & pos, float r, int num_segments) 
    { 
        float theta = 2 * (float)3.1415926 / float(num_segments); 
        float c = cosf(theta);//precalculate the sine and cosine
        float s = sinf(theta);
        float t;

        float x = r;//we start at angle = 0 
        float y = 0; 

        glBegin(GL_LINE_LOOP); 
            for(int ii = 0; ii < num_segments; ii++) 
            { 
                glVertex2f(x + pos.x(), y + pos.y());//output vertex 

                //apply the rotation matrix
                t = x;
                x = c * x - s * y;
                y = s * t + c * y;
            } 
        glEnd(); 
    }

    void DrawRect(const Position & tl, const Position & br, const GLfloat * rgba)
    {
        glPushMatrix();
            glColor4fv(rgba);
            glBegin(GL_QUADS);
                glVertex2i(tl.x(),tl.y());
                glVertex2i(br.x(),tl.y());
                glVertex2i(br.x(),br.y());
                glVertex2i(tl.x(),br.y());
            glEnd();
        glPopMatrix();
    }

    void DrawRectGradient(const Position & tl, const Position & br, const GLfloat * rgbaLeft, const GLfloat * rgbaRight)
    {
        glPushMatrix();
            glBegin(GL_QUADS);
                glColor4fv(rgbaLeft);  glVertex2i(tl.x(),tl.y());
                glColor4fv(rgbaRight); glVertex2i(br.x(),tl.y());
                glColor4fv(rgbaRight); glVertex2i(br.x(),br.y());
                glColor4fv(rgbaLeft);  glVertex2i(tl.x(),br.y());
            glEnd();
        glPopMatrix();
    }
  
    void DrawTexturedRect(const Position & tl, const Position & br, const int & textureID, const GLfloat * rgba)
    {
        glPushMatrix();
            glEnable( GL_TEXTURE_2D );
                glColor4fv(rgba);
                glBindTexture( GL_TEXTURE_2D, textureID );
                glBegin( GL_QUADS );
                    glTexCoord3d(0.0,0.0,.5); glVertex2i(tl.x(),tl.y());
                    glTexCoord3d(1.0,0.0,.5); glVertex2i(br.x(),tl.y());
                    glTexCoord3d(1.0,1.0,.5); glVertex2i(br.x(),br.y());
                    glTexCoord3d(0.0,1.0,.5); glVertex2i(tl.x(),br.y());
                glEnd();
            glDisable( GL_TEXTURE_2D );
        glPopMatrix();
    }

    void DrawIconAndText(const Position & tl, const Position & br, const int & textureID, const int & textureID2, const GLfloat * rgba)
    {
        GLfloat white[4] = {1.0f, 1.0f, 1.0f, 1.0f};
        Position iconSize = br - tl;
        Position statusNumPos = tl + Position(-iconSize.scale(0.15f).x(), iconSize.scale(0.4f).y());
        GUITools::DrawTexturedRect(tl, br, textureID, rgba);
        GUITools::DrawTexturedRect(statusNumPos, statusNumPos + iconSize.scale(0.65f), textureID2, rgba); 
    }

    void SetColor(const GLfloat * src, GLfloat * dest)
    {
        std::copy(src, src + 4, dest);
    }
    
    Uint32 get_pixel32( SDL_Surface *surface, int x, int y )
    {
        //Convert the pixels to 32 bit
        Uint32 *pixels = (Uint32 *)surface->pixels;

        //Get the requested pixel
        return pixels[ ( y * surface->w ) + x ];
    }

    void put_pixel32( SDL_Surface *surface, int x, int y, Uint32 pixel )
    {
        //Convert the pixels to 32 bit
        Uint32 *pixels = (Uint32 *)surface->pixels;

        //Set the pixel
        pixels[ ( y * surface->w ) + x ] = pixel;
    }

    SDL_Surface *flip_surface( SDL_Surface *surface, int flags )
    {
        //Pointer to the soon to be flipped surface
        SDL_Surface *flipped = NULL;

        //If the image is color keyed
        if( surface->flags & SDL_SRCCOLORKEY )
        {
            flipped = SDL_CreateRGBSurface( SDL_SWSURFACE, surface->w, surface->h, surface->format->BitsPerPixel, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, 0 );
        }
        //Otherwise
        else
        {
            flipped = SDL_CreateRGBSurface( SDL_SWSURFACE, surface->w, surface->h, surface->format->BitsPerPixel, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask );
        }

        //If the surface must be locked
        if( SDL_MUSTLOCK( surface ) )
        {
            //Lock the surface
            SDL_LockSurface( surface );
        }

        //Go through columns
        for( int x = 0, rx = flipped->w - 1; x < flipped->w; x++, rx-- )
        {
            //Go through rows
            for( int y = 0, ry = flipped->h - 1; y < flipped->h; y++, ry-- )
            {
                //Get pixel
                Uint32 pixel = get_pixel32( surface, x, y );

                //Copy pixel
                if( ( flags & FLIP_VERTICAL ) && ( flags & FLIP_HORIZONTAL ) )
                {
                    put_pixel32( flipped, rx, ry, pixel );
                }
                else if( flags & FLIP_HORIZONTAL )
                {
                    put_pixel32( flipped, rx, y, pixel );
                }
                else if( flags & FLIP_VERTICAL )
                {
                    put_pixel32( flipped, x, ry, pixel );
                }
            }
        }

        //Unlock surface
        if( SDL_MUSTLOCK( surface ) )
        {
            SDL_UnlockSurface( surface );
        }

        //Copy color key
        if( surface->flags & SDL_SRCCOLORKEY )
        {
            SDL_SetColorKey( flipped, SDL_RLEACCEL | SDL_SRCCOLORKEY, surface->format->colorkey );
        }

        //Return flipped surface
        return flipped;
    }
}
}
