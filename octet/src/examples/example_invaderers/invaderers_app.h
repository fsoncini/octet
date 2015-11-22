////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
// invaderer example: simple game with sprites and sounds
//
// Level: 1
//
// Demonstrates:
//   Basic framework app
//   Shaders
//   Basic Matrices
//   Simple game mechanics
//   Texture loaded from GIF file
//   Audio
//

namespace octet {
    class sprite {
        // where is our sprite (overkill for a 2D game!)
        mat4t modelToWorld;

        // half the width of the sprite
        float halfWidth;

        // half the height of the sprite
        float halfHeight;

        // what texture is on our sprite
        int texture;

        // true if this sprite is enabled.
        bool enabled;

        bool isFacingRight;
    public:
        sprite() {
            texture = 0;
            enabled = true;
        }

        void init(int _texture, float x, float y, float w, float h) {
            modelToWorld.loadIdentity();
            modelToWorld.translate(x, y, 0);
            halfWidth = w * 0.5f;
            halfHeight = h * 0.5f;
            texture = _texture;
            enabled = true;
        }

        void render(texture_shader &shader, mat4t &cameraToWorld) {
            // invisible sprite... used for gameplay.
            if (!texture) return;

            // build a projection matrix: model -> world -> camera -> projection
            // the projection space is the cube -1 <= x/w, y/w, z/w <= 1
            mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);

            // set up opengl to draw textured triangles using sampler 0 (GL_TEXTURE0)
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);

            // use "old skool" rendering
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            shader.render(modelToProjection, 0);

            // this is an array of the positions of the corners of the sprite in 3D
            // a straight "float" here means this array is being generated here at runtime.
            float vertices[] = {
                -halfWidth, -halfHeight, 0,
                halfWidth, -halfHeight, 0,
                halfWidth, halfHeight, 0,
                -halfWidth, halfHeight, 0,
            };

            // attribute_pos (=0) is position of each corner
            // each corner has 3 floats (x, y, z)
            // there is no gap between the 3 floats and hence the stride is 3*sizeof(float)
            glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)vertices);
            glEnableVertexAttribArray(attribute_pos);

            // this is an array of the positions of the corners of the texture in 2D
            static const float uvs[] = {
                0, 0,
                1, 0,
                1, 1,
                0, 1,
            };

            // attribute_uv is position in the texture of each corner
            // each corner (vertex) has 2 floats (x, y)
            // there is no gap between the 2 floats and hence the stride is 2*sizeof(float)
            glVertexAttribPointer(attribute_uv, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)uvs);
            glEnableVertexAttribArray(attribute_uv);

            // finally, draw the sprite (4 vertices)
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }

        void render(federico_shader &shader, mat4t &cameraToWorld, vec4 color) {
            mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);

            shader.render(modelToProjection, color);

            float vertices[] = {
                -halfWidth, -halfHeight, 0,
                halfWidth, -halfHeight, 0,
                halfWidth, halfHeight, 0,
                -halfWidth, halfHeight, 0,
            };

            glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)vertices);
            glEnableVertexAttribArray(attribute_pos);

            static const float uvs[] = {
                0, 0,
                1, 0,
                1, 1,
                0, 1,
            };

            glVertexAttribPointer(attribute_uv, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)uvs);
            glEnableVertexAttribArray(attribute_uv);

            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }

        // move the object
        void translate(float x, float y) {
            modelToWorld.translate(x, y, 0);
        }
        // position the object relative to another.
        void set_relative(sprite &rhs, float x, float y) {
            modelToWorld = rhs.modelToWorld;
            modelToWorld.translate(x, y, 0);
        }

        vec2 get_position() {
            return vec2(modelToWorld[3][0], modelToWorld[3][1]);
        }

        void rotate_z(float angle) {
            modelToWorld.rotateZ(angle);
        }

        void swap_sprite(int texture_) {
            texture = texture_;
        }

        bool& is_facing_right() {
            return isFacingRight;
        }

        // return true if this sprite collides with another.
        // note the "const"s which say we do not modify either sprite
        bool collides_with(const sprite &rhs) const {
            float dx = rhs.modelToWorld[3][0] - modelToWorld[3][0];
            float dy = rhs.modelToWorld[3][1] - modelToWorld[3][1];

            // both distances have to be under the sum of the halfwidths
            // for a collision
            return
                (fabsf(dx) < halfWidth + rhs.halfWidth) &&
                (fabsf(dy) < halfHeight + rhs.halfHeight)
                ;
        }

        bool is_above(const sprite &rhs, float margin) const {
            float dx = rhs.modelToWorld[3][0] - modelToWorld[3][0];

            return
                (fabsf(dx) < halfWidth + margin)
                ;
        }

        bool &is_enabled() {
            return enabled;
        }
    };

    class invaderers_app : public octet::app {
        // Matrix to transform points in our camera space to the world.
        // This lets us move our camera
        mat4t cameraToWorld;

        // shader to draw a textured triangle
        texture_shader texture_shader_;
        federico_shader federico_shader_;

        enum {
            num_sound_sources = 8, //changed from 8 to 9 for boss music and back to 8
            num_missiles = 2,
            num_bombs = 2,
            num_borders = 4,
            num_boss_missiles = 3,

            // sprite definitions
            ship_sprite = 0,
            game_over_sprite,
            you_win_sprite,

            first_missile_sprite,
            last_missile_sprite = first_missile_sprite + num_missiles - 1,

            first_bomb_sprite,
            last_bomb_sprite = first_bomb_sprite + num_bombs - 1,

            first_boss_missile,
            last_boss_missile = first_boss_missile + num_boss_missiles - 1,

            num_sprites,

        };

        // timers for missiles and bombs
        int missiles_disabled;
        int bombs_disabled;
        int boss_missiles_disabled;

        // accounting for bad guys
        int live_invaderers;
        int num_lives;

        // game state
        bool game_over;
        int score;

        // speed of enemy
        float invader_velocity;

        // sounds
        ALuint whoosh;
        ALuint bang;
        //ALuint boss_music;
        unsigned cur_source;
        ALuint sources[num_sound_sources];

        // big array of sprites
        sprite sprites[num_sprites];

        // random number generator
        class random randomizer;

        // a texture for our text
        GLuint font_texture;

        // information for our text
        bitmap_font font;

        //Declare arrays and global variables, used in reading and implementing Csv file
        static const int map_width = 61;
        static const int map_height = 20;
        int map[map_height][map_width];
        int map2[map_height][map_width];
        dynarray<sprite> map_sprite_background;
        dynarray<sprite> map_sprite_background2;
        dynarray<sprite> invaderers;
        dynarray<sprite> vampires;
        //dynarray<sprite> coins; possibly i dont need it here fix
        sprite bg_sprite;
        sprite boss_key;

        //Declare global variables for main character sprite
        float sir_arthur_height = 0.35f;
        float sir_arthur_width = 0.35f;

        //Declare bool for jumping state
        bool isJumping = false;
        bool canJump = true;
        int jumpFrameCount = 0;

        //Endgame stuff
        bool isBossEnabled = false;
        sprite boss_sprite;
        int boss_lives = 5;

        bool isBossJumping = false;
        bool canBossJump = false;
        int bossJumpFrameCount = 0;
        int timeWithoutArmor = 0;

        ALuint get_sound_source() { return sources[cur_source++ % num_sound_sources]; }

        //called to read a CSV file for the background and borders map
        void read_csv() {

            std::ifstream file("background.csv");

            char buffer[2048];
            int i = 0;

            while (!file.eof()) {
                file.getline(buffer, sizeof(buffer));

                char *b = buffer;
                for (int j = 0;; ++j) {
                    char *e = b;
                    while (*e != 0 && *e != ',') ++e;

                    map[i][j] = std::atoi(b);

                    if (*e != ',') break;
                    b = e + 1;
                }
                ++i;
            }
        }
        
        void read_csv2() {
            std::ifstream file("background2.csv");

            char buffer[2048];
            int i = 0;

            while (!file.eof()) {
                file.getline(buffer, sizeof(buffer));

                char *b = buffer;
                for (int j = 0;; ++j) {
                    char *e = b;
                    while (*e != 0 && *e != ',') ++e;

                    map2[i][j] = std::atoi(b);

                    if (*e != ',') break;
                    b = e + 1;
                }
                ++i;
            }

        }
       
        //called to initialize the background and borders maps from the CSV file
        void setup_visual_map() {

            GLuint bush = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/grass.gif");
            GLuint invaderer = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/invaderer.gif");
            GLuint vampire = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/vampire.gif"); 
            GLuint skull = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/skull.gif");

            for (int i = 0; i < map_height; ++i) {
                for (int j = 0; j < map_width; ++j) {

                    sprite s;

                    if (map[i][j] == 1) {

                        s.init(bush, -3 + 0.15f + 0.3f*j, 3 - 0.15f - 0.3f*i, 0.3f, 0.3f);
                        map_sprite_background.push_back(s);
                        //num_bush++;
                    }
                    else if (map[i][j] == 2) {
                        s.init(bush, -3 + 0.15f + 0.3f*j, 3 - 0.15f - 0.3f*i, 0.3f, 0.3f);
                        s.is_enabled() = false;
                        map_sprite_background.push_back(s);
                    }
                    else if (map[i][j] == 3) {
                        s.init(invaderer, -3 + 0.15f + 0.3f*j, 3 - 0.15f - 0.3f*i, 0.25f, 0.25f);
                        invaderers.push_back(s);
                    }
                    else if (map[i][j] == 4) {
                        s.init(vampire, -3 + 0.15f + 0.3f*j, 3 - 0.12f - 0.3f*i, 0.7f, 0.35f);
                        s.is_facing_right() = false;
                        vampires.push_back(s);
                    }
                    else if (map[i][j] == 5) {
                        boss_key.init(skull, -3 + 0.15f + 0.3f*j, 3 - 0.12f - 0.3f*i, 0.3f, 0.3f); 
                    }
                }
            }
        }
         
        //set up visual map for boss level
        void setup_visual_map2() {
            GLuint bush2 = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/grass2.gif");
                                  
            for (int i = 0; i < map_height; ++i) {
                for (int j = 0; j < map_width; ++j) {

                    sprite s;

                    if (map2[i][j] == 1) {
                        s.init(bush2, -3 + 0.15f + 0.3f*j, 3 - 0.15f - 0.3f*i, 0.3f, 0.3f);
                        map_sprite_background2.push_back(s);
                        //num_bush++;
                    }

                    else if (map2[i][j] == 2) {
                        s.init(bush2, -3 + 0.15f + 0.3f*j, 3 - 0.15f - 0.3f*i, 0.3f, 0.3f);
                        s.is_enabled() = false;
                        map_sprite_background2.push_back(s);
                    }

                }
            }

        }

        // called when we hit an enemy
        void on_hit_invaderer() {
            ALuint source = get_sound_source();
            alSourcei(source, AL_BUFFER, bang);
            alSourcePlay(source);

            live_invaderers--;
            score++;
            if (live_invaderers == 4) {
                invader_velocity *= 4;
            }
            else if (live_invaderers == 0) {
                game_over = true;
                sprites[game_over_sprite].translate(-20, 0);
            }
        }


        // called when we are hit
        void on_hit_ship() {
            ALuint source = get_sound_source();
            alSourcei(source, AL_BUFFER, bang);
            alSourcePlay(source);

            if (--num_lives == 0) {
                game_over = true;
                float dx = bg_sprite.get_position().x() - sprites[game_over_sprite].get_position().x();
                sprites[ship_sprite].translate(0, -20);
                sprites[game_over_sprite].translate(dx, 0);
               
                
            }
        }

        void on_hit_boss() {
            ALuint source = get_sound_source();
            alSourcei(source, AL_BUFFER, bang);
            alSourcePlay(source);

            if (--boss_lives == 0) {
                boss_sprite.translate(0.0f, -20);
                game_over = true;
                float dx = bg_sprite.get_position().x() - sprites[you_win_sprite].get_position().x();
                sprites[you_win_sprite].translate(dx, 0);
                sprites[ship_sprite].translate(0.0f, -20);
            }
        }


        // use the keyboard to move the ship left, right, up and down

        void move_ship() {

            const float ship_speed = 0.05f;

            if (is_key_going_down(key_up) && canJump && !isJumping) {
                isJumping = true;
                canJump = false;
            }

            if (isJumping && jumpFrameCount <= 20) {
                sprites[ship_sprite].translate(0, ship_speed);
                ++jumpFrameCount;
                if (!isBossEnabled) {
                    for (unsigned int i = 0; i < map_sprite_background.size(); ++i) {
                        if (sprites[ship_sprite].collides_with(map_sprite_background[i])) {
                            isJumping = false;
                        }
                    }
                }
                if (isBossEnabled) {
                    for (unsigned int i = 0; i < map_sprite_background2.size(); ++i) {
                        if (sprites[ship_sprite].collides_with(map_sprite_background2[i])) {
                            isJumping = false;
                        }
                    }
                }
            }

            else if (jumpFrameCount > 20) {
                isJumping = false;
            }
                
            if (!isJumping) {
                sprites[ship_sprite].translate(0, -ship_speed);
                if (!isBossEnabled) {
                    for (unsigned int i = 0; i < map_sprite_background.size(); ++i) {
                        if (sprites[ship_sprite].collides_with(map_sprite_background[i])) {
                            sprites[ship_sprite].translate(0, ship_speed);
                            jumpFrameCount = 0;
                            canJump = true;
                        }
                    }
                }
                if (isBossEnabled) {
                    for (unsigned int i = 0; i < map_sprite_background2.size(); ++i) {
                        if (sprites[ship_sprite].collides_with(map_sprite_background2[i])) {
                            sprites[ship_sprite].translate(0, ship_speed);
                            jumpFrameCount = 0;
                            canJump = true;
                        }
                    }
                }
            }

            if (is_key_down(key_left)) {
                sprites[ship_sprite].translate(-ship_speed, 0);
                sprites[ship_sprite].is_facing_right() = false;

                if (!isBossEnabled) {
                    for (unsigned int i = 0; i < map_sprite_background.size(); i++) {
                        if (sprites[ship_sprite].collides_with(map_sprite_background[i])){
                            sprites[ship_sprite].translate(+ship_speed, 0);
                        }
                    }
                }
                if (isBossEnabled) {
                    for (unsigned int i = 0; i < map_sprite_background2.size(); i++) {
                        if (sprites[ship_sprite].collides_with(map_sprite_background2[i])){
                            sprites[ship_sprite].translate(+ship_speed, 0);
                        }
                    }                
                }
            }

            else if (is_key_down(key_right)) {
                sprites[ship_sprite].translate(+ship_speed, 0);
                sprites[ship_sprite].is_facing_right() = true;

                if (!isBossEnabled) {
                    for (unsigned int i = 0; i < map_sprite_background.size(); i++) {
                        if (sprites[ship_sprite].collides_with(map_sprite_background[i])){
                            sprites[ship_sprite].translate(-ship_speed, 0);
                        }
                    }
                }
                if (isBossEnabled) {
                    for (unsigned int i = 0; i < map_sprite_background2.size(); i++) {
                        if (sprites[ship_sprite].collides_with(map_sprite_background2[i])){
                            sprites[ship_sprite].translate(-ship_speed, 0);
                        }
                    }                
                }

            }

            if (boss_key.is_enabled() && boss_key.collides_with(sprites[ship_sprite])) { 
                isBossEnabled = true;
                invaderers.resize(0);
                vampires.resize(0);
                sprite &bomb = sprites[first_bomb_sprite];
                bomb.translate(0, -20);
                boss_key.is_enabled() = false;

                GLuint boss = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/boss_flipped.gif");
                float dx = sprites[ship_sprite].get_position().x() - 3.5f;
                float dy = sprites[ship_sprite].get_position().y() + 4.0f;
                boss_sprite.init(boss, dx, dy, 1.0f, 1.1f);
                boss_sprite.is_facing_right() = true;
            }

            bg_sprite.translate(sprites[ship_sprite].get_position().x() - bg_sprite.get_position().x(), 0.0f);
            cameraToWorld.translate(sprites[ship_sprite].get_position().x() - cameraToWorld[3][0], 0.0f, 0.0f);
            if (cameraToWorld[3][0] < 0.0f) {
                cameraToWorld.translate(-cameraToWorld[3][0], 0.0f, 0.0f);
                bg_sprite.translate(-bg_sprite.get_position().x(), 0.0f);
            }
            else if (cameraToWorld[3][0] > map_width*0.3f - 6.0f) {
                cameraToWorld.translate(map_width*0.3f - 6.0f - cameraToWorld[3][0], 0.0f, 0.0f);
                bg_sprite.translate(map_width*0.3f - 6.0f - bg_sprite.get_position().x(), 0.0f);
            }
            
            
        }

        // fire button (space)
        void fire_missiles() {
            if (missiles_disabled) {
                --missiles_disabled;
            }
            else if (is_key_going_down(' ')) {
                // find a missile
                for (int i = 0; i != num_missiles; ++i) {
                    if (!sprites[first_missile_sprite + i].is_enabled()) {
                        sprites[first_missile_sprite + i].set_relative(sprites[ship_sprite], 0.0f, 0.0f);
                        sprites[first_missile_sprite + i].is_enabled() = true;

                        GLuint miss = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/missile_horizontal.gif");
                        GLuint miss_left = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/missile_horizontal_flipped.gif");

                        if (sprites[ship_sprite].is_facing_right()) {
                            sprites[first_missile_sprite + i].swap_sprite(miss);
                            sprites[first_missile_sprite + i].is_facing_right() = true;
                        }
                        else {
                            sprites[first_missile_sprite + i].swap_sprite(miss_left);
                            sprites[first_missile_sprite + i].is_facing_right() = false;
                        }


                        missiles_disabled = 5;
                        ALuint source = get_sound_source();
                        alSourcei(source, AL_BUFFER, whoosh);
                        alSourcePlay(source);
                        break;
                    }
                }
            }
        }

        // pick an invader and fire a bomb
        void fire_bombs() {
            if (bombs_disabled) {
                --bombs_disabled;
            }
            else {
                // find an invaderer
                sprite &ship = sprites[ship_sprite];
                for (int j = randomizer.get(0, invaderers.size()); j < invaderers.size(); ++j) {
                    sprite &invaderer = invaderers[j];
                    if (invaderer.is_enabled() && invaderer.is_above(ship, 0.3f)) {
                        // find a bomb
                        for (int i = 0; i != num_bombs; ++i) {
                            if (!sprites[first_bomb_sprite + i].is_enabled()) {
                                sprites[first_bomb_sprite + i].set_relative(invaderer, 0.0f, -0.25f);
                                sprites[first_bomb_sprite + i].is_enabled() = true;
                                bombs_disabled = 30;
                                ALuint source = get_sound_source();
                                alSourcei(source, AL_BUFFER, whoosh);
                                alSourcePlay(source);
                                return;
                            }
                        }
                        return;
                    }
                }
            }
        }

        // animate the missiles
        void move_missiles() {
            const float missile_speed = 0.3f;
            for (int i = 0; i != num_missiles; ++i) {
                sprite &missile = sprites[first_missile_sprite + i];
                if (missile.is_enabled()) {
                    if (missile.is_facing_right()) {
                        missile.translate(missile_speed, 0);
                    }
                    else {
                        missile.translate(-missile_speed, 0);
                    }
                    
                    if (!isBossEnabled) {
                        for (int j = 0; j != invaderers.size(); ++j) {
                            sprite &invaderer = invaderers[j];
                            if (invaderer.is_enabled() && missile.collides_with(invaderer)) {
                                invaderer.is_enabled() = false;
                                invaderer.translate(20, 0);
                                missile.is_enabled() = false;
                                missile.translate(20, 0);
                                on_hit_invaderer();

                                goto next_missile;
                            }
                        }

                        for (int j = 0; j != vampires.size(); ++j) {
                            sprite &vampire = vampires[j];
                            if (vampire.is_enabled() && missile.collides_with(vampire)) {
                                vampire.is_enabled() = false;
                                vampire.translate(20, 0);
                                missile.is_enabled() = false;
                                missile.translate(20, 0);
                                on_hit_invaderer();

                                goto next_missile;
                            }
                        }
                    }
                    else {
                        if (missile.collides_with(boss_sprite)) {
                            sprites[first_missile_sprite].translate(0, -20);
                            on_hit_boss();
                        }
                    }
                    

                    for (unsigned int j = 0; j < map_sprite_background.size(); ++j) {
                        if (missile.collides_with(map_sprite_background[j])) {
                            missile.is_enabled() = false;
                            missile.translate(20, 0);
                        }
                    }
                }
            next_missile:;
            }
        }

        // animate the bombs
        void move_bombs() {
            const float bomb_speed = 0.2f;
            for (int i = 0; i != num_bombs; ++i) {
                sprite &bomb = sprites[first_bomb_sprite + i];
                if (bomb.is_enabled()) {
                    bomb.translate(0, -bomb_speed);
                    if (bomb.collides_with(sprites[ship_sprite])) {
                        bomb.is_enabled() = false;
                        bomb.translate(0, -20);
                        bombs_disabled = 50;
                        on_hit_ship();
                        goto next_bomb;
                    }
                    for (unsigned int j = 0; j < map_sprite_background.size(); ++j) {
                        if (bomb.collides_with(map_sprite_background[j])) {
                            bomb.is_enabled() = false;
                            bomb.translate(0, -20);
                        }
                    }
                }
            next_bomb:;
            }
        }

        // move the array of enemies
        void move_invaders(float dx, float dy) {
            for (int j = 0; j != invaderers.size(); ++j) {
                sprite &invaderer = invaderers[j];
                if (invaderer.is_enabled()) {
                    invaderer.translate(dx, dy);
                }
            }
        }

        //make vampires move toward sir arthur ("ship")
        void move_vampires(float dx) {
            const float ship_speed = 0.05f;

            for (unsigned int i = 0; i < vampires.size(); ++i) {
                sprite &vampire = vampires[i];
                if (vampire.is_enabled()) {
                    float distance = vampire.get_position().x() - sprites[ship_sprite].get_position().x();
                    if (fabsf(distance) <= 3.0f) {
                        if (distance < 0.0f) {
                            vampire.translate(dx, -ship_speed);
                            vampire.is_facing_right() = false;
                        }
                        else {
                            vampire.translate(-dx, -ship_speed);
                            vampire.is_facing_right() = true;
                        }
                    }
                    else {
                        vampire.translate(0, -ship_speed);
                    }

                    for (unsigned int j = 0; j < map_sprite_background.size(); ++j) {
                        if (vampire.collides_with(map_sprite_background[j])) {
                            vampire.translate(0, ship_speed);
                        }
                    }
                }
            }
        }

        //make sir arthur jump back when he is hit once
        void vampire_attack() {
            for (unsigned int i = 0; i < vampires.size(); ++i) {
                if (vampires[i].collides_with(sprites[ship_sprite])) {
                    float distance = vampires[i].get_position().x() - sprites[ship_sprite].get_position().x();
                    if (distance > 0.0f) {
                        if (sprites[ship_sprite].get_position().x() < -3.0f + 0.3f + 1.0f + sir_arthur_width / 2.0f) {
                            float dx = -3.0f + 0.3f + sir_arthur_width / 2.0f - sprites[ship_sprite].get_position().x();
                            sprites[ship_sprite].translate(dx + 0.1f, 0.0f);
                        }
                        else {
                            sprites[ship_sprite].translate(-1.0f, 0.0f);
                        }

                    }
                    else {
                        if (sprites[ship_sprite].get_position().x() > 0.3f*map_width-3.0f - 0.3f - 1.0f - sir_arthur_width / 2.0f) {
                            float sx = 0.3f*map_width - 3.0f - 0.3f - sir_arthur_width / 2.0f - sprites[ship_sprite].get_position().x();
                            sprites[ship_sprite].translate(sx - 0.1f, 0.0f);
                        }

                        else
                        {
                            sprites[ship_sprite].translate(1.0f, 0.0f);
                        }
                    }

                    if (--num_lives == 0) {
                        game_over = true;
                        float dx = bg_sprite.get_position().x() - sprites[game_over_sprite].get_position().x();
                        sprites[game_over_sprite].translate(dx, 0);
                    }
                }
            }   
            
        }

        // check if any invaders hit the sides.
        bool invaders_collide(sprite &border) {
            for (int j = 0; j != invaderers.size(); ++j) {
                sprite &invaderer = invaderers[j];
                if (invaderer.is_enabled() && invaderer.collides_with(border)) {
                    return true;
                }
            }
            return false;
        }

       //make boss move, jump and attack
        void move_boss() {
            const float boss_speed = 0.02f;
            float distance = sprites[ship_sprite].get_position().x() - boss_sprite.get_position().x();
            if (distance < 0.0f) {
                boss_sprite.is_facing_right() = false;
                boss_sprite.translate(-boss_speed, 0.0f);
                //here this piece of code below (seems ok now)
                for (unsigned int i = 0; i < map_sprite_background2.size(); ++i) {
                    if (boss_sprite.collides_with(map_sprite_background2[i])) {
                        boss_sprite.translate(boss_speed, 0.0f);
                        bossJumpFrameCount = 0;
                        canBossJump = true;
                    }
                }
            }
            else {
                boss_sprite.is_facing_right() = true;
                boss_sprite.translate(boss_speed, 0.0f);
                //here more tentative piece of code
                for (unsigned int i = 0; i < map_sprite_background2.size(); ++i) {
                    if (boss_sprite.collides_with(map_sprite_background2[i])) {
                        boss_sprite.translate(-boss_speed, 0.0f);
                        bossJumpFrameCount = 0;
                        canBossJump = true;
                    }
                }
            }       


            if (canBossJump && !isBossJumping) {
                float jumpProb = randomizer.get(0.0f, 100.0f);
                if (jumpProb > 95.0f) {
                    isBossJumping = true;
                    canBossJump = false;
                }
            }

            if (isBossJumping && bossJumpFrameCount <= 50) {
                boss_sprite.translate(0.0f, boss_speed);
                ++bossJumpFrameCount;
            }
            else if (bossJumpFrameCount > 50) {
                isBossJumping = false;
            }

            if (!isBossJumping) {
                boss_sprite.translate(0.0f, -boss_speed);
                for (unsigned int i = 0; i < map_sprite_background2.size(); ++i) {
                    if (boss_sprite.collides_with(map_sprite_background2[i])) {
                        boss_sprite.translate(0.0f, boss_speed);
                        bossJumpFrameCount = 0;
                        canBossJump = true;
                    }
                }
            }
           
            if (isBossJumping) {
                boss_sprite.translate(0.0f, boss_speed);
                for (unsigned int i = 0; i < map_sprite_background2.size(); ++i) {
                    if (boss_sprite.collides_with(map_sprite_background2[i])) {
                        boss_sprite.translate(0.0f, -boss_speed);
                        bossJumpFrameCount = 0;
                        canBossJump = true; //here
                    }
                }
              
            }
       

            //make sir arthur ("ship") jump back when he loses his armor
            if (boss_sprite.collides_with(sprites[ship_sprite])) {
                float distance = boss_sprite.get_position().x() - sprites[ship_sprite].get_position().x();

                if (distance > 0.0f) {
                    if (sprites[ship_sprite].get_position().x() < -3.0f + 0.3f + 1.0f + sir_arthur_width / 2.0f) {
                        float dx = -3.0f + 0.3f + sir_arthur_width / 2.0f - sprites[ship_sprite].get_position().x();
                        sprites[ship_sprite].translate(dx + 0.1f, 0.0f);
                    }
                    else {
                        sprites[ship_sprite].translate(-1.0f, 0.0f);
                    }

                }
                else {
                    if (sprites[ship_sprite].get_position().x() > 0.3f*map_width - 3.0f - 0.3f - 1.0f - sir_arthur_width / 2.0f) {
                        float sx = 0.3f*map_width - 3.0f - 0.3f - sir_arthur_width / 2.0f - sprites[ship_sprite].get_position().x();
                        sprites[ship_sprite].translate(sx - 0.1f, 0.0f);
                    }

                    else
                    {
                        sprites[ship_sprite].translate(1.0f, 0.0f);
                    }
                }

                on_hit_ship();
            }
        }

        void fire_boss_missiles() {
            if (boss_missiles_disabled) {
                --boss_missiles_disabled;
            }
            else {
                for (int i = 0; i != num_boss_missiles; ++i) {
                    if (!sprites[first_boss_missile + i].is_enabled()) {
                        if (boss_sprite.is_facing_right()) {
                            sprites[first_boss_missile + i].set_relative(boss_sprite, 0.5f, -0.3f);
                            sprites[first_boss_missile + i].is_facing_right() = true;
                        }
                        else {
                            sprites[first_boss_missile + i].set_relative(boss_sprite, -0.5f, -0.3f);
                            sprites[first_boss_missile + i].is_facing_right() = false;
                        }
                        
                        sprites[first_boss_missile + i].is_enabled() = true;
                        boss_missiles_disabled = 30;

                        ALuint source = get_sound_source();
                        alSourcei(source, AL_BUFFER, whoosh);
                        alSourcePlay(source);
                        return;
                    }
                }
                return;

            }
        }

        void move_boss_missiles() {
            const float bomb_speed = 0.2f;
            for (int i = 0; i != num_boss_missiles; ++i) {
                sprite &bomb = sprites[first_boss_missile + i];
                if (bomb.is_enabled()) {
                    if (bomb.is_facing_right()) {
                        bomb.translate(bomb_speed, 0.0f);
                    }
                    else {
                        bomb.translate(-bomb_speed, 0.0f);
                    }
                    
                    if (bomb.collides_with(sprites[ship_sprite])) {
                        bomb.is_enabled() = false;
                        bomb.translate(0, -20);
                        bombs_disabled = 50;
                        on_hit_ship();
                        goto next_boss_bomb;
                    }
                    for (unsigned int j = 0; j < map_sprite_background2.size(); ++j) {
                        if (bomb.collides_with(map_sprite_background2[j])) {
                            bomb.is_enabled() = false;
                            bomb.translate(0, -20);
                        }
                    }
                }
            next_boss_bomb:;
            }
        }

        //lets sir Arthur regain armor
        void give_armor() {
            if (num_lives == 1 && timeWithoutArmor <= 45) {
                ++timeWithoutArmor;
            }
            if (num_lives == 1 && timeWithoutArmor > 45) {
                num_lives = 2;
                timeWithoutArmor = 0;
            }
        }


        void draw_text(texture_shader &shader, float x, float y, float scale, const char *text) {
            mat4t modelToWorld;
            modelToWorld.loadIdentity();
            modelToWorld.translate(x, y, 0);
            modelToWorld.scale(scale, scale, 1);
            mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);

            /*mat4t tmp;
            glLoadIdentity();
            glTranslatef(x, y, 0);
            glGetFloatv(GL_MODELVIEW_MATRIX, (float*)&tmp);
            glScalef(scale, scale, 1);
            glGetFloatv(GL_MODELVIEW_MATRIX, (float*)&tmp);*/

            enum { max_quads = 32 };
            bitmap_font::vertex vertices[max_quads * 4];
            uint32_t indices[max_quads * 6];
            aabb bb(vec3(0, 0, 0), vec3(256, 256, 0));

            unsigned num_quads = font.build_mesh(bb, vertices, indices, max_quads, text, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, font_texture);

            shader.render(modelToProjection, 0);

            glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, sizeof(bitmap_font::vertex), (void*)&vertices[0].x);
            glEnableVertexAttribArray(attribute_pos);
            glVertexAttribPointer(attribute_uv, 3, GL_FLOAT, GL_FALSE, sizeof(bitmap_font::vertex), (void*)&vertices[0].u);
            glEnableVertexAttribArray(attribute_uv);

            glDrawElements(GL_TRIANGLES, num_quads * 6, GL_UNSIGNED_INT, indices);
        }

    public:

        // this is called when we construct the class
        invaderers_app(int argc, char **argv) : app(argc, argv), font(512, 256, "assets/big.fnt") {
        }

        // this is called once OpenGL is initialized
        void app_init() {
            // set up the shader
            texture_shader_.init();
            federico_shader_.init();

            //read the Csv
            read_csv();
            read_csv2();

            //initializes background borders 
            setup_visual_map();
            setup_visual_map2();
            

            // set up the matrices with a camera 5 units from the origin
            cameraToWorld.loadIdentity();
            cameraToWorld.translate(0, 0, 3);

            //initializes sprites at normal texture
            font_texture = resource_dict::get_texture_handle(GL_RGBA, "assets/big_0.gif");

            GLuint ship = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/sir_arthur.gif");
            sprites[ship_sprite].init(ship, -2.5f, -2.5f, sir_arthur_width, sir_arthur_height);
            sprites[ship_sprite].is_facing_right() = true;

            GLuint GameOver = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/GameOver.gif");
            sprites[game_over_sprite].init(GameOver, 20, 0, 3, 1.5f);

            GLuint YouWin = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/lancelot.gif");
            sprites[you_win_sprite].init(YouWin, 20, 0, 6, 4.0f);          
           
            // use the missile texture
            GLuint missile = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/missile_horizontal.gif");
            for (int i = 0; i != num_missiles; ++i) {
                // create missiles off-screen
                sprites[first_missile_sprite + i].init(missile, 20, 0, 0.25f, 0.0625f);
                sprites[first_missile_sprite + i].is_enabled() = false;
            }

            for (int i = 0; i != num_boss_missiles; ++i) {
                sprites[first_boss_missile + i].init(missile, 20, 0, 0.25f, 0.0625f);
                sprites[first_boss_missile + i].is_enabled() = false;
            }

            // use the bomb texture
            GLuint bomb = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/bomb.gif");
            for (int i = 0; i != num_bombs; ++i) {
                // create bombs off-screen
                sprites[first_bomb_sprite + i].init(bomb, 20, 0, 0.0625f, 0.25f);
                sprites[first_bomb_sprite + i].is_enabled() = false;
            }

            bg_sprite.init(0, 0.0f, 0.0f, 6.0f, 6.0f);

            // sounds
            whoosh = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/whoosh.wav");
            bang = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/bang.wav");
            //boss_music = resource_dict::get_sound_handle(AL_FORMAT_MONO16, "assets/invaderers/boss_music.wav");
            cur_source = 0;
            alGenSources(num_sound_sources, sources);

            // sundry counters and game state.
            missiles_disabled = 0;
            bombs_disabled = 50;
            boss_missiles_disabled = 40;
            invader_velocity = 0.01f;
            live_invaderers = invaderers.size();
            num_lives = 2;
            game_over = false;
            score = 0;
        }

        // called every frame to move things
        void simulate() {
            if (game_over) {
                return;
            }

            move_ship();

            fire_missiles();

            move_missiles();

            if (!isBossEnabled) {
                fire_bombs();

                move_bombs();

                move_invaders(invader_velocity, 0);

                move_vampires(fabsf(invader_velocity));

                vampire_attack();

                for (unsigned int i = 0; i < map_sprite_background.size(); i++) {
                    sprite &border = map_sprite_background[i];
                    if (invaders_collide(border)) {
                        invader_velocity = -invader_velocity;
                        move_invaders(invader_velocity, -0.1f);
                    }
                }
            }
            else {
                move_boss();

                fire_boss_missiles();

                move_boss_missiles();

                give_armor();

            }
            
        }

    
        // this is called to draw the world
        void draw_world(int x, int y, int w, int h) {
            simulate();

            // set a viewport - includes whole window area
            glViewport(x, y, w, h);
            //added 
            int screen_width = 0;
            int screen_height = 0;
            get_viewport_size(screen_width, screen_height);

            // clear the background to black
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // don't allow Z buffer depth testing (closer objects are always drawn in front of far ones)
            glDisable(GL_DEPTH_TEST);

            // allow alpha blend (transparency when alpha channel is 0)
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            //draw background with our custom shader
            if (!isBossEnabled) {
                bg_sprite.render(federico_shader_, cameraToWorld, vec4(102.0 / 255.0, 47.0 / 255.0, 12.0 / 255.0, 1.0));
            }
            else {
                bg_sprite.render(federico_shader_, cameraToWorld, vec4(50.0 / 255.0, 14.0 / 255.0, 80.0 / 255.0, 1.0));
            }
            //end level boss fight
            if (boss_key.is_enabled());
            boss_key.render(texture_shader_, cameraToWorld);
            

            //draw the map sprites (border)
            if (!isBossEnabled) {
                for (unsigned int i = 0; i < map_sprite_background.size(); ++i) {
                    if (map_sprite_background[i].is_enabled()) {
                        map_sprite_background[i].render(texture_shader_, cameraToWorld);
                    }
                }
            }
            //draw border 2
            if (isBossEnabled) {
                for (unsigned int i = 0; i < map_sprite_background2.size(); ++i) {
                    if (map_sprite_background2[i].is_enabled()) {
                        map_sprite_background2[i].render(texture_shader_, cameraToWorld);
                    }
                }
            }




            //draws boss and vampires
            if (!isBossEnabled) {
                for (unsigned int i = 0; i < invaderers.size(); ++i) {
                    invaderers[i].render(texture_shader_, cameraToWorld);
                }

                for (unsigned int i = 0; i < vampires.size(); ++i) {
                    vampires[i].render(texture_shader_, cameraToWorld);
                    GLuint vamp = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/vampire.gif");
                    GLuint vamp_left = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/vampire_flipped.gif");
                    if (vampires[i].is_facing_right()) {
                        vampires[i].swap_sprite(vamp);
                    }
                    else {
                        vampires[i].swap_sprite(vamp_left);
                    }
                }
            }
            else {
                GLuint boss = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/boss.gif");
                GLuint boss_flipped = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/boss_flipped.gif");
                if (boss_sprite.is_facing_right()) {
                    boss_sprite.swap_sprite(boss_flipped);
                }
                else {
                    boss_sprite.swap_sprite(boss);
                }

                boss_sprite.render(texture_shader_, cameraToWorld);
            }
            

            // draw "ship" (sir arthur) and horizontal missiles
            for (int i = 0; i != num_sprites; ++i) {
                sprites[i].render(texture_shader_, cameraToWorld);

                if (i == ship_sprite) {
                    GLuint ship = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/sir_arthur.gif");
                    GLuint ship_left = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/sir_arthur_flipped.gif");
                    GLuint ship2 = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/sir_arthur_naked.gif");
                    GLuint ship2_left = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/sir_arthur_naked_flipped.gif");
                    
                    if (sprites[i].is_facing_right()) {
                        if (num_lives > 1)
                            sprites[i].swap_sprite(ship);
                        else
                            sprites[i].swap_sprite(ship2);
                    }
                    else {
                        if (num_lives > 1)
                            sprites[i].swap_sprite(ship_left);
                        else
                            sprites[i].swap_sprite(ship2_left);
                    }
                }
                else if (i >= first_boss_missile && i <= last_boss_missile) {
                    GLuint missile = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/missile_horizontal.gif");
                    GLuint missile_flipped = resource_dict::get_texture_handle(GL_RGBA, "assets/invaderers/missile_horizontal_flipped.gif");

                    if (sprites[i].is_facing_right()) {
                        sprites[i].swap_sprite(missile);
                    }
                    else {
                        sprites[i].swap_sprite(missile_flipped);
                    }
                }
            }

            char score_text[32];
            sprintf(score_text, "score: %d   lives: %d\n", score, num_lives);
            if (sprites[ship_sprite].get_position().x() < 0.0f) {
                draw_text(texture_shader_, -1.75f, 2, 1.0f / 256, score_text);
            }
            else {
                draw_text(texture_shader_, sprites[ship_sprite].get_position().x() - 1.75f, 2, 1.0f / 256, score_text);
            }

          
            // move the listener with the camera
            vec4 &cpos = cameraToWorld.w();
            alListener3f(AL_POSITION, cpos.x(), cpos.y(), cpos.z());
        }

    };
}
