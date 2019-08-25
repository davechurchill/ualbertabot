# AKBot - StarCraft AI Competition Bot

This bot based on the original UAlberta Bot written by David Churchill (dave.churchill@gmail.com)

Author: Andrey Kurdyumov (kant2002@gmail.com)

## Dependencies

For mamaging dependencies used vcpkg

    # install dependencies
    vcpkg install sdl2 sdl2-image

This is additional dependencies to Dave Churchill's bot.

Create environment variable `VCPKG_CMAKE_BUILDFILE` which point to the file `C:\vcpkg\scripts\buildsystems\vcpkg.cmake` or such.
See https://github.com/Microsoft/vcpkg/blob/master/docs/users/integration.md#cmake-toolchain-file-recommended-for-open-source-cmake-projects 
for explanation of purpose of the file

## How to build

For testing locally you need only VS2017, for these you should use Debug and Release solution configuration.
For actual building go to `UAlbertaBot\VisualStudio` and open `AKBot.sln`

You can build using CMake

    mkdir build
    cd build
    cmake -G "Visual Studio 16 2019" ..

## Documentation
Please check out the [UAlbertaBot Wiki](https://github.com/davechurchill/ualbertabot/wiki) for full instructions and documentation
and [Wiki of current project](https://github.com/kant2002/ualbertabot/wiki) for additional information which specific for this project!

## Additional code configuration

### Defines
- *BOSS_ASSERT_ENABLE* This define allow assertion in the BOSS module. 
	In original source code it is hardcoded in the source code which results in the very 
	slow debug builds starting from middle game. Most of the time this is not requried, only helpful when needed debug BOSS.
- *SPARCRAFT_ASSERT_ENABLE* This define allow assertion in the Sparcraft code.

## Testing


	.\testing\compare_estimator.ps1

	.\testing\run_games.ps1 -Bot1 AKBot_Sparcraft -Bot2 PurpleWave -Games 5
	.\testing\run_games.ps1 -Bot1 AKBot_Sparcraft -Bot2 100382319 -Games 5
	.\testing\run_games.ps1 -Bot1 AKBot_Sparcraft -Bot2 "Hao Pan" -Games 5
	.\testing\run_games.ps1 -Bot1 AKBot_Sparcraft -Bot2 OpprimoBot -Games 5
	.\testing\run_games.ps1 -Bot1 AKBot_Sparcraft -Bot2 "Marine Hell" -Games 5
	.\testing\run_games.ps1 -Bot1 AKBot_Sparcraft -Bot2 Goliat -Games 5

	
	.\testing\run_games.ps1 -Bot1 AKBot_FAP -Bot2 PurpleWave -Games 5
	.\testing\run_games.ps1 -Bot1 AKBot_FAP -Bot2 100382319 -Games 5
	.\testing\run_games.ps1 -Bot1 AKBot_FAP -Bot2 "Hao Pan" -Games 5
	.\testing\run_games.ps1 -Bot1 AKBot_FAP -Bot2 OpprimoBot -Games 5
	.\testing\run_games.ps1 -Bot1 AKBot_FAP -Bot2 "Marine Hell" -Games 5
	.\testing\run_games.ps1 -Bot1 AKBot_FAP -Bot2 Goliat -Games 5