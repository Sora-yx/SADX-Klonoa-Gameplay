# SADX Klonoa Gameplay

WAHOO! Rupurudu!

![alt text](https://upload.wikimedia.org/wikipedia/en/thumb/5/5f/Klonoa.png/220px-Klonoa.png)

## About

Hm? What is this?

This is a SADX mod that replaces Sonic with Klonoa from Lunatea's Veil (PS2). This isn't a simple character skin, this ports the original Klonoa model with his original skeleton and animations.
It also implements Klonoa abilities such as shooting wind bullet, capturing enemy and doing super jump. 
Some objects (like rings and checkpoints) are also replaced with their Klonoa counterpart.
There is also a custom HUD and HP system (hearts) manually recreated from the Klonoa remaster assets.

Note that everything but the Klonoa model can be disabled in the mod settings, so if you just want a simple character skin, that is possible. If you want everything but the Klonoa objects, it is also possible.

Ideally, the long term goal of this mod would be to implement Klonoa as an extra new story to Sonic Adventure without replacing any character, with custom layouts for the levels so they coud fit Klonoa gameplay.

## HOW

This is very different from regular character replacement, the original Sonic model isn't edited at any point. What this mod does instead is it hacks the function that render Sonic and tells the game to draw Klonoa instead.

- This is done with custom code injected through the SADX Mod Loader.
- Klonoa is loaded from an external model that got converted to the Sonic Adventure format with the [SA Blender addon](https://github.com/Justin113D/BlenderSASupport/wiki#how-to-create-model-mods-for-the-sonic-adventure-games) as well as his animations. 
- This is similar to what the [SA2 Sonic Mod](https://github.com/X-Hax/SADXChunkModels/blob/master/SA2Sonic/SA2Sonic.cpp) does.

Special thanks to [Triph](https://github.com/entriphy) for exporting Klonoa models and anims from the PS2 game and uploaded all of them, this project couldn't exist otherwise.

This project took a lot of work, since I had to replace every animation, otherwise the game would simply crash eventually. I had to find a counterpart with Klonoa anims for every Sonic anims.
Obviously the game doesn't have a counterpart for all of them, so I ended using a trick: When the mod is loaded, all the missing animations are replaced with a falling animation as a failsafe.
The final missing ones will have to be made from scratch or through [animation retargeting](https://github.com/Mwni/blender-animation-retargeting).

Adding all the custom abilities also took a lot of efforts to make everything working properly. As you can imagine, SADX was never planned to allow a character to grab enemies and throw them, lol.


## Features

* Klonoa playable with his original animations!
* Hover ability, while jumping hold A to hover for a bit, this is similar to Knuckles glide.
* Can capture enemy (press the action button close enough from an enemy to capture them, this also work on character fight.)
* Press the action button to throw an enemy and damage the other ones (including bosses.)
* Can Super Jump with an enemy grabbed. (Press jump while holding an enemy.)
* Bosses fight have been edited to be beatable in a Klonoa style.
* Replace common objects with their Klonoa counterpart (optional).
* Custom HUD with HP system (hearts) instaed of rings (optional).

## Install

1. Download the mod from the [GitHub releases page](https://github.com/Sora-yx/SADX-Klonoa-Gameplay/releases).
2. Extract the folder contained in the archive into the "mods" folder at the root of your game. <br> If you do not have a "mods" folder, download the [mod loader](https://github.com/x-hax/sadx-mod-loader).
3. Launch the Mod Manager and enable "Klonoa Gameplay".
4. (optional) Use the "configure" button in the Mod Manager to tweak the mod's configuration.


