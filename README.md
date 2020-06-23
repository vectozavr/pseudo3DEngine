# pseudo3DEngine
Simple pseudo-3d engine implementation. This library allows you to make two dimensional map and draw pseudo 3D image using Ray-Cast.

This is a Vectozavr's pseudo3DEngine fork, but with too many code changes and a bunch of fixes and improvements, such as:
- The network system is replaced by another, WORKING system
- Fisheye correction
- Jumps
- Realistic wall height (seriously, the zero-height wall was like half a player)
- Realistic circle collision (not just 10 walls)
- Reflections remain inside the mirrors
- Menu fix (buttons now detect only mouse clicks, not holding)
- The application stops reading input and moving the cursor while you are switched to another window
- FPS limit (no more 600 fps in menu)
- Missing resources fix
- Small optimizations

Demonstration of how does this engine work:
![Project demonstration](demo/demo_engine.jpg)

Some results of texturing:
![Project demonstration](demo/demo_just_textures.png)

We can switch off the textures:
![Project demonstration](demo/demo_textures_off.png)

We can smooth floor (performance-heavy):
![Project demonstration](demo/demo_smooth.png)

This engine can perfom some mirrors. As you can see, we get correct reflection image:
![Project demonstration](demo/demo_mirror.png)

Reflection loop included:
![Project demonstration](demo/demo_mirror_loop.png)

Online test:
![Project demonstration](demo/demo_online.png)

Jumps (but without camera vertical rotation):
![Project demonstration](demo/demo_jump.png)
