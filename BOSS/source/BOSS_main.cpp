#include "BOSS.h"
#include "BOSSParameters.h"
#include "BOSSExperiments.h"

#include "CImg/CImg.h"

using namespace BOSS;
using namespace cimg_library;

int main2() 
{
    CImg<unsigned char> image("logo.bmp");
    CImg<unsigned char> visu(500,400,1,3,0);

    const unsigned char red[]   = {255,0,0};
    const unsigned char green[] = {0,255,0};
    const unsigned char blue[]  = {0,0,255};

    image.blur(2.5);
    CImgDisplay main_disp(image,"Click a point");
    CImgDisplay draw_disp(visu,"Intensity profile");

    while (!main_disp.is_closed() && !draw_disp.is_closed()) 
    {
        main_disp.wait();
        if (main_disp.button() && main_disp.mouse_y()>=0) 
        {
            const int y = main_disp.mouse_y();
            visu.fill(0).draw_graph(image.get_crop(0,y,0,0,image.width()-1,y,0,0),red,1,1,0,255,0);
            visu.draw_graph(image.get_crop(0,y,0,1,image.width()-1,y,0,1),green,1,1,0,255,0);
            visu.draw_graph(image.get_crop(0,y,0,2,image.width()-1,y,0,2),blue,1,1,0,255,0).display(draw_disp);
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    // Initialize all the BOSS internal data
    BOSS::init();

    // Read in the config parameters that will be used for experiments
    BOSS::BOSSParameters::Instance().ParseParameters("BOSS_Config.txt");
    
    // Run the experiments
    BOSS::Experiments::RunExperiments("BOSS_Config.txt");
    
    return 0;
}

