UAlbertaBot - StarCraft AI Competition Bot

Author:  David Churchill (dave.churchill@gmail.com)
         University of Alberta
Type:    .dll
Race:    Protoss

Setup:

Create Windows Environment Variables As Follows:
- BWAPI_DIR - Location of BWAPI libraries
- BOOST_DIR - Location of BOOST 1.47 libraries (or higher)

Visual Studio Compilation:

- Open UAlbertaBot/VisualStudio/UAlbertaBot.sln Visual Studio 2013
- Ensure 'Platform Toolset' for each project is 'v90' or '2008' (choose not to upgrade project)
- Select 'Release' build mode
- Compile Project 'UAlbertaBot'
