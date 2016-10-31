////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

//#include "plank.h"

#include <iostream>
#include <fstream>
#include <vector>


namespace octet {
  /// Scene containing a box with octet.
  class example_shapes : public app {
    // scene for drawing box
    ref<visual_scene> app_scene;
	const float PI = 3.14159;
	btDiscreteDynamicsWorld *dynamics_world;

	

	//Declare arrays and global variables, used in reading and implementing Csv file
	static const int map_width = 61;
	static const int map_height = 20;
	int map[map_height][map_width];
	

	dynarray<mesh_instance> slabs;
	


  public:
    example_shapes(int argc, char **argv) : app(argc, argv) {
    }

    ~example_shapes() {
    }

    /// this is called once OpenGL is initialized
    void app_init() {
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();
      app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 4, 0));

      material *red = new material(vec4(1, 0, 0, 1));
      material *green = new material(vec4(0, 1, 0, 1));
      material *blue = new material(vec4(0, 0, 1, 1));

      mat4t mat;
      mat.translate(-3, 6, 0);

	  Read_Csv();
	 
	  //btRigidBody *first_box = NULL;
	  //btRigidBody *first_cylinder = NULL;
	  
	  //it can be used to directly access RigidBody component
	  //app_scene->add_shapeRB(mat, new mesh_sphere(vec3(2, 2, 2), 2), red, &first_sphere, true);

      //mat.loadIdentity();
      //mat.translate(0, 10, 0);
      //app_scene->add_shapeRB(mat, new mesh_box(vec3(2, 2, 2)), red, &first_box, true);

     /* mat.loadIdentity();
      mat.translate( 3, 6, 0);
      app_scene->add_shapeRB(mat, new mesh_cylinder(zcylinder(vec3(0, 0, 0), 2, 4)), blue, &first_cylinder, true);*/

	  //test bridge
	  create_bridge();


      // ground
      mat.loadIdentity();
      mat.translate(0, -1, 0);
      app_scene->add_shape(mat, new mesh_box(vec3(200, 1, 200)), red, false);

	  //inizializes map after reading csv
	  set_up_map();
    }

	void set_up_map() {
		btRigidBody *boxRB = NULL;
		mat4t mtw;
		//mtw.loadIdentity();
		
		
		

		for (int i = 0; i < map_height; i++) {
			for (int j = 0; j < map_width; j++) {
				mesh_instance m;
				
				if (map[i][j] == 1) {

					mesh_instance *slab = app_scene->add_shapeRB(mtw, new mesh_box(vec3(1, 1, 1), 1), new material(vec4(1, 0, 1, 1)), &boxRB, false);
					slabs.push_back(*slab);
					
					mtw.init(1.0f, 1.0f, 1.0f, 1.0f);
					mtw.loadIdentity();
				}
				
			}
		}

	
	}


    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      
	
	  int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);
	 
      
	  
	  //draw the map meshes //FIX IT
	  //for (unsigned int i = 0; i < slabs.size(); ++i) {
		 // 
		 // mat4t mtw;
		 // mtw.loadIdentity();
		 // mtw.translate(vec3(slabs[i].get_node() -> get_x(), (slabs[i].get_node() -> get_y(), 0.0f));
	  //}
	  //
	  
	  
	  
	  
	  // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);






      // draw the scene
      app_scene->render((float)vx / vy);





    }

	//this is a test
	void create_bridge() {
		
		
		btRigidBody *first_sphere = NULL;
		btRigidBody *boxRB = NULL;
		
		mat4t mtw;
		mtw.loadIdentity();
		mtw.translate(vec3(0, 0.5f, 0));
		mesh_instance *b1 = app_scene->add_shapeRB(mtw, new mesh_box(vec3(1, 1, 1), 1), new material(vec4(0, 0, 1, 1)), &boxRB, false);
		
		mtw.loadIdentity();
		mtw.translate(vec3(1.6f, 1.25f, 0.0f));
		mesh_instance *p1 = app_scene->add_shapeRB(mtw, new mesh_box(vec3(0.5f, 0.25f, 1)), new material(vec4(0, 1, 0, 1)), &boxRB, true);

		mtw.loadIdentity();
		mtw.translate(vec3(2.7f, 1.25f, 0.0f));
		mesh_instance *p2 = app_scene->add_shapeRB(mtw, new mesh_box(vec3(0.5f, 0.25f, 1)), new material(vec4(0, 1, 1, 1)),&boxRB, true);

		mtw.loadIdentity();
		mtw.translate(vec3(3.8f, 1.25f, 0.0f));
		mesh_instance *p3 = app_scene->add_shapeRB(mtw, new mesh_box(vec3(0.5f, 0.25f, 1)), new material(vec4(0, 1, 0, 1)),&boxRB, true);

		mtw.loadIdentity();
		mtw.translate(vec3(4.9f, 1.25f, 0.0f));
		mesh_instance *p4 = app_scene->add_shapeRB(mtw, new mesh_box(vec3(0.5f, 0.25f, 1)), new material(vec4(0, 1, 1, 1)), &boxRB, true);

		mtw.loadIdentity();
		mtw.translate(vec3(6.5f, 0.5f, 0.0f));
		mesh_instance *b2 = app_scene->add_shapeRB(mtw, new mesh_box(vec3(1, 1, 1)), new material(vec4(1, 0, 0, 1)), &boxRB, false);

	
		// hinges //FIX THIS

		btHingeConstraint *c1 = new btHingeConstraint(*(b1->get_node()->get_rigid_body()), *(p1->get_node()->get_rigid_body()),
			btVector3(0.5f, 0.5f, 0.0f), btVector3(-0.5f, 0.25f, 0.0f),
			btVector3(0, 0, 1), btVector3(0, 0, 1), false);
		c1->setLimit(-PI * 0.1f, PI* 0.1f);
		dynamics_world->addConstraint(c1);

		btHingeConstraint *c2 = new btHingeConstraint(*(p1->get_node()->get_rigid_body()), *(p2->get_node()->get_rigid_body()),
			btVector3(0.5f, 0.25f, 0.0f), btVector3(-0.5f, 0.25f, 0.0f),
			btVector3(0, 0, 1), btVector3(0, 0, 1), false);
		c2->setLimit(-PI * 0.1f, PI* 0.1f);
		dynamics_world->addConstraint(c2);

		btHingeConstraint *c3 = new btHingeConstraint(*(p2->get_node()->get_rigid_body()), *(p3->get_node()->get_rigid_body()),
			btVector3(0.5f, 0.25f, 0.0f), btVector3(-0.5f, 0.25f, 0.0f),
			btVector3(0, 0, 1), btVector3(0, 0, 1), false);
		c3->setLimit(-PI * 0.1f, PI* 0.1f);
		dynamics_world->addConstraint(c3);

		btHingeConstraint *c4 = new btHingeConstraint(*(p3->get_node()->get_rigid_body()), *(p4->get_node()->get_rigid_body()),
			btVector3(0.5f, 0.25f, 0.0f), btVector3(-0.5f, 0.25f, 0.0f),
			btVector3(0, 0, 1), btVector3(0, 0, 1), false);
		c4->setLimit(-PI * 0.1f, PI* 0.1f);
		dynamics_world->addConstraint(c4);

		btHingeConstraint *c5 = new btHingeConstraint(*(p4->get_node()->get_rigid_body()), *(b2->get_node()->get_rigid_body()),
			btVector3(0.5f, 0.25f, 0.0f), btVector3(-0.5f, 0.5f, 0.0f),
			btVector3(0, 0, 1), btVector3(0, 0, 1), false);
		c5->setLimit(-PI * 0.1f, PI* 0.1f);
		dynamics_world->addConstraint(c5);

	}


	void Read_Csv() {


		std::ifstream file("background_new.csv");

	

		// store the line here
		char buffer[2048];
		int i = 0;

		// loop over lines
		while (!file .eof()) {
			file.getline(buffer, sizeof(buffer));

			// loop over columns
			char *b = buffer;
			for (int j = 0; ; ++j) {
				char *e = b;
				while (*e != 0 && *e != ',') ++e;

				map[i][j] = std::atoi(b);

				if (*e != ',') break;
				b = e + 1;
			}
			++i;
		}
	}


  };
}
