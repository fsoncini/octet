////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//


#include "slab.h"; //test, maybe not needed
#include <array>;
#include "stick.h";

namespace octet {

	//Class to Read a csv file stored in the assets folder
	class ReadCsv {
		
	public:
		dynarray<char> variables;

		void ReadCsvData(dynarray<uint8_t> file_content) {
			
			dynarray<uint8_t> cleanData;

			for each(uint8_t c in file_content) {
				if (c != ' ' && c != '\n' && c != '\r') {
					cleanData.push_back(c);
				}
			}

			//Get list of ids for bridge parts
			for (unsigned int cursor = 0; cursor < cleanData.size(); ++cursor) {
				char currentChar = cleanData[cursor];
				if (currentChar == ';') {
					break;
				}
				else if (currentChar == ',') {
					continue;
				}
				else {
					variables.push_back(currentChar);
				}
			}
		}

		ReadCsv() {
		};

		~ReadCsv() {

		};

		void ReadFile() {
			dynarray<uint8_t> file_content;
			std::string file_name = "assets/bridges.txt";
			app_utils::get_url(file_content, file_name.c_str());

			ReadCsvData(file_content);
		}

	};


  class ToolsAndMiddleware : public app {
  private:
	
	//constraints
	const float PI = 3.14159;
	btDiscreteDynamicsWorld *physicalWorld;

    // scene for drawing box
    ref<visual_scene> app_scene;

	//camera & fps members
	mouse_look moving_mouse_view;
	ref<camera_instance> main_camera;

	helper_fps_controller fpsHelper;
	ref<scene_node> playerNode;

	/* storing all sticks so we can do clean-up*/
	stick s;
	dynarray<stick> sticks;

	// jukebox (plays sound when you hit it)
	int soundTowerIndex;
	int playerIndex;	
	int doorIndex;

	//materials
	material *red;
	material *green;
	material *blue;
	material *pink;
	material *black;

	ALuint sound;
	unsigned int soundSource;
	unsigned int numSoundSources = 32;
	ALuint sources[32];
	bool canPlaySound;

	int frameCount = 0;

	//terrain 
	struct terrain_mesh_source : mesh_terrain::geometry_source {
		mesh::vertex vertex(vec3_in bb_min, vec3_in uv_min, vec3_in uv_delta, vec3_in pos) {
			vec3 p = bb_min + pos;
			vec3 uv = uv_min + vec3((float)pos.x(), (float)pos.z(), 0) * uv_delta;
			return mesh::vertex(p, vec3(0, 1, 0), uv);
		}
	};
	terrain_mesh_source terrain_source;

	//create instance of ReadCsv class (to read data from csv)
	ReadCsv rcsv;

  public:
    /// this is called when we construct the class before everything is initialised.
    ToolsAndMiddleware(int argc, char **argv) : app(argc, argv) {
	
    }

    /// this is called once OpenGL is initialized
    void app_init() {
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();
	  app_scene->get_camera_instance(0)->get_node()->translate(vec3(0.0f, 0.0f, 1.0f));
	  
	  physicalWorld = app_scene->get_bt_world(); //method added in visual_scene.h

	  moving_mouse_view.init(this, 200.0f / 360, false);
	  fpsHelper.init(this);

	  main_camera = app_scene->get_camera_instance(0);
	  main_camera->get_node()->translate(vec3(0, 4, 0));
	  main_camera->set_far_plane(10000);

	  //Initialize materials
	  red = new material(vec4(1, 0, 0, 1));
	  green = new material(vec4(0, 1, 0, 1));
	  blue = new material(vec4(0, 0, 1, 1));
	  pink = new material(vec4(255, 1, 255, 1));
	  black = new material(vec4(0, 0, 0, 1));


	  //Create and add terrain background
	  mat4t mat;
	  mat.loadIdentity();
	  mesh_instance *mi = app_scene->add_shape( mat, new mesh_terrain(vec3(100.0f, 0.5f, 100.0f), ivec3(100, 1, 100), terrain_source),
		  pink, false, 0);
	  btRigidBody *rb = mi->get_node()->get_rigid_body();	  
 
	  //Read csv file
	  rcsv.ReadFile();

	  //Create door. Used as a different sample for hinges and sound.
	  
	  //Create door jamb
	  mat.loadIdentity();
	  mat.translate(vec3(-15.0f, 1.0f, 0.0f));
	  mesh_instance *k1 = app_scene->add_shape(mat, new mesh_box(vec3(0.2f, 4.0f, 0.2f)), blue, false);
	  
	  //Create door
	  mat.loadIdentity();
	  mat.translate(vec3(-14.8f, 1.0f, 0.0f));
	  mesh_instance *door = app_scene->add_shape(mat, new mesh_box(vec3(1.0f, 4.0f, 0.2f)), black, true);
	  doorIndex = door->get_node()->get_rigid_body()->getUserIndex();

	  //Create hinge between jamb and door
	  btHingeConstraint *d = new btHingeConstraint(*(k1->get_node()->get_rigid_body()), *(door->get_node()->get_rigid_body()),
		  btVector3(0.1f, 2.0f, 0.2f), btVector3(-0.5f, 2.0f, 0.2f),
		  btVector3(0, 2, 0), btVector3(0, 2, 0), false);
	  d->setLimit(-PI * 0.1f, PI* 0.1f);
	  physicalWorld->addConstraint(d);
	  
	  //Declare dimensions for player fps pov
	  float player_height = 1.8f;
	  float player_radius = 0.25f;
	  float player_mass = 90.0f;

	  //Player 
	  mat.loadIdentity();
	  mat.translate(0.0f, player_height*6.0f, 50.0f);
	  mesh_instance *playerInstance = app_scene->add_shape(
		  mat,
		  new mesh_sphere(vec3(0), player_radius),
		  new material(vec4(1, 0, 0, 1)),
		  true, player_mass,
		  new btCapsuleShape(0.25f, player_height)
	  );
	  playerNode = playerInstance->get_node();
	  playerIndex = playerNode->get_rigid_body()->getUserIndex();

	  //Call functions to create springs and bridge
	  MakeSprings();
	  MakeBridge();

	  //Create Tower for SOUND and COLLISION purpose 
	  mat.loadIdentity();
	  mat.translate(vec3(30, 1, 0));
	  mesh_instance *soundTower = app_scene->add_shape(mat, new mesh_box(vec3(2,12,2)),
		  blue, false);
	  soundTowerIndex = soundTower->get_node()->get_rigid_body()->getUserIndex();

	  sound = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/bang.wav");
	  soundSource = 0;
	  alGenSources(numSoundSources, sources);
	  canPlaySound = true;

    }
	
	ALuint getSoundSource() {
		soundSource = soundSource % numSoundSources;
		soundSource++;
		return sources[soundSource];
	}

	//Retrieves results from collisions
	void CollisionCallbacks() {
		int numManifolds = physicalWorld->getDispatcher()->getNumManifolds();
		for (unsigned int i = 0; i < numManifolds; ++i) {
			btPersistentManifold *manifold = physicalWorld->getDispatcher()->getManifoldByIndexInternal(i);
			int index0 = manifold->getBody0()->getUserIndex();
			int index1 = manifold->getBody1()->getUserIndex();

			if (index0 == playerIndex || index1 == playerIndex) {
				if (index0 == soundTowerIndex || index1 == soundTowerIndex) {
					if (canPlaySound) {
						ALuint source = getSoundSource();
						alSourcei(source, AL_BUFFER, sound);
						alSourcePlay(source);
						canPlaySound = false;

					}
				}
			}
		}
	}


	//Reads parameters from Csv file, then creates a bridge.
	void MakeBridge () {

		mesh_instance *slabs[20];
		btHingeConstraint *hinges[20];
		
		float deck_x = 0.0f;
		float plank_x = 0.0f;
		float increment_deck = 0.0f;
		float increment_plank = 0.0f;	
		
		//Create and add meshes read from Csv file
		for (int i = 0; i < rcsv.variables.size(); i++) {
			mat4t mtw;
			mtw.loadIdentity();

			if (rcsv.variables[i] == 'D') {
				slab deck = slab(mtw, vec3(1, 1, 1), vec3(deck_x + increment_deck, 0.5f, 0), black, 0);
				mtw.translate(deck.get_translate()); 
				deck_x = deck.get_x();
				slabs[i] = app_scene->add_shape(mtw, deck.get_mesh(), deck.get_material(), false);
				increment_deck += 6.5f; 
			} 

			else if (rcsv.variables[i] == 's') {
				slab plank = slab(mtw, vec3(0.5f, 0.25f, 1), vec3(deck_x + 1.6f + increment_plank, 1.25f, 0), red, 0);
				mtw.translate(plank.get_translate());
				plank_x = plank.get_x();
				slabs[i] = app_scene->add_shape(mtw, plank.get_mesh(), plank.get_material(), true);
				increment_plank += 1.1f;			
			}
		}
		
		//Create hinges
		for (int i = 0; i < rcsv.variables.size()-1; i++) {
			hinges[i] = new btHingeConstraint(*(slabs[i]->get_node()->get_rigid_body()), *(slabs[i + 1]->get_node()->get_rigid_body()),
				btVector3(0.5f, 0.125f, 0.0f), btVector3(-0.5f, 0.125f, 0.0f),
				btVector3(0, 0, 1), btVector3(0, 0, 1), false);
			hinges[i]->setLimit(-PI * 0.1f, PI* 0.1f);
			physicalWorld->addConstraint(hinges[i]);
		}
	}
	
	//Creates two shapes and connects them with a spring
	void MakeSprings() {

		mat4t mtw;
		mtw.translate(-3, 10, 0);
		btRigidBody *rbAbove = NULL;
		mesh_instance *miAbove = app_scene->add_shape(mtw, new mesh_box(vec3(1, 1, 1)), black, false);
		rbAbove = miAbove->get_node()->get_rigid_body();

		mtw.loadIdentity();
		mtw.translate(-5, 8, 0);
		btRigidBody *rbBelow = NULL;
		mesh_instance *miBelow = app_scene->add_shape(mtw, new mesh_box(vec3(1, 1, 1)), red, true, 1.0f);
		rbBelow = miBelow->get_node()->get_rigid_body();

		btTransform frameInA, frameInB;
		frameInA = btTransform::getIdentity();
		frameInA.setOrigin(btVector3(btScalar(0.0f), btScalar(-0.5f), btScalar(0.0f)));
		frameInB = btTransform::getIdentity();
		frameInB.setOrigin(btVector3(btScalar(0.0f), btScalar(0.5f), btScalar(0.0f)));

		btGeneric6DofSpringConstraint *c1 = new btGeneric6DofSpringConstraint(*rbAbove, *rbBelow, frameInA, frameInB, true);
		c1->setLinearUpperLimit(btVector3(0., 5.0f, 0.));
		c1->setLinearLowerLimit(btVector3(0., -5.0f, 0.));

		c1->setAngularLowerLimit(btVector3(-1.5f, -1.5f, -1.5f));
		c1->setAngularUpperLimit(btVector3(1.5f, 1.5f, 1.5f));

		physicalWorld->addConstraint(c1, false);

		c1->setDbgDrawSize(btScalar(5.f));
		c1->enableSpring(0.0f, true);
		c1->setStiffness(0.0f, 10.0f);
		c1->setDamping(0.0f, 0.5f);
	}
	
	//possibly not use

	void CleanupSticks() {
		for (unsigned int i = 0; i < sticks.size(); ++i) {
			if (sticks[i].get_timer() > 1/*150*/) {
				app_scene->delete_mesh_instance(sticks[i].getp_mesh_instance());
				sticks[i] = sticks[sticks.size() - 1];
				sticks.resize(sticks.size() - 1);
			}
		}
	}

	//rename
	void ShootSticks() {
		mat4t mtw;
		mtw.translate(main_camera->get_node()->get_position());
		stick s = stick(app_scene->add_shape(mtw, new mesh_box(vec3(0.2f, 0.2f, 6.0f)), green, true, 0.01f));
	    vec3 fwd = -main_camera->get_node()->get_z();
		s.get_mesh_instance().get_node()->apply_central_force(fwd*30.0f);
		sticks.push_back(s);
		CleanupSticks();	
	}

	void InputManager() {

		//Shoot Sticks
		if (is_key_going_down(key_f1)) {
			ShootSticks();
		}

		//Zoom in
		if (is_key_down(key_shift)) {
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, -1.5f));
		}
		
		//Zoom out
		if (is_key_down(key_ctrl)) {
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, 1.5f));
		}

		//Move up
		if (is_key_down(key_up)) {
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0.5f, 0));
		}
		//Move down
		if (is_key_down(key_down)) {
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, -0.5f, 0));
		}

		//Move right
		if (is_key_down(key_right)) {
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0.5f, 0, 0.0f));
		}

		//Move left
		if (is_key_down(key_left)) {
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(-0.5f, 0, 0.0f));
		}

		//Move up
		if (is_key_down(key_page_up)) {
		 app_scene->get_camera_instance(0)->get_node()->translate(vec3(0.0f, 0.5f, 0.0f));
		}

		//Move down
		if (is_key_down(key_page_down)) {
		 app_scene->get_camera_instance(0)->get_node()->translate(vec3(0.0f, -0.5f, 0.0f));
		}

		//TEST SOUND CHECK
		if (is_key_down(key_lmb)) {
		 ALuint source = getSoundSource();
		 alSourcei(source, AL_BUFFER, sound);
		 alSourcePlay(source);
		}

	}


	/// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);
	  
	  InputManager();

	  CollisionCallbacks();

	  CleanupSticks();
	

	  if (++frameCount > 100) {
		  frameCount = 0;
		  canPlaySound = true;
	  }

	  //Lets us move the camera with the mouse
	  scene_node *camera_node = main_camera->get_node();
	  mat4t &camera_to_world = camera_node->access_nodeToParent();
	  moving_mouse_view.update(camera_to_world);

	 /* fps_helper.update(player_node, camera_node);*/

      // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);

      // draw the scene
      app_scene->render((float)vx / vy);

    }
  };
}
