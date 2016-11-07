Tools & Middleware

**Introduction**

The purpose of this implementation was to create a simple physics demo containing examples of hinge constraints, springs and collision callbacks.

**Implementation**

The tools I used was Bullet, a well know physics library, which is already contained in the octet framework. 

The class ReadCsv contains the logic to read data from a text file of comma separated values (csv), stored in this case in the assets folder. The char (uint8_t) values are first read and purged of whitespace, line feeds and carriage returns. The “clean data” is then iterated through once more and chars that are not commas or semicolons are read into the dynarray variables, which will contain only the significant values from the Csv file.
In this implementation it will be a series of “D”s and “p”s, to represent decks and planks. 

The initialising function app_init() creates default camera instances and illumination settings.
It also initialises the materials that are used of the shapes.
When a mesh instance is created, it is also added to the world with a rigid body, which we will then save in a bullet variable of type btRigidBody.

The sample door and the bridge (which is created with a call to the function MakeBridge()) operate in the same way. A certain number of mesh instances are created and loaded into the world (mtw.loadIdentity()). The csv file will give the instruction as to what length and whether the instances shall be “decks” or “planks”. 
Then hinge constraints will are created between one instance and the next. The bullet library provides a btHingeConstraint type, which requires a rigid body from the objects that the hinge connects. When the hinge constraint is created is added to the physicalWorld (btDiscreteDynamicsWorld in bullet).

The creation of springs operates in a similar way. Here they are generated with a call to the function MakeSprings(): two mesh instances are generated and loaded into the world. The bullet construct btGeneric6DofSpringConstraint allows to generate the spring constraint. It asks for two rigid bodies and for each one of their pivot points (frameInA and frameInB). The constraint is then an upper and lower limit, before being added to the physicalWorld. Other parameters can also be tweaked, such as stiffness and damping, as well as enableSpring.

The collision callbacks are read in the function CollisionCallBacks(), called every frame by the draw_world() function. The key bullet element is the btDispatcher - an interface class that can be used to dispatch calculations with overlapping pairs. It is invoked by the btDynamicsWorld variable and through the method getNumManifolds() is able to calculate the  callbacks as an int and store them in the variable numManifolds.
The resulting indexes are then associated with player and objects for which the collision detections is desired, then a sound from the library is played.

The InputManager() function handles the input buttons to move around in the world space. The basic movements are controlled with up, down, righ, left arrow keys, whil shift allows us to zoom in and ctrl to zoom out. The the pov can be changed by moving the mouse.

Part of the implementation included a first person shooter-like mechanism, where by pressing the f1 key yellow sticks are shot from player’s pov. When they collide with the sound tower the produce a sound that underlines the collision. 

Most of the code is contained in the ToolsAndMiddleware.h file, which includes the ReadCsv class and the ToolsAndMiddleware 
class. 
The slab.h file includes the slab class, which contains some basic parameters for creating plank instances, and methods to easily access parameters such as rigidbody and transform from outside the class. The file stick.h was created for the same reason, and it includes general specifications for the sticks being shot in the game world. 

**Conclusion**

It’s been a rewarding experience to work with the bullet library and learn new tools. It has allowed me to further my knowledge of C++ and Object Oriented Programming. I specifically tried to work more with classes, even thought it may not have been strictly necessary. It could have been improved by adding a python script to as a user interface to directly generate the csv file. Another challeng could have been to use a different sound library such as Fmod. 

**Credits**
Mircea Catana, for helping with the springs implementation.

Elio de Berardinis for helping with collision callbacks.

**Video**

https://youtu.be/RRi9XSW6qaY

**Images**

![Alt text](https://github.com/fsoncini/octet/blob/T&M_assignment/octet/assets/toosl_and_middleware/Capture1.JPG?raw=true"Screenshot 1")


