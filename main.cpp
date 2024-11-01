#include "gl_wrappers.hpp"
#include "glfw_window.hpp"
#include "interactor.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <functional>

static std::function <void()> loop;
#ifdef __EMSCRIPTEN__
static void main_loop() {
    loop();
}
#endif

#ifdef __EMSCRIPTEN__
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 640;
#else
const int WINDOW_WIDTH = 1500;
const int WINDOW_HEIGHT = 1500;
#endif
GLFWwindow *s_window = init_window(WINDOW_WIDTH, WINDOW_HEIGHT);
// Quad s_main_render_target = Quad(TextureParams {});

struct SimParams {
    float dt = 0.0005;
    float x_min = 0.0;
    float x_max = 20.0; // 10.0 originally
    float y_min = 0.0;
    float y_max = 20.0; // 10.0 originally
    float spring_const1 = 6e2; // 3e2 - 6e2
    float spring_const2 = spring_const1/1.0;
    // float drag_const = 7e-5;
    float drag_const = 2e-4;
    float rest_width = 12.0; // 6.0 originally
    float rest_height = 6.0; // 3.0 originally
    float wall_force_const = 1e4;
    float wall_friction = 1.0;
    float g = 9.81;
    int w_count = 128; // 64 originally
    int h_count = 64; // 32 originally
    float m =4.0/(w_count*h_count); // 1.0/(w_count*h_count) originally
} s_sim_params;

/* WireFrame get_quad_wire_frame() {
    std::vector<int> elements = {
        0, 1, 2, 0, 2, 3
    };
    std::vector<float> vertices = {
        -1.0, -1.0, 0.0, -1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, -1.0, 0.0
    };
    return WireFrame {
        {{"position", {3, GL_FLOAT, false, 0, 0}},},
        vertices, elements, WireFrame::TRIANGLES
    };
}*/

WireFrame get_springs_wire_frame(
    int w_count, int h_count
) {
    Attributes attributes = {
        {"position", Attribute{
            2, GL_FLOAT, false, 
            0, 0}},
    };
    std::vector<float> vertices {};
    std::vector<int> elements {};
    int vertex_count = 0;
    for (int i = 0; i < h_count; i++) {
        for (int j = 0; j < w_count; j++) {
            float x_pos = ((float)j + 0.5)/(float)w_count;
            float y_pos = ((float)i + 0.5)/(float)h_count;
            vertices.push_back(x_pos);
            vertices.push_back(y_pos);
            int rightmost_column_vertex = w_count - 1;
            int topmost_row_vertex = h_count - 1;
            if (j < rightmost_column_vertex) {
                // Draw horizontal lines
                elements.push_back(vertex_count);
                elements.push_back(vertex_count + 1);
            } if (i < topmost_row_vertex) {
                // Draw vertical lines
                elements.push_back(vertex_count);
                elements.push_back(vertex_count + w_count); 
            }
            vertex_count++;
        }
    }
    return WireFrame(
        attributes, vertices, elements, WireFrame::LINES
    );
}

const TextureParams MAIN_WINDOW_PARAMS = {
    .format=GL_RGBA32F,
    .width=WINDOW_WIDTH, .height=WINDOW_HEIGHT,
    .generate_mipmap=true,
    .mag_filter=GL_LINEAR, .min_filter=GL_LINEAR, 
    .wrap_s=GL_REPEAT, .wrap_t=GL_REPEAT
};

const TextureParams SIMULATION_PARAMS = {
    .format=GL_RGBA32F, 
    .width=(uint32_t)s_sim_params.w_count,
    .height=(uint32_t)s_sim_params.h_count,
    .mag_filter=GL_NEAREST, .min_filter=GL_NEAREST,
    .wrap_s=GL_CLAMP_TO_EDGE,
    .wrap_t=GL_CLAMP_TO_EDGE,
};

struct MainRenderFrames {
    Quad main_view = Quad(MAIN_WINDOW_PARAMS);
    RenderTarget spring_view = RenderTarget(MAIN_WINDOW_PARAMS);
    Quad coords = Quad(SIMULATION_PARAMS);
    Quad points_to_coord = Quad(SIMULATION_PARAMS);
    Quad tmp = Quad(SIMULATION_PARAMS);
    Quad ext_force = Quad(SIMULATION_PARAMS);
    Quad vel_acc1 = Quad(SIMULATION_PARAMS);
    Quad vel_acc2 = Quad(SIMULATION_PARAMS);
    Quad vel_acc3 = Quad(SIMULATION_PARAMS); 
    Quad vel_acc4 = Quad(SIMULATION_PARAMS);
    Quad laplacian = Quad(SIMULATION_PARAMS);
} s_renders {};

struct GLSLPrograms {
    GLuint copy, add2, init, vel_acc, euler, rk4, view, point_to_coord, ext_force;
    GLSLPrograms() {
        this->copy = Quad::make_program_from_path("./shaders/util/copy.frag");
        this->add2 = Quad::make_program_from_path("./shaders/util/add2.frag");
        this->init
             = Quad::make_program_from_path("./shaders/2d/init-springs.frag");
        this->vel_acc = Quad::make_program_from_path(
            "./shaders/2d/vel-acc.frag"
        );
        this->rk4 = Quad::make_program_from_path(
            "./shaders/integration/rk4.frag");
        this->euler 
            = Quad::make_program_from_path(
                "./shaders/integration/forward-euler.frag");
        this->view = make_program_from_paths(
            "./shaders/2d/view-springs.vert", "./shaders/2d/view-springs.frag");
        this->point_to_coord = Quad::make_program_from_path(
            "./shaders/2d/point-to-coord.frag"
        );
        this->ext_force = Quad::make_program_from_path(
            "./shaders/2d/ext-force.frag"
        );
    }
} GLSL_PROGRAMS = {};

void compute_velocity_acceleration(Quad &dst, const Quad &src) {
    dst.draw(
        GLSL_PROGRAMS.vel_acc,
        {
            {"springConst1", {s_sim_params.spring_const1}},
            {"springConst2", {s_sim_params.spring_const2}},
            {"dragConst", {s_sim_params.drag_const}},
            {"restWidth", {s_sim_params.rest_width}},
            {"restHeight", {s_sim_params.rest_height}},
            {"xMin", {s_sim_params.x_min}},
            {"xMax", {s_sim_params.x_max}},
            {"yMin", {s_sim_params.y_min}},
            {"yMax", {s_sim_params.y_max}},
            {"wallForceConst", {s_sim_params.wall_force_const}},
            {"wallFriction", {s_sim_params.wall_friction}},
            {"wCount", {s_sim_params.w_count}},
            {"hCount", {s_sim_params.h_count}},
            {"m", {s_sim_params.m}},
            {"g", {s_sim_params.g}},
            {"coordTex", {&src}},
            {"extForcesTex", {&s_renders.ext_force}},
        }
    );
}

void add2(Quad &dst, const Quad &a, const Quad &b) {
    dst.draw(
        GLSL_PROGRAMS.add2,
        {
            {"tex1", {&a}}, {"tex2", {&b}},
        }
    );
}

void forward_euler(Quad &q1, const Quad &q0, const Quad &q0_dot, float dt) {
    q1.draw(
        GLSL_PROGRAMS.euler,
        {
            {"dt", {dt}},
            {"coordTex", {&q0}}, 
            {"velAccTex", {&q0_dot}}
        }
    );
}

void time_step() {
    float dt = s_sim_params.dt;
    // q1
    compute_velocity_acceleration(s_renders.vel_acc1, s_renders.coords);
    // q2
    forward_euler(
        s_renders.tmp, s_renders.coords, s_renders.vel_acc1, dt/2.0);
    compute_velocity_acceleration(s_renders.vel_acc2, s_renders.tmp);
    // q3
    forward_euler(
        s_renders.tmp, s_renders.coords, s_renders.vel_acc2, dt/2.0);
    compute_velocity_acceleration(s_renders.vel_acc3, s_renders.tmp);
    // q4
    forward_euler(
        s_renders.tmp, s_renders.coords, s_renders.vel_acc3, dt);
    compute_velocity_acceleration(s_renders.vel_acc4, s_renders.tmp);
    s_renders.tmp.draw(
        GLSL_PROGRAMS.rk4,
        {
            {"coordTex", {&s_renders.coords}},
            {"velAccTex1", {&s_renders.vel_acc1}},
            {"velAccTex2", {&s_renders.vel_acc2}},
            {"velAccTex3", {&s_renders.vel_acc3}},
            {"velAccTex4", {&s_renders.vel_acc4}},
            {"dt", {s_sim_params.dt}}
        }
    );
    s_renders.coords.draw(
        GLSL_PROGRAMS.copy, {
            {"tex", {&s_renders.tmp}}
        }
    );
}

void init() {
    float domain_width = s_sim_params.x_max - s_sim_params.x_min;
    float domain_height = s_sim_params.y_max - s_sim_params.y_min;
    s_renders.coords.draw(
        GLSL_PROGRAMS.init,
        {
            {"width", {s_sim_params.rest_width}},
            {"height", {s_sim_params.rest_height}}, 
            /* {"position", {
                Vec2{.ind={0.0, 0.0}}}
            },*/
            {"position", {
                Vec2{.ind={
                    s_sim_params.x_min + domain_width*0.5F,
                    s_sim_params.y_min + domain_height*0.5F}}}
            },
            {"velocity", {Vec2{.x=10.0, .y=10.0}}},
            {"angle", {3.14159F/2.0F}},
            {"omega", {-10.5F}}
        }
    );
}

int main() {
    init();
    /* {
        auto tex = MAIN_WINDOW_PARAMS;
        tex.width = 1024;
        tex.height = 1024;
        s_renders.main_view.reset(tex);
    }*/
    auto wire_frame = get_springs_wire_frame(
        s_sim_params.w_count, s_sim_params.h_count);

    Interactor interactor(s_window);
    Vec2 mouse_pos = Vec2{
        .ind={-10.0F + s_sim_params.x_min,
              -10.0F + s_sim_params.y_min}};
    int n_clicks = 0;
    
    loop = [&] {
        for (int i = 0; i < 15; i++)
            time_step();
        // std::cout << interact_pos.x << ", " << interact_pos.y << std::endl;
        s_renders.spring_view.draw(
            GLSL_PROGRAMS.view, 
            {
                {"coordTex", {&s_renders.coords}},
                {"xMin", {s_sim_params.x_min}},
                {"xMax", {s_sim_params.x_max}},
                {"yMin", {s_sim_params.y_min}},
                {"yMax", {s_sim_params.y_max}},
                {"coordTexF", {&s_renders.coords}},
                {"extForcesTex", {&s_renders.ext_force}},
                // {"color", {Vec4 {.ind={1.0, 1.0, 1.0, 1.0}}}}
            },
            wire_frame);
        s_renders.main_view.draw(
            GLSL_PROGRAMS.copy,
            {{"tex", {&s_renders.spring_view}}}
        );
        s_renders.spring_view.clear();
        glfwPollEvents();
        {
            interactor.click_update(s_window);
            if (interactor.left_pressed()) {
                mouse_pos = interactor.get_mouse_position();
                Vec2 interact_pos = {
                    .ind={
                        s_sim_params.x_min
                        + mouse_pos[0]
                            *(s_sim_params.x_max - s_sim_params.x_min),
                        s_sim_params.y_min
                        + mouse_pos[1]
                            *(s_sim_params.y_max - s_sim_params.y_min)
                    }
                };
                float sigma = 0.05*(s_sim_params.x_max - s_sim_params.x_min);
                if (n_clicks++ == 0) {
                    s_renders.points_to_coord.draw(
                        GLSL_PROGRAMS.point_to_coord,
                        {
                            {"r", {interact_pos}},
                            {"coordTex", {&s_renders.coords}},
                        }
                    );
                }
                s_renders.ext_force.draw(
                    GLSL_PROGRAMS.ext_force,
                    {
                        {"p1", {interact_pos}},
                        {"sigma", {sigma}},
                        {"coordTex", {&s_renders.coords}},
                        {"pointToCoordTex", {&s_renders.points_to_coord}},
                    }
                );
            } else {
                n_clicks = 0;
            }
            if (interactor.left_released()) {
                s_renders.ext_force.clear();
            }
            if (glfwGetKey(s_window, GLFW_KEY_SPACE) == GLFW_PRESS)
                init();
        };
        glfwSwapBuffers(s_window);
    };

    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, true);
    #else
    while (!glfwWindowShouldClose(s_window))
        loop();
    #endif

    glfwDestroyWindow(s_window);
    return 1;
}