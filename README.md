# AKBot - StarCraft AI Competition Bot

This bot based on the original UAlberta Bot written by David Churchill (dave.churchill@gmail.com)

Author: Andrey Kurdyumov (kant2002@gmail.com)

## Dependencies

For mamaging dependencies used vcpkg

    # install dependencies
    vcpkg install boost
    vcpkg install boost-di

This is additional dependencies to Dave Churchill's bot.

## How to build

For testing locally you need only VS2017, for these you should use Debug and Release solution configuration.
If you want made submission to SSCAIT, you have to install VS2013 Update 5 from https://www.visualstudio.com/vs/older-downloads/.
Community version should be enough for building. To make install minimal, you could uncheck all ceckboxes in the installer.

For actual building go to `UAlbertaBot\VisualStudio` and open `UAlbertaBot.sln`

## Documentation
Please check out the [Wiki](https://github.com/davechurchill/ualbertabot/wiki) for full instructions and documentation!

## Additional code configuration

### Defines
- *BOSS_ASSERT_ENABLE* This define allow assertion in the BOSS module. 
	In original source code it is hardcoded in the source code which results in the very 
	slow debug builds starting from middle game. Most of the time this is not requried, only helpful when needed debug BOSS.
- *SPARCRAFT_ASSERT_ENABLE* This define allow assertion in the Sparcraft code.