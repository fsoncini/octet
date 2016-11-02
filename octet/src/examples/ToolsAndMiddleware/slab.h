#ifndef SLAB
#define SLAB

#ifndef OCTET_BULLET
#define OCTET_BULLET 1
#endif
#include "..\..\octet.h"

namespace octet {

	class slab : public octet::resource {
	protected:
		scene_node *node;
		mat4t model_to_world;
		vec3 slab_size;
		mesh_box *slab_mesh;
		material *slab_material;

		btScalar slab_mass;
		btRigidBody *rigidbody;
		btMotionState *motion_state;

		//test
		vec3 translate;

	public:
		slab() {

		}
		//test adding trans
		slab(mat4t mtw, vec3 size, vec3 trans, material *mat, btScalar mass) {
			init(mtw, size, trans, mat, mass);
		}

		~slab() {

		}
		//test adding trans
		void init(mat4t_in mtw, vec3 size, vec3 trans, material *mat, btScalar mass = 1.0f) {
			model_to_world = mtw;
			slab_size = size;
			translate = trans; //test
			slab_material = mat;
			slab_mass = mass;
			btMatrix3x3 matrix(get_btMatrix3x3(model_to_world));
			btVector3 translation(get_btVector3(model_to_world[3].xyz()));
			motion_state = new btDefaultMotionState(btTransform(matrix, translation));

			btCollisionShape *shape = new btBoxShape(get_btVector3(size));
			btVector3 inertia;
			shape->calculateLocalInertia(mass, inertia);
			rigidbody = new btRigidBody(mass, motion_state, shape, inertia);
			rigidbody->setActivationState(DISABLE_DEACTIVATION);

			slab_mesh = new mesh_box(size);
			node = new scene_node(mtw, atom_);
		}

		btRigidBody* get_rigidbody() {
			return rigidbody;
		}

		scene_node* get_scene_node() {
			return node;
		}

		mesh_box* get_mesh() {
			return slab_mesh;
		}
		material* get_material() {
			return slab_material;
		}
		//test
		vec3 get_translate() {
			return translate;
		}

	};


}

#endif
