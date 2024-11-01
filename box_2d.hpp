#include "gl_wrappers.hpp"
#include "glfw_window.hpp"

namespace sim_2d {

struct SimParams {
    int steps_frame = 15;
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
    float m =1.0/2048.0; // 1.0/(w_count*h_count) originally
    Vec2 initial_position = Vec2{
        .ind={x_min + (x_max - x_min)*0.5F,
              y_min + (y_max - y_min)*0.5F}};
    Vec2 initial_velocity = Vec2{.x=10.0, .y=10.0};
    float initial_angle = 3.14159F/2.0F;
    float initial_angular_velocity = -10.0F;
    
};

struct MainRenderFrames {
    TextureParams main_view_tex_params;
    TextureParams sim_tex_params;
    Quad main_view;
    RenderTarget spring_view;
    Quad coords;
    Quad points_to_coord;
    Quad tmp;
    Quad ext_force;
    Quad vel_acc1;
    Quad vel_acc2;
    Quad vel_acc3; 
    Quad vel_acc4;
    Quad laplacian;
    MainRenderFrames(
        SimParams sim_params, int window_width, int window_height);
};

struct GLSLPrograms {
    GLuint copy, add2, init, vel_acc, euler, rk4, view, point_to_coord, ext_force;
    GLSLPrograms();
};

class Simulation {
    GLSLPrograms m_programs;
    MainRenderFrames m_frames;
    WireFrame m_wire_frame;
    void compute_velocity_acceleration(
        Quad &dst, const Quad &src, const Quad &ext_forces,
        SimParams sim_params);
    void add2(Quad &dst, Quad &a, const Quad &b);
    void forward_euler(
        Quad &q1, const Quad &q0, const Quad &q0_dot, float dt);
    public:
    Simulation(int window_width, int window_height, SimParams sim_params);
    void init_config(SimParams sim_params);
    void time_step(SimParams sim_params);
    void render_view(SimParams sim_params);
    void set_hold_position(Vec2 interact_pos);
    void add_ext_force(Vec2 interact_pos, float size);
    void clear_ext_force();

};

}