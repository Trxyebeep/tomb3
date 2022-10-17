# tomb3 changelog

## Unreleased

### Original bug fixes

### tomb3 additions

### tomb3 bug fixes

- Fix laser seams and color by refactoring its drawing function.


## [1.0.0](https://github.com/Trxyebeep/tomb3decomp/tree/V1.0.0) (Oct. 15 2022)

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
