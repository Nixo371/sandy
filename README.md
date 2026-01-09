# sandy
---
A simple sand simulator I made to learn about graphics programming in C with SDL3


## Dependencies
- SDL3
- GNU Make

## Build
```make```

## Run
```./sandy```

## Features
- Terribly inefficient software rendering
- Left Click to add sand
<p>
    <img src="https://github.com/Nixo371/sandy/blob/master/assets/sandy_spawning_sand.gif" width="100%" alt="Spawning Sand">
</p>

- Right Click to remove sand
<p>
    <img src="https://github.com/Nixo371/sandy/blob/master/assets/sandy_removing_sand.gif" width="100%" alt="Spawning Sand">
</p>

- Scroll to change the size of your cursor
<p>
    <img src="https://github.com/Nixo371/sandy/blob/master/assets/sandy_cursor_size.gif" width="100%" alt="Spawning Sand">
</p>

## Docs
Not much here at the moment, I'll explain the #defines in `include/constants.h` that are useful:
- **WIDTH** and **HEIGHT** define the window size
- **FPS** defines the target FPS, the simulation will run at that framerate at the FASTEST
- **DEFAULT_BLOB_RADIUS** sets the initial value for the radius of the cursor
- **MIN_BLOB_RADIUS** defines the minimum value for the radius of the cursor (I have no idea what happens if the cursor radius is negative)
- **MAX_BLOB_RADIUS** same thing but the max
- **CURSOR_THICKNESS** the thickness of the circle representing your cursor
