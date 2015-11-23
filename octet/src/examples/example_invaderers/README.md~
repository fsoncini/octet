##SIR ARTHUR 'N GOBLINS

The hack is inspired by the popular 1980’s arcade game Ghosts n Goblins, developed by Capcom. It modifies the existing "Example Invaderers" game in 
the Octet framework. The player controls sir Arthurs, a knight in a heavy armor. In the original game he has to rescue a princess from an evil ghoul,
in this version he only gets kudos from other fellow knights if he succeeds in killing the boss.

The game is developed as a simple 2D platformer with two levels. In the first, sir Arthurs faces vampires and a handful of 1970's invaders that shoot 
him from above.
When he makes his way through the level and touches the skull he can enter the second level and fight the boss, or he could also choose to destroy
all the invaderers to enter the second level.


The two levels are drawn and coloured using CSV files and a fragment shader.
The map from the original invaderers was originally a 20X20 tiles (ach tile a 0.3x0.3 square) and it was resized to a larger 61x20 tiles, 
in order to enable the scrolling platform effect. Each tile is read into an array of int type called map (and map2 for the second CSV
file) that stores values for height and width of the map. The information relative to position and texture of object that are present in the 
maps are stored into dynarrays containing "sprite" types, which are called map_sprite_background[map_sprite_background2 for the
second level], which stores the grass sprites, invaderers and vampires.

The functions read csv() and read csv2() read the CSV files [called background.csv and background.csv2] stored in the assets folder of the 
example_invaderers file. The functions were implemented following the Octet example suggestions. Through these function calls the content of the
CSV files is read and stored into the previously created 2D containers.

The functions setup_visual_map() and setup_visual_map2() initialize all the tiles containing objects in the two levels and store them in the 
dynarrays.

The shader(federico_shader.h), which was implemented with the help and supervision of Mircea Catana, leaves the vertex shader (inherited
from Octet's shader class) unmodified and has a render method that colors the tiles according to RGB values. It is used twice for the 
two different levels with different colors.


The control system is implemented inside the function move_ship(), which contains functions to move the player left and right. The key_up key was
modified and associated with a function that lets the player jump.
Implemented without physics, it translates the player up for the time of 20 frames, and then it stops executing until the player returns to
collide with elements of the environment.

To enable the scrolling effect the camera was pinned to the sprite bg_sprite, which represents the entirety of the background that is then rendered 
through the shader.

To control the vampires two functions were added: move_vampires(...) and vampire_attack(). The first one, after reading the vampires's position from
the map, measures the distance between the vampire and the player. When in close range the vampire will start moving in the direction of the player,
in both directions left or right.
The function vampire_attack() translates sir Arthur away from the vampire when he is hit, enabling him to loose his amorur without being immediately
hit again and prompt the game over.

The boss is controlled by the move_boss() function, which incorporates a randomizer float called jumpProb and gives the boss the ability to jump randomly.
The jumping ability of the boss is implemented the same way it was for the player, by translating the sprite up for a time lasting a certain number of frames.

To let sir Arthur regain his armor after he is hit only once, the function give_armor() was implemented. It is called when the player touches the
diamond skull and enters the second level and after sir Arthurs spends more than 45 frames without armor. 
Finally, when the boss is hit 8 times, a congratulations photo from the film Lancelot du Lac, pops up on the screen.


##CREDITS
Mircea Catana for helping in writing and implementing the shader, the jumping functions and overseing me through the whole development.
Elio De Bernardinis for helping me with setting up the CSV files.


##DEMO LINK:





##SCREENSHOTS

![Alt text](https://github.com/fsoncini/octet/blob/Intro-to-Programming_assignment_1_final/octet/assets/invaderers/photo1.png?raw=true"Screenshot 1")

![Alt text](https://github.com/fsoncini/octet/blob/Intro-to-Programming_assignment_1_final/octet/assets/invaderers/photo2.png?raw=true"Screenshot 2")

![Alt text](https://github.com/fsoncini/octet/blob/Intro-to-Programming_assignment_1_final/octet/assets/invaderers/photo3.png?raw=true"Screenshot 3")

![Alt text](https://github.com/fsoncini/octet/blob/Intro-to-Programming_assignment_1_final/octet/assets/invaderers/photo4.png?raw=true"Screenshot 4")

![Alt text](https://github.com/fsoncini/octet/blob/Intro-to-Programming_assignment_1_final/octet/assets/invaderers/photo5.png?raw=true"Screenshot 5")

![Alt text](https://github.com/fsoncini/octet/blob/Intro-to-Programming_assignment_1_final/octet/assets/invaderers/photo6.png?raw=true"Screenshot 6")




