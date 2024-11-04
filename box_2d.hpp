#include "gl_wrappers.hpp"
#include "glfw_window.hpp"
#include "uniform_parameters.hpp"

namespace sim_2d {

typedef UniformParams SimParams;

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