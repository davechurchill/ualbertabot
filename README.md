**UAlbertaBot** - StarCraft AI Competition Bot - David Churchill (dave.churchill@gmail.com)

**Update: Feb 28, 2020**
- UAlbertaBot now uses VS2019
- UAlbertaBot now uses BWAPI 4.4.0. To get it to compile, you have to compile BWAPI.lib and BWAPIClient.lib with VS2019 and put them in the BWAPI_INSTALL_DIR/lib/ folder
- UAlbertaBot no longer uses BWTA - it is replaced with the custom BaseLocationManager from CommandCenter
- UAlbertaBot now uses the Client .exe compilation method instead of module DLL
- The bot will only work for the first game played after the .exe is run, so AUTO_RESTART won't work well. I'll fix this later
- You can download StarCraft 1.16.1 here: http://ftp.blizzard.com/pub/broodwar/patches/PC/BW-1161.exe

Please check out the [Wiki](https://github.com/davechurchill/ualbertabot/wiki) for full instructions and documentation!

NOTE: If you're looking for a StarCraft II bot please check out [CommandCenter](https://github.com/davechurchill/commandcenter/)
