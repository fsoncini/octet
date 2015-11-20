////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#include "federico_tree2.h"


namespace octet {
    class lsystems : public app {

        class node {
            vec3 pos;
            float angle;
        public:
            node() {
                pos = vec3(0.0f, 0.0f, 0.0f);
                angle = 0.0f;
            }

            node(vec3 pos_, float angle_) {
                pos = pos_;
                angle = angle_;
            }

            vec3& get_pos() {
                return pos;
            }

            float& get_angle() {
                return angle;
            }
        };

        const float PI = 3.14159265f;
        const float SEGMENT_LENGTH = 0.5f;
        float SEGMENT_WIDTH = 0.1f;

        ref<visual_scene> app_scene;

        tree t;

        dynarray<node> node_stack;

        float tree_max_y = 0.0f;

        material *material_wood;
        material *material_leaf;
        material *material_autumn_leaf;
        material *material_acqua;
        material *material_pastel_green;
        material *material_ocra;

        int current_example = 1; 
        int current_iteration = 0;
        int n = 1; // color index here?
        const int min_example = 1;
        const int MAX_example = 8;
        const int min_iteration = 0;
        const int MAX_iteration = 7;//is it right?

        float far_plane = 500.0f;
        float add_angle = 0.0f;



    public:
        lsystems(int argc, char **argv) : app(argc, argv) {
        }

        void app_init() {
            t.read_file(current_example);

            app_scene = new visual_scene();
            app_scene->create_default_camera_and_lights();
            app_scene->get_camera_instance(0)->get_node()->translate(vec3(0.0f, 0.0f, 1.0f));


            material_wood = new material(vec4(0.59f, 0.29f, 0.0f, 1.0f));//brown wood
            material_leaf = new material(vec4(0.0f, 0.4f, 0.0f, 1.0f)); //green leaf
            material_autumn_leaf = new material(vec4(0.47f, 0.06f, 0.19f, 1.0f));//autumn leaf
            material_acqua = new material(vec4(0.38f, 0.89f, 0.87f, 1.0f));//acqua
            material_pastel_green = new material(vec4(0.02f, 0.97f, 0.42f, 1.0f));//pastel green
            material_ocra = new material(vec4(0.94f, 0.78f, 0.08f, 1.0f));//ocra yellow

            create_geometry();
        }

        void draw_world(int x, int y, int w, int h) {

            handle_input();

            app_scene->begin_render(w, h);

            //glClearColor(0, 0, 0, 1);
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // changed directly in visual scene (is there another way?)

            app_scene->update(1.0f / 30.0f);

            app_scene->render((float)w / h);
        }

        
     

        void handle_input() {
            //add iterations   
            
            if (is_key_going_down(key_space)) {             
                    ++current_iteration;
                    t.apply();
                    draw_again();
                    std::cout << "current iteration: " << current_iteration << "\n";                
            
            }

            //reverse iterations
            if (is_key_down(key_esc)) {                
                --current_iteration;
                t.read_file(current_example);
                for (unsigned int i = 0; i <= current_iteration -1; i++){
                    t.apply();
                    draw_again();
                }          
                std::cout << "current iteration: " << current_iteration << "\n";
                
                              
            }

            if (is_key_going_down(key_f11)) {
                if (current_example < MAX_example)
                {
                    current_iteration = min_iteration;
                    ++current_example;
                    t.read_file(current_example);
                    

                    draw_again();
                    std::cout << "\ncurrent example: " << current_example << "\n";// check
                }
            }

            if (is_key_going_down(key_f10)) {
                if (current_example > min_example)
                {
                    --current_example;
                    t.read_file(current_example);

                    draw_again();
                    std::cout << "\ncurrent example: " << current_example << "\n";//check
                }

            }

            //zoom in
            if (is_key_down(key_up)){
                app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, -1.50f));          
            }
            //zoom out
            if (is_key_down(key_down)) {
                app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, 2.0f));
            }

            //move right
            if (is_key_down(key_right)) {
                app_scene->get_camera_instance(0)->get_node()->translate(vec3(1.0f, 0, 0.0f)); 
            }

            //move left
            if (is_key_down(key_left)) {
                app_scene->get_camera_instance(0)->get_node()->translate(vec3(-1.0f, 0, 0.0f));
            }


            //increase angle at current iteration
            if (is_key_down(key_f1)) {

                if (current_iteration) {
                    t.read_file(current_example);
                    add_angle += 1.5f;
                    for (unsigned int i = 1; i <= current_iteration; i++){
                        t.apply();
                        draw_again();
                    }
                }
                
            }

            //decrease angle at current iteration
            if (is_key_down(key_f2)) {

                if (current_iteration) {
                    t.read_file(current_example);
                    add_angle -= 1.5f;
                    for (unsigned int i = 1; i <= current_iteration; i++){
                        t.apply();
                        draw_again();
                        }
                    }
                }

            //if (current_iteration > 4) {
            //    app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, 0, 1.50f)); //fix this here
            //    
            //}
            
        }

        void draw_again(){

            app_scene = new visual_scene();
            app_scene->create_default_camera_and_lights();

            //setting the far plane further away so the tree doesn't disappear when zooming out
            app_scene->get_camera_instance(0)->set_far_plane(far_plane);

            tree_max_y = 0.0f;
            create_geometry();

            app_scene->get_camera_instance(0)->get_node()->translate(vec3(0, tree_max_y / 2.0f, 0.0f));

        }

        vec3 draw_segment(vec3 start_pos, float angle) {
            vec3 mid_pos;
            vec3 end_pos;

            mid_pos.x() = start_pos.x() + SEGMENT_LENGTH * cos((angle + 90.0f) * PI / 180.0f);
            mid_pos.y() = start_pos.y() + SEGMENT_LENGTH * sin((angle + 90.0f) * PI / 180.0f);
            end_pos.x() = start_pos.x() + SEGMENT_LENGTH * 2.0f * cos((90.0f + angle) * PI / 180.0f);
            end_pos.y() = start_pos.y() + SEGMENT_LENGTH * 2.0f * sin((90.0f + angle) * PI / 180.0f);

            if (tree_max_y < end_pos.y()) {
                tree_max_y = end_pos.y();
            }

            mat4t mtw;
            mtw.loadIdentity();
            mtw.translate(mid_pos);
            mtw.rotate(angle, 0.0f, 0.0f, 1.0f);

            
            mat4t mtw2;
            mtw2.loadIdentity();
            mtw2.rotate(90, 1, 0, 0);
            //mesh_box *box = new mesh_box(vec3(SEGMENT_WIDTH, SEGMENT_LENGTH, SEGMENT_WIDTH), mtw);
            mesh_cylinder *box = new mesh_cylinder(zcylinder(vec3(0), SEGMENT_WIDTH, SEGMENT_LENGTH), mtw2*mtw);

            scene_node *node = new scene_node();
            app_scene->add_child(node);
            
            if (current_iteration == 1) {
                app_scene->add_mesh_instance(new mesh_instance(node, box, material_wood));
            }

            if (current_iteration == 2) {
                app_scene->add_mesh_instance(new mesh_instance(node, box, material_leaf));
            }

            if (current_iteration == 3) {
                app_scene->add_mesh_instance(new mesh_instance(node, box, material_autumn_leaf));
            }

            if (current_iteration == 4) {
                app_scene->add_mesh_instance(new mesh_instance(node, box, material_acqua));
            }

            if (current_iteration == 5) {
                app_scene->add_mesh_instance(new mesh_instance(node, box, material_pastel_green));
            }

            if (current_iteration == 6) {
                app_scene->add_mesh_instance(new mesh_instance(node, box, material_ocra));
            }



         /*   if (current_iteration < 4){
                app_scene->add_mesh_instance(new mesh_instance(node, box, material_wood));
            }
            else {
                app_scene->add_mesh_instance(new mesh_instance(node, box, material_ocra));
            }
*/
            return end_pos;
        }

        
        void create_geometry() {
            dynarray<char> axiom = t.get_axiom();
            vec3 pos = vec3(0.0f, 0.0f, 0.0f);
            float angle = 0.0f;
            for (unsigned int i = 0; i < axiom.size(); ++i) {
                if (axiom[i] == '+') {

                    //std::cout << "\Angle: " << angle << "\n";//check
                    switch (current_example)
                    {
                    case 1: angle += (25.7f + add_angle);
                        break;
                    case 2: angle += (20.0f + add_angle);
                        break;
                    case 3: angle += (22.5f + add_angle);
                        break;
                    case 4: angle += (20.0f + add_angle);
                        break;
                    case 5: angle += (25.7f + add_angle);
                        break;
                    case 6: angle += (22.5f + add_angle);
                        break;
                    case 7: angle += (90.0f + add_angle);
                        break;
                    case 8: angle += (25.7f + add_angle);
                        break;                                             
                    }
                }
                else if (axiom[i] == '-') {

                    //std::cout << "\Angle: " << angle << "\n";//check

                    switch (current_example)
                    {
                    case 1: angle -= (25.7f + add_angle);
                        break;
                    case 2: angle -= (20.0f + add_angle);
                        break;
                    case 3: angle -= (22.5f + add_angle);
                        break;
                    case 4: angle -= (20.0f + add_angle);
                        break;
                    case 5: angle -= (25.7f + add_angle);
                        break;
                    case 6: angle -= (22.5f + add_angle);
                        break;
                    case 7: angle -= (90.0f + add_angle);
                        break;
                    case 8: angle -= (25.7f + add_angle);
                        break;
                    }
                }
                else if (axiom[i] == '[') {
                    node n = node(pos, angle);
                    node_stack.push_back(n);
                }
                else if (axiom[i] == ']') {
                    node n = node_stack[node_stack.size() - 1];
                    node_stack.pop_back();
                    angle = n.get_angle();
                    pos = n.get_pos();
                }

                /*else if (axiom[i] == 'A') {
                    
                    current_iteration = 4;
                }*/

                else if (axiom[i] == 'F') {
                    pos = draw_segment(pos, angle);
                }
   

            }
        }

    };
}