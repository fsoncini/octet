namespace octet {
    namespace shaders {
        class federico_shader : public shader {
            GLuint modelToProjectionIndex_;
        public:
            void init() {
                const char vertex_shader[] = SHADER_STR(
                    attribute vec4 pos;
                uniform mat4 modelToProjection;

                void main() {
                    gl_Position = modelToProjection * pos;
                }
                );

                const char fragment_shader[] = SHADER_STR(
                    void main() {
                    gl_FragColor = vec4(102.0/255.0, 47.0/255.0, 12.0/255.0, 1.0);
                }
                );

                shader::init(vertex_shader, fragment_shader);

                modelToProjectionIndex_ = glGetUniformLocation(program(), "modelToProjection");
            }

            void render(const mat4t &modelToProjection) {
                shader::render();

                glUniformMatrix4fv(modelToProjectionIndex_, 1, GL_FALSE, modelToProjection.get());
            }
        };
    }
}
