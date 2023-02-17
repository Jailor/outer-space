# Outer Space OpenGL
The subject of this project is the photorealistic presentation of 3D objects using OpenGL library. The project showcases OpenGL features, as well as features of the C++ language.
## About :thought_balloon:
The scene depicts a space scene with various space related objects on  a mars-like planet. The main base is formed of multiple astronauts, buildings, a spinning metal pyramid, a TIE fighter, and a military helicopter. Behind us there are 2 buildings: a prosperity dome and a big power plant. There is also a spinning moon very far away with some astronauts and an asteroid ring. Next to it there is a star trek base and another planet with asteroids. Pictures for all of these are shown in the [respective section](#implementation-and-pictures-computer)


## Features :white_check_mark:
As far as functionalities are concerned, I implemented the following:
-	Directional, Point and Spotlights and shadows for all of them using omnidirectional shadow maps. 
-	Animation of helicopter and helicopter blades, animation of spotlight direction (bunker), controllable animation/movement of the rover, animation of the star trek base, moon, and its cloud. Animation is independent of framerate using delta time.
-	Instanced draws: instanced drawing techniques used for drawing the Saturn-like planet.
-	:flashlight: Flashlight: a spotlight follows the camera at all times, and can supports toggling, it can be turned on or off.
-	Transparency: dome and tempest have transparency attached to them
-	Teleportation and sound: by pressing some buttons, one can quickly change the scene of visualization in an interactive manner. There is a 1-second timeout for this teleportation, during this time the screen gradually turns a bright white while sound is played 
-	Skybox: gives a sense of realism. The skybox automatically changes from the day scene to the night scene.
-	Scene visualization: visualize the whole scene using animation and Bézier curves.

## Implementation and Pictures :computer:

### Notable implementation details
- Inheritance: C++ inheritance, polymorphism and constructor delegation is used in order to model the lighting and the shadow classes.
- Omnidirectional shadow maps: Cubemap textures are used in order to have shadows from point lights and spot lights
- Rock cloud: the rock clouds are dynamically generated in  OpenGL using a randomized algorithm
### Main video
<img src="/piese/screenshots/Board1.PNG" width="400" title="Main chessboard">


