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
		//test implementation to read csv into variable. Working

		//store data from text file in variables. At the end I should have a version of {'D', 'p', 'p', 'D'} etc
		
	public:
		dynarray<char> variables;

		void read_csv_data(dynarray<uint8_t> file_content) {
			
			dynarray<uint8_t> clean_data;

			for each(uint8_t c in file_content) {
				if (c != ' ' && c != '\n' && c != '\r') {
					clean_data.push_back(c);
				}
			}

			// get list of ids for Bridge parts
			for (unsigned int cursor = 0; cursor < clean_data.size(); ++cursor) {
				char current_char = clean_data[cursor];
				if (current_char == ';') {
					break;
				}
				else if (current_char == ',') {
					continue;
				}
				else {
					variables.push_back(current_char);
				}
			}
		}

		ReadCsv() {
		};

		void read_file() {
			dynarray<uint8_t> file_content;
			std::string file_name = "assets/bridges.txt";
			app_utils::get_url(file_content, file_name.c_str());

			read_csv_data(file_content);
		}

	};


	
	//class bullet {
	//	mesh_instance *mi;
	//	int timer;
	//public:
	//	bullet() {
	//		timer = 0;
	//	}

	//	bullet(mesh_instance *mi_) {
	//		mi = mi_;
	//		timer = 0;
	//	}

	//	mesh_instance& get_mesh_instance() {
	//		return *mi;
	//	}

	//	mesh_instance* getp_mesh_instance() {
	//		return mi;
	//	}

	//	int& get_timer() {
	//		return timer;
	//	}
	//};

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

	/*helper_fps_controller fps_helper;*/
	helper_fps_controller fps_helper;
	ref<scene_node> player_node;

	/* storing all sticks so we can do clean-up*/
	stick s;
	dynarray<stick> sticks;

	// jukebox (plays sound when you hit it)
	int jukebox_index;
	int player_index;	

	//materials
	material *red;
	material *green;
	material *blue;
	material *pink;
	material *black;

	//ALuint sound;
	//unsigned int sound_source;
	//unsigned int num_sound_sources = 32;
	//ALuint sources[32];
	//bool can_play_sound;

	int frame_count = 0;

	//terrain 
	struct terrain_mesh_source : mesh_terrain::geometry_source {
		mesh::vertex vertex(vec3_in bb_min, vec3_in uv_min, vec3_in uv_delta, vec3_in pos) {
			vec3 p = bb_min + pos;
			vec3 uv = uv_min + vec3((float)pos.x(), (float)pos.z(), 0) * uv_delta;
			return mesh::vertex(p, vec3(0, 1, 0), uv);
		}
	};
	terrain_mesh_source terrain_source;

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
	  fps_helper.init(this);

	  main_camera = app_scene->get_camera_instance(0);
	  main_camera->get_node()->translate(vec3(0, 4, 0));
	  main_camera->set_far_plane(10000);

	  //initialize materials
	  red = new material(vec4(1, 0, 0, 1));
	  green = new material(vec4(0, 1, 0, 1));
	  blue = new material(vec4(0, 0, 1, 1));
	  pink = new material(vec4(255, 1, 255, 1));
	  black = new material(vec4(0, 0, 0, 1));

	  //terrain background
	  mat4t mat;
	  mat.loadIdentity();
	  mesh_instance *mi = app_scene->add_shape( mat, new mesh_terrain(vec3(100.0f, 0.5f, 100.0f), ivec3(100, 1, 100), terrain_source),
		  pink, false, 0);
	  btRigidBody *rb = mi->get_node()->get_rigid_body();	  
 
	  //read csv file
	  rcsv.read_file();

	  //DOOR with hinges test
	  //mat.loadIdentity();
	  //mat.translate(vec3(-9.0f, 1.0f, 0.0f));
	  //mesh_instance *k1 = app_scene->add_shape(mat, new mesh_box(vec3(0.2f, 4.0f, 0.2f)), blue, false);

	  //mat.loadIdentity();
	  //mat.translate(vec3(-7.8f, 1.0f, 0.0f));
	  //mesh_instance *door = app_scene->add_shape(mat, new mesh_box(vec3(1.0f, 4.0f, 0.2f)), black, true);

	  //btHingeConstraint *d = new btHingeConstraint(*(k1->get_node()->get_rigid_body()), *(door->get_node()->get_rigid_body()),
		 // btVector3(0.1f, 2.0f, 0.2f), btVector3(-0.5f, 2.0f, 0.2f),
		 // btVector3(0, 2, 0), btVector3(0, 2, 0), false);
	  ////c1->setLimit(-PI * 0.1f, PI* 0.1f);
	  //physicalWorld->addConstraint(d);
	  
	  //player fps dimensions
	  float player_height = 1.8f;
	  float player_radius = 0.25f;
	  float player_mass = 90.0f;

	  //sphere being shot FIX
	  //mat.loadIdentity();
	  //mat.translate(0.0f, player_height*6.0f, 30.0f);
	
	  //mesh_instance *mi2 = app_scene->add_shape( mat, new mesh_box(vec3(0.2f, 0.2f, 6.0f)),green,
		 // true, player_mass,
		 // new btCapsuleShape(0.25f, player_height)
	  //); 

	  //player_node = mi2->get_node();
	  //player_index = player_node->get_rigid_body()->getUserIndex();


	  create_springs();
	  //MakeSprings();
	  MakeBridge();

	  ////big purple box
	  //mat.loadIdentity();
	  //mat.translate(vec3(30, 1, 0));
	  //mesh_instance *mi3 = app_scene->add_shape(mat, new mesh_box(vec3(2)), new material(vec4(0.2, 0.1, 0.5, 1)), false);
	  //jukebox_index = mi3->get_node()->get_rigid_body()->getUserIndex();

	  //sound = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/bang.wav");
	  //sound_source = 0;
	  //alGenSources(num_sound_sources, sources);
	  //can_play_sound = true;

    }
		
	//working. Rename after deleting create_bridge()

	void MakeBridge () {

		mesh_instance *slabs[20];
		btHingeConstraint *hinges[20];
		
		float deck_x = 0.0f;
		float plank_x = 0.0f;
		float increment_deck = 0.0f;
		float increment_plank = 0.0f;	
		
		//create and add meshes read from Csv file
		for (int i = 0; i < rcsv.variables.size(); i++) {
			mat4t mtw;
			mtw.loadIdentity();

			if (rcsv.variables[i] == 'D') {
				slab deck = slab(mtw, vec3(1, 1, 1), vec3(deck_x + increment_deck, 0.5f, 0), black, 0);
				mtw.translate(deck.get_translate()); 
				deck_x = deck.get_x();
				slabs[i] = app_scene->add_shape(mtw, deck.get_mesh(), deck.get_material(), false);
				increment_deck += 6.5f; //add or subtract here
			} 

			else if (rcsv.variables[i] == 's') {
				slab plank = slab(mtw, vec3(0.5f, 0.25f, 1), vec3(deck_x + 1.6f + increment_plank, 1.25f, 0), red, 0);
				mtw.translate(plank.get_translate());
				plank_x = plank.get_x();
				slabs[i] = app_scene->add_shape(mtw, plank.get_mesh(), plank.get_material(), true);
				increment_plank += 1.1f;			
			}
		}
		
		// create hinges
		for (int i = 0; i < rcsv.variables.size()-1; i++) {
			hinges[i] = new btHingeConstraint(*(slabs[i]->get_node()->get_rigid_body()), *(slabs[i + 1]->get_node()->get_rigid_body()),
				btVector3(0.5f, 0.125f, 0.0f), btVector3(-0.5f, 0.125f, 0.0f),
				btVector3(0, 0, 1), btVector3(0, 0, 1), false);
			hinges[i]->setLimit(-PI * 0.1f, PI* 0.1f);
			physicalWorld->addConstraint(hinges[i]);
		}
	}
	
	void create_springs() {
		mat4t mtw;
		mtw.translate(-3, 10, 0);
		btRigidBody *rb1 = NULL;
		mesh_instance *mi1 = app_scene->add_shape(mtw, new mesh_box(vec3(1, 1, 1)), new material(vec4(1, 0, 0, 1)), false);
		rb1 = mi1->get_node()->get_rigid_body();

		mtw.loadIdentity();
		mtw.translate(-5, 8, 0);
		btRigidBody *rb2 = NULL;
		mesh_instance *mi2 = app_scene->add_shape(mtw, new mesh_box(vec3(1, 1, 1)), new material(vec4(0, 1, 0, 1)), true, 1.0f);
		rb2 = mi2->get_node()->get_rigid_body();

		btTransform frameInA, frameInB;
		frameInA = btTransform::getIdentity();
		frameInA.setOrigin(btVector3(btScalar(0.0f), btScalar(-0.5f), btScalar(0.0f)));
		frameInB = btTransform::getIdentity();
		frameInB.setOrigin(btVector3(btScalar(0.0f), btScalar(0.5f), btScalar(0.0f)));

		btGeneric6DofSpringConstraint *c1 = new btGeneric6DofSpringConstraint(*rb1, *rb2, frameInA, frameInB, true);
		c1->setLinearUpperLimit(btVector3(0., 5.0f, 0.));
		c1->setLinearLowerLimit(btVector3(0., -5.0f, 0.));

		c1->setAngularLowerLimit(btVector3(-1.5f, -1.5f, 0));
		c1->setAngularUpperLimit(btVector3(1.5f, 1.5f, 0));

		physicalWorld->addConstraint(c1, false);

		c1->setDbgDrawSize(btScalar(5.f));
		c1->enableSpring(0, true);
		c1->setStiffness(0, 10.0f);
		c1->setDamping(0, 0.5f);
	}



	void MakeSprings() {

		mat4t mtw;
		mtw.translate(-5, 10, 0);
		btRigidBody *rb1 = NULL;
		mesh_instance *mi1 = app_scene->add_shape(mtw, new mesh_box(vec3(1, 1, 1)), new material(vec4(1, 0, 0, 1), false));
		rb1 = mi1->get_node()->get_rigid_body();

		mtw.loadIdentity();
		mtw.translate(-3, 8, 0);
		btRigidBody *rb2 = NULL;
		mesh_instance *mi2 = app_scene->add_shape(mtw, new mesh_box(vec3(1, 1, 1)), black, true, 1.0f);
		rb2 = mi2->get_node()->get_rigid_body();

		btTransform frameInA, frameInB;
		frameInA = btTransform::getIdentity();
		frameInA.setOrigin(btVector3(btScalar(0.0f), btScalar(-0.5f), btScalar(0.0f)));
		frameInB = btTransform::getIdentity();
		frameInB.setOrigin(btVector3(btScalar(0.0f), btScalar(0.5f), btScalar(0.0f)));

		btGeneric6DofSpringConstraint *c1 = new btGeneric6DofSpringConstraint(*rb2, *rb1, frameInA, frameInB, true);
		c1->setLinearUpperLimit(btVector3(0.0f, 5.0f, 0.0f));
		c1->setLinearLowerLimit(btVector3(0.0f, -5.0f, 0.0f));

		c1->setAngularLowerLimit(btVector3(-1.5f, -1.5f, -1.5f));
		c1->setAngularUpperLimit(btVector3(1.5f, 1.5f, 1.5f));

		physicalWorld->addConstraint(c1, false);

		c1->setDbgDrawSize(btScalar(5.f));
		c1->enableSpring(0.0f, true);
		c1->setStiffness(0.0f, 10.0f);
		c1->setDamping(0.0f, 0.5f);

	}

	//void shoot_reset() {
	//	mat4t mtw;
	//	mtw.translate(main_camera->get_node()->get_position());
	//	
	//	bullet b.get_mesh_instance().get_node()->apply_central_force(fwd*30.0f);
	//}


	void stick_cleanup() {
		for (unsigned int i = 0; i < sticks.size(); ++i) {
			if (sticks[i].get_timer() > 1/*150*/) {
				app_scene->delete_mesh_instance(sticks[i].getp_mesh_instance());
				sticks[i] = sticks[sticks.size() - 1];
				sticks.resize(sticks.size() - 1);
			}
		}
	}


	void shoot() {
		mat4t mtw;
		mtw.translate(main_camera->get_node()->get_position());
		stick s = stick(app_scene->add_shape(mtw, new mesh_box(vec3(0.2f, 0.2f, 6.0f)), green, true, 0.01f));
	vec3 fwd = -main_camera->get_node()->get_z();
		s.get_mesh_instance().get_node()->apply_central_force(fwd*30.0f);
		sticks.push_back(s);
		stick_cleanup();
		//shoot_reset();
	}



	//ALuint get_sound_source() {
	//	sound_source = sound_source % num_sound_sources;
	//	sound_source++;
	//	return sources[sound_source];
	//}


	//void check_collisions() {
	//	int num_manifolds = physicalWorld->getDispatcher()->getNumManifolds();
	//	for (unsigned int i = 0; i < num_manifolds; ++i) {
	//		btPersistentManifold *manifold = physicalWorld->getDispatcher()->getManifoldByIndexInternal(i);
	//		int index0 = manifold->getBody0()->getUserIndex();
	//		int index1 = manifold->getBody1()->getUserIndex();

	//		if (index0 == player_index || index1 == player_index) {
	//			if (index0 == jukebox_index || index1 == jukebox_index) {
	//				if (can_play_sound) {
	//					ALuint source = get_sound_source();
	//					alSourcei(source, AL_BUFFER, sound);
	//					alSourcePlay(source);
	//					can_play_sound = false;
	//				}
	//			}
	//		}
	//	}
	//}


	void InputManager() {

		//shoot
		if (is_key_going_down(key_f1)) {
			shoot();
		}

		//zoom in
		if (is_key_down(key_shift)) {
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, -1.5f));
		}
		
		//zoom out
		if (is_key_down(key_ctrl)) {
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, 1.5f));
		}
		//move up
		if (is_key_down(key_up)) {
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0.5f, 0));
		}
		//move down
		if (is_key_down(key_down)) {
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, -0.5f, 0));
		}
		//move right
		if (is_key_down(key_right)) {
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(0.5f, 0, 0.0f));
		}
		//move left
		if (is_key_down(key_left)) {
			app_scene->get_camera_instance(0)->get_node()->translate(vec3(-0.5f, 0, 0.0f));
		}
		//move up
		if (is_key_down(key_page_up)) {
		 app_scene->get_camera_instance(0)->get_node()->translate(vec3(0.0f, 0.5f, 0.0f));
		}
		//move down
		if (is_key_down(key_page_down)) {
		 app_scene->get_camera_instance(0)->get_node()->translate(vec3(0.0f, -0.5f, 0.0f));
		}

		//TEST
		//if (is_key_down(key_mmb)) {
		// ALuint source = get_sound_source();
		// alSourcei(source, AL_BUFFER, bang);
		// alSourcePlay(source);
		//}

	}


	/// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);
	  
	  InputManager();

	  stick_cleanup();

	/*  check_collisions();*/

	  if (++frame_count > 100) {
		  frame_count = 0;
		  //can_play_sound = true;
	  }

	  //update camera
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
