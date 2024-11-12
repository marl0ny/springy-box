#include "gl_wrappers.hpp"
#include "glfw_window.hpp"
#include "parameters3d.hpp"

namespace sim_3d {


struct MainRenderFrames {
    TextureParams main_view_tex_params;
    TextureParams sim_tex_params;
    Quad main_view;
    RenderTarget spring_view;
    Quad positions;
    Quad velocities;
    Quad tmp[2];
    Quad rk4_velocities[4];
    Quad rk4_accelerations[4];
    Quad ext_forces;
    /* Quad points_to_coord;
    Quad tmp;
    Quad ext_force;
    Quad laplacian;*/
    MainRenderFrames(
        SimParams sim_params, int window_width, int window_height);
};

struct GLSLPrograms {
    GLuint copy, add2, init, acceleration, view, view_static;
    GLuint rk4, forward_euler;
    GLSLPrograms();
};

class Simulation {
    GLSLPrograms m_programs;
    MainRenderFrames m_frames;
    WireFrame m_wire_frame;
    WireFrame m_floor_wire_frame;
    WireFrame m_box_bounds_wire_frame;
    void add2(Quad &dst, Quad &a, const Quad &b);
    void forward_euler(
        Quad &q1, const Quad &q0, const Quad &q0_dot, float dt);
    void compute_acceleration(
        Quad &accelerations,
        const Quad &positions, const Quad &velocities,
        const Quad &ext_forces, SimParams sim_params);
    void compute_velocity_acceleration(
        Quad &velocities_f, Quad &accelerations,
        const Quad &positions, const Quad &velocities_i,
        const Quad &ext_forces, 
        SimParams sim_params);
    void compute_rk4_intermediaries(
        int index, SimParams sim_params, float dt);
    public:
    Simulation(int window_width, int window_height, SimParams sim_params);
    void init_config(SimParams sim_params);
    void time_step(SimParams sim_params);
    void render_view(SimParams sim_params, Quaternion rotation, float scale);
    void set_hold_position(Vec2 interact_pos);
    void add_ext_force(Vec2 interact_pos, float size);
    void clear_ext_force();

};

}