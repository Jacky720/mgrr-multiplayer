# Metal Gear Rising Revengeance Multiplayer Mod
Mod is published by RuslanchikX at NexusMods, you likely want to download from there: [MGRR 2 Players Mode](https://www.nexusmods.com/metalgearrisingrevengeance/mods/797)

This mod enables local multiplayer of Metal Gear Rising by spawning additional player entities through [Frouk's plugin SDK](https://github.com/Frouk3/mgr-plugin-sdk/) and overriding their inputs to handle each controller separately.

## Mod installation
Place the generated .asi file in your METAL GEAR RISING REVENGEANCE/scripts folder and use [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader) or [Rising Mod Manager](https://www.nexusmods.com/metalgearrisingrevengeance/mods/449) to hook it.
Then copy the rising_multiplayer folder from the source base directory to your GameData folder, this contains all assets for the custom UI

An additional ple010_us.dat file included on NexusMods fixes the voice soundbanks for DLC characters.


## Development setup
Install [Frouk's plugin SDK](https://github.com/Frouk3/mgr-plugin-sdk/).
Launch the solution in Visual Studio and compile as "Release x86".


