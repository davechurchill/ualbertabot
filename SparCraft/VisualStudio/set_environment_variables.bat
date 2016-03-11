@echo off
echo Set SparCraft Windows Environment Variables
echo Please edit this file before running for the first time

pause

setx BWAPI_DIR G:\bwapi\bwapi\include
setx BWTA_DIR G:\AIUR\
setx BOOST_DIR E:\boost_1_56_0
setx EXTERNAL_LIB_DIR c:\libraries\external_lib_dir
setx SDL_DIR C:\libraries\SDL-1.2.15
setx SDL_IMAGE_DIR C:\libraries\SDL_image-1.2.12
setx SDL_GFX_DIR c:\libraries\SDL_gfx-2.0.23

pause