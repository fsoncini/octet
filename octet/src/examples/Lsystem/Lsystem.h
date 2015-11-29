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
        dynarray <material*> materials;

        material *material_wood;
        material *material_leaf;
        material *material_autumn_leaf;
        material *material_acqua;
        material *material_pastel_green;
        material *material_ocra;
        material *material_red;

        int current_example = 1;
        unsigned int current_iteration = 0;
        int n = 1; // color index here?
        const int min_example = 1;
        const int MAX_example = 8;
        int min_iteration = 0;
        int max_iteration = 5;
        

        float far_plane = 500.0f;
        float add_angle = 0.0f;
        float add_width = 0.0f;

        int trunk_counter = 0;
        bool example_mode = false;
              

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
            material_red = new material(vec4(0.94f, 0.03f, 0.20f, 1.0f)); //Santa Claus red

            
            materials.push_back(material_wood);
            materials.push_back(material_leaf);
            materials.push_back(material_autumn_leaf);
            materials.push_back(material_acqua);
            materials.push_back(material_pastel_green);
            materials.push_back(material_ocra);
            materials.push_back(material_red);

            create_geometry();
            std::cout << "\ncurrent example: " << min_example << "\n";
            std::cout << "\ncurrent iteration: " << min_iteration << "\n";

        }

        void draw_world(int x, int y, int w, int h) {
            handle_input();
            app_scene->begin_render(w, h);
            app_scene->update(1.0f / 30.0f);
            app_scene->render((float)w / h);

        }

        //resets example to first iteration
        void reset_to_first() {           
            current_iteration = min_iteration;
            t.read_file(current_example);
            draw_again();
            std::cout << "\ncurrent example: " << current_example << "\n";
            std::cout << "\ncurrent iteration: " << current_iteration << "\n";

        }

        //resets system to initial status
        void reboot() {
            current_example = min_example;
            current_iteration = min_iteration;
            t.read_file(current_example);
            draw_again();
            std::cout << "\ncurrent example: " << min_example << "\n";
            std::cout << "\ncurrent iteration: " << min_iteration << "\n";
        }

  
 
        //hotkeys are specified in this function
        void handle_input() {         
            
            if (is_key_going_down(key_space) && current_iteration < max_iteration) {
                ++current_iteration;

                //
                if (current_example == 1) {
                    max_iteration = 5;
                }
                else if (current_example == 2) {
                    max_iteration = 5;
                }
                else if (current_example == 3) {
                    max_iteration = 4;
                }
                else if (current_example == 4) {
                    //min_iteration = 0;
                    max_iteration = 7;
                }
                else if (current_example == 5) {
                    //min_iteration = 0;
                    max_iteration = 7;
                }
                else if (current_example == 6) {
                    //min_iteration = 0;
                    max_iteration = 7; //one more because of X 
                }
                else if (current_example == 7) {
                    max_iteration = 6;
                }

                else if (current_example == 8) {
                    max_iteration = 6;
                }

             
                if (current_iteration < max_iteration){ 
                    t.apply();
                    draw_again();
                    std::cout << "\ncurrent example: " << current_example << "\n";
                    std::cout << "current iteration: " << current_iteration << "\n";
                }
                //fix this
                else if (current_iteration == max_iteration) {           
                    reset_to_first();
                    std::cout << "\nIteration limit reached. \n";
                }
            }
            //reverse iterations
            if (is_key_going_down(key_esc)) {
                --current_iteration;
                t.read_file(current_example);
                for (unsigned int i = 0; i < current_iteration; ++i)
                    t.apply();
                    draw_again();
                
                std::cout << "current iteration: " << current_iteration << "\n";
            }

            //move forward through examples 1-8
            if (is_key_going_down(key_f11)) {
                ++current_example;
                if (current_example < MAX_example) {
                        current_iteration = min_iteration;
                        t.read_file(current_example);
                        draw_again();
                        std::cout << "\ncurrent example: " << current_example << "\n";
                        std::cout << "\ncurrent iteration: " << min_iteration << "\n";
            
                }

                else if (current_example == MAX_example) {
                    reset_to_first();                  
                }
            }

            //move backward through examples 1-8
            if (is_key_going_down(key_backspace)) {
                --current_example;
                if (current_example > min_example)
                {
                    current_iteration = 0;
                    t.read_file(current_example);
                    draw_again();
                    std::cout << "\ncurrent example: " << current_example << "\n";//check
                }

                else if (current_example <= min_example) {
                    reboot();
                }
            }

            //reset to first iteration                        
            if (is_key_down(key_f4)) {
             reset_to_first();
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
            if (is_key_going_down(key_f1)) {

                if (current_iteration) {
                    t.read_file(current_example);
                    add_angle += 1.5f;
                    for (unsigned int i = 1; i <= current_iteration; i++){
                        t.apply();
                        draw_again();
                    }
                }
            }

            //Rotation (adjust and change)
            if (is_key_down(key_delete))
            {
                for (int i = 0; i < app_scene->get_num_mesh_instances(); ++i) {
                    mesh_instance *mi = app_scene->get_mesh_instance(i);
                    mi->get_node()->rotate(2.0f, vec3(0, 1, 0));
                }
            }

            //resume here
            if (is_key_down(key_tab))
            {
                for (int i = 0; i < app_scene->get_num_mesh_instances(); ++i) {
                    mesh_instance *mi = app_scene->get_mesh_instance(i);
                    mi->get_node()->rotate(2.0f, vec3(0, -1, 0));
                }
            }
                

            //increase segment width
            if (is_key_going_down(key_f8)) {
                if (current_iteration > min_iteration) {
                    t.read_file(current_example);
                    add_width += 0.05f;
                    for (unsigned int i = 1; i <= current_iteration; i++) {
                        t.apply();
                        draw_again();
                    }
                }
            }

            //decrease segment width 
            if (is_key_going_down(key_f7)) { 
                if (current_iteration > min_iteration) {
                t.read_file(current_example);
                add_width -= 0.05f;
                for (unsigned int i = 1; i <= current_iteration; i++) {
                        t.apply();
                        draw_again();
                    }
                }
            }
                                           
            //decrease angle at current iteration
            if (is_key_going_down(key_f2)) {
                if (current_iteration) {
                    t.read_file(current_example);
                    add_angle -= 1.5f;
                    for (unsigned int i = 1; i <= current_iteration; i++){
                        t.apply();
                        draw_again();
                    }
                }
            }

            //switch to choose between normal mode and example mode
            if (is_key_going_down(key_f3)) {
                example_mode = !example_mode;
                draw_again();
            }
            
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
            

            mesh_cylinder *box = new mesh_cylinder(zcylinder(vec3(0), SEGMENT_WIDTH + add_width, SEGMENT_LENGTH), mtw2*mtw);
            
            scene_node *node = new scene_node();
            app_scene->add_child(node);

            if (example_mode) {
                app_scene->add_mesh_instance(new mesh_instance(node, box, materials[trunk_counter%materials.size()]));
            }
            else {
                app_scene->add_mesh_instance(new mesh_instance(node, box, materials[current_iteration%materials.size()]));
            }
                           
            return end_pos;
        }


        void create_geometry() {
            dynarray<char> axiom = t.get_axiom();
            vec3 pos = vec3(0.0f, 0.0f, 0.0f);
            float angle = 0.0f;
            for (unsigned int i = 0; i < axiom.size(); ++i) {
                if (axiom[i] == '+') {

                    
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
                    case 8: angle += (90.0f + add_angle);
                        break;                                             
                    }
                }
                else if (axiom[i] == '-') {               

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
                    case 8: angle -= (90.0f + add_angle);
                        break;
                    }
                }
                else if (axiom[i] == '[') {
                    node n = node(pos, angle);
                    node_stack.push_back(n);
                    trunk_counter++;
                }
                else if (axiom[i] == ']') {
                    node n = node_stack[node_stack.size() - 1];
                    node_stack.pop_back();
                    angle = n.get_angle();
                    pos = n.get_pos();
                    trunk_counter--;
                }
        
                else if (axiom[i] == 'F') {
                    pos = draw_segment(pos, angle);
                }
            }
        }

    };
}