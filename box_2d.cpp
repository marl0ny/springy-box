#include "box_2d.hpp"

namespace sim_2d {

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

MainRenderFrames::MainRenderFrames(
    SimParams sim_params, int window_width, int window_height) :
    main_view_tex_params(
        {
            .format=GL_RGBA16F, 
            .width=(uint32_t)window_width,
            .height=(uint32_t)window_height,
            .mag_filter=GL_LINEAR, .min_filter=GL_LINEAR,
            .wrap_s=GL_CLAMP_TO_EDGE,
            .wrap_t=GL_CLAMP_TO_EDGE,
        }
    ),
    sim_tex_params(
        {
            .format=GL_RGBA32F, 
            .width=(uint32_t)sim_params.wCount.i32,
            .height=(uint32_t)sim_params.hCount.i32,
            .mag_filter=GL_NEAREST, .min_filter=GL_NEAREST,
            .wrap_s=GL_CLAMP_TO_EDGE,
            .wrap_t=GL_CLAMP_TO_EDGE,
        }
    ),
    main_view(Quad(main_view_tex_params)),
    spring_view(RenderTarget(main_view_tex_params)),
    coords(Quad(sim_tex_params)),
    points_to_coord(Quad(sim_tex_params)),
    tmp(Quad(sim_tex_params)),
    ext_force(Quad(sim_tex_params)),
    vel_acc1(Quad(sim_tex_params)),
    vel_acc2(Quad(sim_tex_params)),
    vel_acc3(Quad(sim_tex_params)),
    vel_acc4(Quad(sim_tex_params)),
    laplacian(Quad(sim_tex_params)) {}

GLSLPrograms::GLSLPrograms() {
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

Simulation::Simulation(
    int window_width, int window_height, SimParams sim_params
): m_programs(), m_frames(sim_params, window_width, window_height),
    m_wire_frame(get_springs_wire_frame(
        sim_params.wCount.i32, sim_params.hCount.i32)) {
    this->init_config(sim_params);
}

void Simulation::compute_velocity_acceleration(
    Quad &dst, const Quad &src, const Quad &ext_forces,
    SimParams sim_params) {
    dst.draw(
        m_programs.vel_acc,
        {
            {"springConst1", sim_params.springConst1},
            {"springConst2", sim_params.springConst2},
            {"dragConst", sim_params.dragConst},
            {"restWidth", sim_params.restWidth},
            {"restHeight", sim_params.restHeight},
            {"xMin", sim_params.xMin},
            {"xMax", sim_params.xMax},
            {"yMin", sim_params.yMin},
            {"yMax", sim_params.yMax},
            {"wallForceConst", sim_params.wallForceConst},
            {"wallFriction", sim_params.wallFriction},
            {"wCount", sim_params.wCount},
            {"hCount", sim_params.hCount},
            {"m", sim_params.m},
            {"g", sim_params.g},
            {"coordTex", {&src}},
            {"extForcesTex", {&m_frames.ext_force}},
        }
    );
}

void Simulation::add2(Quad &dst, Quad &a, const Quad &b) {
    dst.draw(
        m_programs.add2,
        {
            {"tex1", {&a}}, {"tex2", {&b}},
        }
    );
}

void Simulation::forward_euler(
    Quad &q1, const Quad &q0, const Quad &q0_dot, float dt) {
    q1.draw(
    m_programs.euler,
    {
        {"dt", {dt}},
        {"qTex", {&q0}}, 
        {"qDotTex", {&q0_dot}}
    }
    );
}

void Simulation::init_config(SimParams sim_params) {
    // float domain_width = sim_params.x_max - sim_params.x_min;
    // float domain_height = sim_params.y_max - sim_params.y_min;
    m_frames.coords.draw(
        m_programs.init,
        {
            {"width", sim_params.restWidth},
            {"height", sim_params.restHeight}, 
            {"position", sim_params.initialPosition},
            {"velocity", sim_params.initialVelocity},
            {"angle", sim_params.initialAngle},
            {"omega", sim_params.initialAngularVelocity}
        }
    );
}

void Simulation::time_step(SimParams sim_params) {
    float dt = sim_params.dt.f32;
    // q1
    compute_velocity_acceleration(
        m_frames.vel_acc1, m_frames.coords, m_frames.ext_force, sim_params
    );
    // q2
    forward_euler(
        m_frames.tmp, m_frames.coords, m_frames.vel_acc1, dt/2.0);
    compute_velocity_acceleration(
        m_frames.vel_acc2, m_frames.tmp, m_frames.ext_force, sim_params
    );
    // q3
    forward_euler(
        m_frames.tmp, m_frames.coords, m_frames.vel_acc2, dt/2.0);
    compute_velocity_acceleration(
        m_frames.vel_acc3, m_frames.tmp, m_frames.ext_force, sim_params
    );
    // q4
    forward_euler(
        m_frames.tmp, m_frames.coords, m_frames.vel_acc3, dt);
    compute_velocity_acceleration(
        m_frames.vel_acc4, m_frames.tmp, m_frames.ext_force, sim_params
    );
    m_frames.tmp.draw(
        m_programs.rk4,
        {
            {"qTex", {&m_frames.coords}},
            {"qDotTex1", {&m_frames.vel_acc1}},
            {"qDotTex2", {&m_frames.vel_acc2}},
            {"qDotTex3", {&m_frames.vel_acc3}},
            {"qDotTex4", {&m_frames.vel_acc4}},
            {"dt", sim_params.dt}
        }
    );
    m_frames.coords.draw(
        m_programs.copy, {
            {"tex", {&m_frames.tmp}}
        }
    );
}

void Simulation::render_view(SimParams sim_params) {
    m_frames.spring_view.draw(
        m_programs.view, 
        {
            {"coordTex", {&m_frames.coords}},
            {"xMin", sim_params.xMin},
            {"xMax", sim_params.xMax},
            {"yMin", sim_params.yMin},
            {"yMax", sim_params.yMax},
            {"coordTexF", {&m_frames.coords}},
            {"extForcesTex", {&m_frames.ext_force}},
            // {"color", {Vec4 {.ind={1.0, 1.0, 1.0, 1.0}}}}
        },
        m_wire_frame);
    m_frames.main_view.draw(
        m_programs.copy,
        {{"tex", {&m_frames.spring_view}}}
    );
    m_frames.spring_view.clear();
}

void Simulation::set_hold_position(Vec2 interact_pos) {
    m_frames.points_to_coord.draw(
        m_programs.point_to_coord,
        {
            {"r", {interact_pos}},
            {"coordTex", {&m_frames.coords}},
        }
    );
}

void Simulation::add_ext_force(Vec2 interact_pos, float size) {
    m_frames.ext_force.draw(
        m_programs.ext_force,
        {
            {"p1", {interact_pos}},
            {"sigma", {size}},
            {"coordTex", {&m_frames.coords}},
            {"pointToCoordTex", {&m_frames.points_to_coord}},
        }
    );
}

void Simulation::clear_ext_force() {
    m_frames.ext_force.clear();
}

}