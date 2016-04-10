#include "GUITools.h"
#include "GUI.h"

namespace SparCraft
{
namespace GUITools
{
    std::string GetClipboardText()
    {
        std::string text;

        #ifdef WIN32

            if (! OpenClipboard(nullptr))
            {
                return text;
            }

            // Get handle of clipboard object for ANSI text
            HANDLE hData = GetClipboardData(CF_TEXT);
            if (hData == nullptr)
            {
                return text; 
            }

            // Lock the handle to get the actual text pointer
            char * pszText = static_cast<char*>( GlobalLock(hData) );
            if (pszText == nullptr)
            {
                return text;
            }

            // Save text in a string class instance
            text = std::string( pszText );

            // Release the lock
            GlobalUnlock( hData );

            // Release the clipboard
            CloseClipboard();
      
        #endif

        return text;
    }

    void DrawString(const Position & p, const std::string & text, const GLfloat * rgba) 
    {
        Position origin(p);
        Position fontSize(8,8);

        glPushMatrix();
        glColor4fv(rgba);
        glEnable( GL_TEXTURE_2D );
        glBindTexture( GL_TEXTURE_2D, GUI::TextureFont );
        glBegin( GL_QUADS );

            int linePos = 0;
            for (size_t i(0); i < text.length(); ++i)
            {
                if (text[i] == '\n') 
                { 
                    origin = Position(p.x(), origin.y() + fontSize.y()); 
                    linePos = 0;
                }
                else
                {
                    Position charStart = Position(origin.x() + linePos*fontSize.x(), origin.y() - fontSize.y());
                    Position charEnd = charStart + fontSize;

                    char cc = text[i];
                    float xPos = ((cc % 16) / 16.0f);
                    float yPos = (cc >> 4) / 16.0f;
                    float del = 1/16.0f;
                    glTexCoord2f(xPos,yPos);            glVertex2i(charStart.x(),charStart.y());
                    glTexCoord2f(xPos+del,yPos);        glVertex2i(charEnd.x(),charStart.y());
                    glTexCoord2f(xPos+del,yPos+del);    glVertex2i(charEnd.x(),charEnd.y());
                    glTexCoord2f(xPos,yPos+del);        glVertex2i(charStart.x(),charEnd.y());
                    
                    linePos++;    
                }
            }
        glEnd();
        glDisable( GL_TEXTURE_2D );
        glPopMatrix();
    }

    void DrawStringLarge(const Position & p, const std::string & text, const GLfloat * rgba) 
    {
        DrawString(p, text, rgba);
    }

    void DrawStringWithShadow(const Position & p, const std::string & text, const GLfloat * rgba) 
    {
        DrawString(p, text, rgba);
    }

    void DrawStringLargeWithShadow(const Position & p, const std::string & text, const GLfloat * rgba) 
    {
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
                    glTexCoord2f(0.0,0.0); glVertex2i(tl.x(),tl.y());
                    glTexCoord2f(1.0,0.0); glVertex2i(br.x(),tl.y());
                    glTexCoord2f(1.0,1.0); glVertex2i(br.x(),br.y());
                    glTexCoord2f(0.0,1.0); glVertex2i(tl.x(),br.y());
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

}
}
