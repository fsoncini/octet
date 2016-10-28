////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#include "plank.h"


namespace octet {
  /// Scene containing a box with octet.
  class example_shapes : public app {
    // scene for drawing box
    ref<visual_scene> app_scene;
	const float PI = 3.14159;
	btDiscreteDynamicsWorld *dynamics_world;

  public:
    example_shapes(int argc, char **argv) : app(argc, argv) {
    }

	btRigidBody *first_sphere = NULL;
	btRigidBody *first_box = NULL;

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
	 
	  btRigidBody *first_box = NULL;
	  btRigidBody *first_cylinder = NULL;
	  
	  //it can be used to directly access RigidBody component
	  app_scene->add_shapeRB(mat, new mesh_sphere(vec3(2, 2, 2), 2), red, &first_sphere, true);

      mat.loadIdentity();
      mat.translate(0, 10, 0);
      app_scene->add_shapeRB(mat, new mesh_box(vec3(2, 2, 2)), red, &first_box, true);

     /* mat.loadIdentity();
      mat.translate( 3, 6, 0);
      app_scene->add_shapeRB(mat, new mesh_cylinder(zcylinder(vec3(0, 0, 0), 2, 4)), blue, &first_cylinder, true);*/

	  //test bridge
	  create_bridge();


      // ground
      mat.loadIdentity();
      mat.translate(0, -1, 0);
      app_scene->add_shape(mat, new mesh_box(vec3(200, 1, 200)), green, false);
    }

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);

      // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);

      // draw the scene
      app_scene->render((float)vx / vy);
    }

	//this is just a test
	void create_bridge() {
		mat4t mtw;
		mtw.loadIdentity();
		mtw.translate(vec3(0, 0.5f, 0));
		mesh_instance *b1 = app_scene->add_shapeRB(mtw, new mesh_sphere(vec3(2, 2, 2), 2), new material(vec4(0, 0, 1, 1)), &first_sphere, false);

		mtw.loadIdentity();
		mtw.translate(vec3(1.6f, 1.25f, 0.0f));
		mesh_instance *p1 = app_scene->add_shapeRB(mtw, new mesh_box(vec3(0.5f, 0.25f, 1)), new material(vec4(0, 1, 0, 1)), &first_box, true);

		mtw.loadIdentity();
		mtw.translate(vec3(2.7f, 1.25f, 0.0f));
		mesh_instance *p2 = app_scene->add_shape(mtw, new mesh_box(vec3(0.5f, 0.25f, 1)), new material(vec4(0, 1, 1, 1)), true);

		mtw.loadIdentity();
		mtw.translate(vec3(3.8f, 1.25f, 0.0f));
		mesh_instance *p3 = app_scene->add_shape(mtw, new mesh_box(vec3(0.5f, 0.25f, 1)), new material(vec4(0, 1, 0, 1)), true);

		mtw.loadIdentity();
		mtw.translate(vec3(4.9f, 1.25f, 0.0f));
		mesh_instance *p4 = app_scene->add_shape(mtw, new mesh_box(vec3(0.5f, 0.25f, 1)), new material(vec4(0, 1, 1, 1)), true);

		mtw.loadIdentity();
		mtw.translate(vec3(6.5f, 0.5f, 0.0f));
		mesh_instance *b2 = app_scene->add_shape(mtw, new mesh_box(vec3(1, 1, 1)), new material(vec4(1, 0, 0, 1)), false);

		// hinges

		//btHingeConstraint *c1 = new btHingeConstraint(*first_sphere, *first_cylinder);
		//	btVector3(0.5f, 0.5f, 0.0f), btVector3(-0.5f, 0.25f, 0.0f),
		//	btVector3(0, 0, 1), btVector3(0, 0, 1), false);
		//c1->setLimit(-PI * 0.1f, PI* 0.1f);
		//dynamics_world->addConstraint(c1);





	}
  };
}
