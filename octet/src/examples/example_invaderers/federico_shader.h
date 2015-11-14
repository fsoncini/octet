namespace octet {
    namespace shaders {
        class federico_shader : public shader {
            GLuint modelToProjectionIndex_;
            GLuint colorIndex_;
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
                    uniform vec4 color;
                    void main() {
                    gl_FragColor = color;
                }
                );

                shader::init(vertex_shader, fragment_shader);

                modelToProjectionIndex_ = glGetUniformLocation(program(), "modelToProjection");
                colorIndex_ = glGetUniformLocation(program(), "color");
            }

            void render(const mat4t &modelToProjection, const vec4 &color) {
                shader::render();

                glUniform4fv(colorIndex_, 1, color.get());
                glUniformMatrix4fv(modelToProjectionIndex_, 1, GL_FALSE, modelToProjection.get());
            }
        };
    }
}
