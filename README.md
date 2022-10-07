# SADX Klonoa Gameplay

WAHOO! Rupurudu!

![alt text](https://upload.wikimedia.org/wikipedia/en/thumb/5/5f/Klonoa.png/220px-Klonoa.png)

## About

Hm? What is this?

This is a SADX mod for the PC version that replaces Sonic with Klonoa from Lunatea's Veil (PS2). This isn't a simple character skin, this ports the original Klonoa model with his original skeleton and animations.
It also implements Klonoa abilities such as wind bullet, enemy capture and super jump. 
Some objects (like rings and checkpoints) are also replaced with their Klonoa counterpart.
There is also a custom HUD and HP system (hearts) manually recreated from the Klonoa remaster assets.

Note that everything, but the Klonoa model can be disabled in the mod settings. So if you just want a simple character skin, that is possible. If you want everything but the Klonoa objects, it is also possible.

Ideally, the long-term goal of this mod would be to implement Klonoa as a new extra story to Sonic Adventure without replacing any characters, with custom layouts for the levels so they coud fit Klonoa gameplay.

## HOW

This is very different from regular characters replacement, the original Sonic model isn't edited at any point. What this mod does instead is it hacks the function that renders Sonic and tells the game to draw Klonoa instead.

- This is done with custom code which is injected through the SADX Mod Loader.
- Klonoa is loaded from an external model that got converted to the Sonic Adventure format with the [SA Blender addon](https://github.com/Justin113D/BlenderSASupport/wiki#how-to-create-model-mods-for-the-sonic-adventure-games) as well as his animations. 
- This is similar to what the [SA2 Sonic Mod](https://gamebanana.com/mods/247991) does.

Special thanks to [Triph](https://github.com/entriphy) for exporting Klonoa models and anims and uploading all of them, this project couldn't exist otherwise.

This mod took a lot of work, I had to replace every animations, otherwise the game would have crashed eventually.  
Obviously Klonoa doesn't have a counterpart for all of them, so I ended using a trick: When the mod is loaded, all the missing animations are replaced with a falling animation as a failsafe.
At the time I'm writing this, there is still few animations that aren't replaced and they will have to be made from scratch or through [animation retargeting](https://github.com/Mwni/blender-animation-retargeting).

Adding all the custom abilities also took a lot of efforts to make everything working properly, not to mention all the fixes that I had to code to make Klonoa work in pretty much every situation.

As you can imagine, SADX was never planned to have a character that can grab enemies and throw them, lol.

## Features

* Klonoa playable with his original animations!
* Hover ability: while jumping, hold A to hover for a bit, this is similar to Knuckles glide but with a much shorter duration time.
* Capture enemy: (press the action button close enough from an enemy to capture them, this also works on character fight.)
* Throw enemy: press the action button to throw an enemy and damage the other ones (including bosses.)
* Super Jump: while holding an enemy, press jump twice to do a Super Jump. WAHOO!
* Bosses fight have been edited to be beatable in a Klonoa style.
* Replace common objects with their Klonoa counterpart (optional).
* Custom HUD with HP system (hearts) instead of rings (optional).

## Install

1. Download the mod from the [GitHub releases page](https://github.com/Sora-yx/SADX-Klonoa-Gameplay/releases).
2. Extract the folder contained in the archive into the "mods" folder at the root of your game. <br> If you do not have a "mods" folder, download the [mod loader](https://github.com/x-hax/sadx-mod-loader).
3. Launch the Mod Manager and enable "Klonoa Gameplay".
4. (optional) Use the "configure" button in the Mod Manager to tweak the mod's configuration.

## FAQ

"Why does Klonoa doesn't have any blending / lighting effect?"
- The model format used is Chunk Model which isn't compatible with Lantern Engine at the moment. Therefore, Klonoa won't benefit from the lighting effects/blending stuff. Only basic model are compatible.

"Can you convert the model to Basic format?"
- Unfortunanely no, the basic format doesn't support weights, which if you don't know is the standard for any 3D game and Klonoa doesn't make an exception. Sonic Adventure did because it was an early 3D game.

"Can you do that with X game and Y character"
- In theory, if you have the models and animations imported in Blender, they can be ported to both Adventure games assuming they aren't too greedy. It's a lot of work though and I don't plan to do something like that again anytime soon.


