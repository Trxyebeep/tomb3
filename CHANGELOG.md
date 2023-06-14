# tomb3 changelog

## Unreleased:

### Original bug fixes

- Fix text sprites getting cut off at the bottom.

### tomb3 additions

- Improve Spanish localization.
- Localize pause menu text.

### tomb3 bug fixes

- Fix duckroll pickups.


## [2.0.2](https://github.com/Trxyebeep/tomb3/tree/V2.0.1) (May. 8 2023)

### Original bug fixes

- Fix wade snapping.
- Fix floordata issues with triangular geometry, no collision tiles, and more generic situations.
- Fix some effects clipping.

### tomb3 additions

- Improve hair interaction with water surfaces.

### tomb3 bug fixes

- Fix title starting with the passport selected.
- Fix audio looping.
- Fix savegame fail if the saves/savesg folder doesn't exist, and create it instead.


## [2.0.1](https://github.com/Trxyebeep/tomb3/tree/V2.0.1) (Apr. 20 2023)

### tomb3 additions

- Increase air gradually instead of snapping to full when leaving water rooms.
- Restore graphics adapter settings in the setup window.

### tomb3 bug fixes

- Fix graphics adapter enumeration.
- Fix script extractor, fix Non-English scripts.

## [2.0.0](https://github.com/Trxyebeep/tomb3/tree/V2.0.0) (Apr. 18 2023)

### Original bug fixes

- Fix Lara refusing to go in some crawlspaces.
- Fix unsafe memory access in boss shield drawing routines.
- Fix sound adapter names in the setup window.
- Fix volume options not applying correctly when launching the game.
- Fix SFX volume not completely muting when setting it to 0.
- Fix blood sparks not using scale.
- Make underwater blood match normal blood on German version.
- Fix explosion dynamic lighting transferring to other sparks.
- Fix the harpoon gun reloading at every shot in NG+.
- Fix the game loading title between levels.

### tomb3 additions

- Finish decompiling the game and compile a new executable.
- Improve falling raptor physics.
- Increase drawn vertices limit once more.
- Increase internal memory from 3.6MBs to 15MBs.
- Increase some more internal renderer limits.
- Increase static mesh limit from 50 to 256.
- Increase items limit from 256 to 1024.
- Increase AI/nonlot enemy limit from 5/12 to 20/20, respectively.
- Rework the renderer to use 32 bit color instead of 16 bit internally.
- Improve the dynamic light formula to make the light transition smoother.
- Improve room vert distance fog formula.
- Reset Lara's cold bar in DOZY mode.
- Improve Puna electricity color.
- Improve electric fence sparks.
- Create new setup window.
- Start setup window centered.
- Move some hardcoded aspects of the game to the script.
- Add new script commands for TRLE builders.

### tomb3 bug fixes

- Fix PSX shadow not following Lara during some animations.
- Fix Willard pathing.
- Fix some window creation/style.
- Fix Tony flame.
- Fix sound adapter enumeration.


## [1.6.0](https://github.com/Trxyebeep/tomb3/tree/V1.6.0) (Jan. 30 2023)

### Original bug fixes

- Fix missing effect on Willard explosion.
- Fix Lara meshbits not being saved.
- Fix Lara's grab position when performing the crawl to grab animation on a triangle ledge.
- Fix Lara teleporting mid-air when performing the crawl to grab animation on a no-col triangle ledge.
- Fix location skip bug in NG+ mode.
- Fix All Hallows loading with corrupt textures if player didn't get all secrets in NG+ mode.
- Fix screen clipping for boss explosion rings/shields.
- Fix odd color on explosion rings.

### tomb3 additions

- Remove fire from Lara in DOZY.
- Make footprints tilt with the floor below them.
- Make PSX shadow tilt with the floor below it.
- Restore PSX boss explosion ring colors.

### tomb3 bug fixes

- Fix map not registering rooms when on vehicles.
- Fix possible error in dynamic light formula.
- Fix Lara meshbits not affecting drawn meshes.
- Fix original PC Sophia Rings starry shape.


## [1.5.0](https://github.com/Trxyebeep/tomb3/tree/V1.5.0) (Jan. 6 2023)

### Original bug fixes

- Fix Dash Timer not being saved.
- Fix Exposure Meter not being saved.
- Fix fish not being saved.
- Fix Sophia rings not being saved.
- Fix rare crash in High Security Compound.
- Fix unsafe memory access in Wasp Spawner.
- Fix dynamic light formula on room geometry.
- Fix grenades, rockets, and harpoons ignoring the carcass in Crash Site.
- Fix grenade launcher unequip frame.
- Fix grenade and rocket launchers double counting kills.
- Fix cheats not working in All Hallows.

### tomb3 additions

- Add dynamic light to DOZY.
- Add map (controls in the using guide).
- New system for expanding savegame format.
- Add Discord Rich Presence for base game + Gold.
- Automatically disable PSX save system when running Gold.
- Weapons cheat now gives Lara the Racetrack Key + flares in Lara's Home.
- Add NG+ for Gold.

### tomb3 bug fixes

- Fix PSX save system passport deadlock.
- Fix Gold getting a crystal at the start.
- Fix crash when taking screenshots in windowed mode.
- Fix screenshot file number starting at 1 instead of 0.
- Fix footprints not being cleared between levels (earlier fix from V1.1.0 did not work).
- Fix possible arm corruption/crash after reloading and using weapons.
- Fix window not being rendered on secondary monitor. -From Saracen


## [1.4.0](https://github.com/Trxyebeep/tomb3/tree/V1.4.0) (Dec. 16 2022)

### Original bug fixes

- Fix Lara letting go of some ledges.
- Fix diver harpoons spamming gunshot SFX.
- Fix hypothetical crash when loading a demo.
- Fix bridge bug.
- Fix weather spawning at Lara position instead of camera position.
- Fix lara collision with objects on water surface.
- Fix darts scale.
- Fix underwater blood color flashing.

### tomb3 additions

- Add PSX save crystal system.
- Add option for blue crystal light.
- Raise TPage/TInfos limits.
- double the amount of spawned weather particles.
- Add option for improved (PSX style) electricity effects.

### tomb3 bug fixes

- Fix music stutter after continuous pausing.
- Fix music muting.
- Fix fish movement.
- Fix snow spawning far from Lara.
- Fix lasers getting affected by selected scale.
- Fix Lara refusing to pull up from certain hang animations (on thin ledges).
- Fix Puna's shield wrongfully lighting up when he shoots.
- Fix flare pickups.


## [1.3.0](https://github.com/Trxyebeep/tomb3/tree/V1.3.0) (Nov. 20 2022)

#### the project was officially renamed from tomb3decomp to tomb3 for this release.

### Original bug fixes

- Fix underwater pickups rotating with Lara.
- Fix the wrong ambient track being played after reloading.

### tomb3 additions

- Add option for PSX mono screen colors.
- Move saves to a designated folder.
- Add support for The Lost Artifact.
- Add experimental Windowed support.

### tomb3 bug fixes

- Fix passport selection crash.
- Fix full music mute causing the track to play silently in the background.
- Fix some text boxes not using additive blending.
- Fix some effects disappearing closer than possible.
- Fix text shadow drawing. ("Fixes" text being cut off at the bottom).


## [1.2.0](https://github.com/Trxyebeep/tomb3/tree/V1.2.0) (Nov. 12 2022)

### Original bug fixes

- Make quadbike follow interpolated frames for smoother animations.
- Restore missing wake effect on the UPV.
- Fix guard gun lasers not appearing on interpolated frames.
- Prevent save/load hotkeys during picture fade.
- Prevent skipping cutscenes during picture fade.
- Fix bug that prevented music from being completely muted.
- Fix Lara's torso getting the water tint in wade deep water.
- Fix Lara's hair becoming stiff while using the UPV.
- Fix flares going through the floor when thrown.
- Fix flare sparks detaching after restarting the game.
- Prevent DOZY while using a vehicle.

### tomb3 additions

- Add scale for UI elements, including text and boxes.
- Add option for PSX text colors.
- Add options for PSX and improved PC bar positions.
- Double the amount of vertices/polygons that can be drawn at a time.
- Add pause menu.
- Add option to control inventory/underwater music mute.
- Completely refactor Lara drawing to stop recalculating her matrices multiple times every frame. Only once per frame now.
- Add option for PSX FOV.
- Add option for PSX ammo counter.
- Add option for PSX style boxes.

### tomb3 bug fixes

- Fix footstep sfx being wrong in some rooms.
- Fix bats shape.
- Fix Lasers disappearing in some areas, and restore them for guard guns.
- Fix UPV displacement after getting off.


## [1.1.0](https://github.com/Trxyebeep/tomb3/tree/V1.1.0) (Oct. 29 2022)

### Original bug fixes

- Fix lara getting stuck in corners.
- Fix lara wading in 1 click deep water.
- Fix the shotgun and its ammo not resetting in Jungle sometimes.
- Fix footprints not being cleared between levels.

### tomb3 additions

- Add crawl tilting.
- Add an option for PSX yellow poison bar.
- Make poison bar flash.
- Add an option for (approximate, for now) PSX water color.
- Add colsub for sparks, like smoke.
- Improve bucket drawing, fixes PC bars disappearing in open spaces. Eg. beginning of Thames Wharf.
- Improve pickup display code to show transparency correctly.
- Add an option for PSX crystal sfx.

### tomb3 bug fixes

- Fix laser seams and color.
- Fix GetFullPath (fixes some FMV playback issues).
- Fix skybox drawing (fixes Lara's Home crash when using the no crystals patch).


## [1.0.0](https://github.com/Trxyebeep/tomb3/tree/V1.0.0) (Oct. 15 2022)

### Original bug fixes

- Fix widescreen FOV calculations (Arsunt)
- Fix gamma affecting the skybox
- Fix text/sprites turning blue when looking in certain angles (eg. Assault Course timer)
- Prevent possible matrix overflow
- Fix untextured areas showing traces of previous textures (Arsunt)
- Fix hair collision not taking into account Lara's left hand sphere
- Fix stiff hair during wading
- Fix Kayak rapids mist effect appearing randomly on the screen
- Fix electricity appearing on Lara instead of Sophia when exploding the fusebox
- Fix the wrong odd vertex on Explosion and Sophia rings, and all boss explosion shields
- Fix user settings being lost after crash/ALT+F4
- Fix the sunglasses menu crash
- Fix having to go in the keys menu to avoid default/user binds conflicting
- Fix passport sfx not playing while underwater
- Fix very unsafe memory usage when taking screenshots
- Add experimental fix for transparency issues on Nvidia cards

### tomb3 additions

- Add colsub for different effects that need it
- Change some functions to use 32 bit color instead of 16 internally
- Restore rain in some levels (as in PSX)
- Add footprints
- Add underwater dust
- Make hair follow interpolated frames for much smoother movement
- Make hair collide with the floor
- Restore the pickup display and fix its clipping issues
- Expand on Lara's moveset
- Add DOZY cheat
- Make Lara faster during DOZY
- Add PSX bars (Arsunt)
- Scale the bars properly (Arsunt)
- Use quads instead of lines for original bars (Arsunt)
- properly scale rain and change it into semitransparent quads instead of opaque lines and make the color match PSX
- Properly scale snow
- Implement PSX/Improved PC Sophia rings
- Properly scale lasers and change them into semitransparent quads instead of opaque lines
- Add PSX shadow
- Move all settings from config.txt to the registry
