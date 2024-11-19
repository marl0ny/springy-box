#include "box_3d.hpp"

namespace sim_3d {


WireFrame get_springs_wire_frame(
    IVec3 dimensions, IVec2 dimensions2d
) {
    Attributes attributes = {
        {"position", Attribute{
            2, GL_FLOAT, false, 
            0, 0}},
    };
    std::vector<float> vertices {};
    std::vector<int> elements {};
    int vertex_count = 0;
    for (int i = 0; i < dimensions[2]; i++) {
        for (int j = 0; j < dimensions[1]; j++) {
            for (int k = 0; k < dimensions[0]; k++) {
                Vec3 r_3d = {.ind={
                    (float(k) + 0.5F)/float(dimensions[0]),
                    (float(j) + 0.5F)/float(dimensions[1]),
                    (float(i) + 0.5F)/float(dimensions[2]),
                }};
                Vec2 r_2d = get_2d_from_3d_texture_coordinates(
                    r_3d, dimensions2d, dimensions);
                vertices.push_back(r_2d[0]);
                vertices.push_back(r_2d[1]);
                if (k < dimensions[0] - 1) {
                    elements.push_back(vertex_count);
                    elements.push_back(vertex_count + 1);
                }
                if (j < dimensions[1] - 1) {
                    elements.push_back(vertex_count);
                    elements.push_back(vertex_count 
                        + dimensions[0]);
                }
                if (i < dimensions[2] - 1) {
                    elements.push_back(vertex_count);
                    elements.push_back(vertex_count 
                        + dimensions[0]*dimensions[1]);
                }
                vertex_count++;
            }
        }
    }
    return WireFrame(
        attributes, vertices, elements, WireFrame::LINES
    );
}

WireFrame get_floor_wire_frame(Vec3 xyzMin, Vec3 xyzMax) {
     Attributes attributes = {
        {"position", Attribute{
            3, GL_FLOAT, false, 
            0, 0}},
    };
    std::vector<float> vertices {
        1.0F*xyzMin[0], xyzMin[1], 1.0F*xyzMin[2],
        1.0F*xyzMin[0], xyzMin[1], 1.0F*xyzMax[2],
        1.0F*xyzMax[0], xyzMin[1], 1.0F*xyzMax[2],
        1.0F*xyzMax[0], xyzMin[1], 1.0F*xyzMin[2]
    };
    std::vector<int> elements {
        0, 1, 3, 3, 2, 1
    };
    return WireFrame(attributes, vertices, elements, WireFrame::TRIANGLES);

}

WireFrame get_box_bounds_wire_frame(Vec3 xyzMin, Vec3 xyzMax) {
     Attributes attributes = {
        {"position", Attribute{
            3, GL_FLOAT, false, 
            0, 0}},
    };
    std::vector<float> vertices (3*2*2*2);
    int n = 0;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                vertices[n++] = (k == 0)? xyzMin[0]: xyzMax[0];
                vertices[n++] = (j == 0)? xyzMin[1]: xyzMax[1];
                vertices[n++] = (i == 0)? xyzMin[2]: xyzMax[2];
            }
        }
    }
    std::vector<int> elements {
        0, 1, 2, 3, 0, 2, 1, 3,
        4, 5, 6, 7, 4, 6, 5, 7,
        0, 4, 1, 5, 2, 6, 3, 7
    };
    return WireFrame(attributes, vertices, elements, WireFrame::LINES);

}

RenderFrames::RenderFrames(
    SimParams sim_params, int window_width, int window_height):
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
            .width=(uint32_t)get_2d_from_3d_dimensions(
                sim_params.springCountDimensions)[0],
            .height=(uint32_t)get_2d_from_3d_dimensions(
                sim_params.springCountDimensions)[1],
            .mag_filter=GL_NEAREST, .min_filter=GL_NEAREST,
            .wrap_s=GL_CLAMP_TO_EDGE,
            .wrap_t=GL_CLAMP_TO_EDGE,
        }
    ),
    spring_view(RenderTarget(main_view_tex_params)),
    positions(Quad(sim_tex_params)),
    velocities(Quad(sim_tex_params)),
    tmp {Quad(sim_tex_params), Quad(sim_tex_params)},
    rk4_velocities {
        Quad(sim_tex_params), Quad(sim_tex_params),
        Quad(sim_tex_params), Quad(sim_tex_params)
    },
    rk4_accelerations {
        Quad(sim_tex_params), Quad(sim_tex_params),
        Quad(sim_tex_params), Quad(sim_tex_params)
    },
    line_to_positions(Quad(sim_tex_params)),
    ext_forces(Quad(sim_tex_params))
    {}

GLSLPrograms::GLSLPrograms() {
    this->copy = Quad::make_program_from_path("./shaders/util/copy.frag");
    this->add2 = Quad::make_program_from_path("./shaders/util/add2.frag");
    this->uniform_color 
        = Quad::make_program_from_path("./shaders/util/uniform-color.frag");
    this->init
            = Quad::make_program_from_path(
                "./shaders/3d/init-springs.frag");
    this->acceleration
        = Quad::make_program_from_path(
            "./shaders/3d/acceleration.frag"
        );
    this->view = make_program_from_paths(
        "./shaders/3d/view-springs.vert",
        "./shaders/3d/view-springs.frag"
    );
    this->view_static = make_program_from_paths(
        "./shaders/3d/background.vert",
        "./shaders/util/uniform-color.frag"
    );
    this->line_to_positions = Quad::make_program_from_path(
        "./shaders/3d/line-to-position.frag"
    );
    this->ext_force = Quad::make_program_from_path(
        "./shaders/3d/ext-force.frag"
    );
    this->forward_euler = Quad::make_program_from_path(
        "./shaders/integration/forward-euler.frag"
    );
    this->rk4 = Quad::make_program_from_path(
        "./shaders/integration/rk4.frag"
    );
}

void Simulation::init_config(SimParams sim_params) {
    int init_positions = 0;
    int init_velocities = 1;
    // m_frames.line_to_positions.draw(
    //     m_programs.uniform_color, {{"color", Vec4{.ind={1.0,}}}}
    // );
    m_frames.positions.draw(
        m_programs.init,
        {
            {"dimensions3D", {sim_params.restDimensions}},
            {"position", {sim_params.initialPosition}},
            {"velocity", {sim_params.initialVelocity}},
            {"axis", {sim_params.initialAxis}},
            {"rotationAxis", {sim_params.initialRotationAxis}},
            {"angularSpeed", {sim_params.initialAngularVelocity}},
            {"mode", init_positions},
            {"texelDimensions3D", {sim_params.springCountDimensions}},
            {"texelDimensions2D",
                {get_2d_from_3d_dimensions(
                    sim_params.springCountDimensions)}}
        }
    );
    m_frames.velocities.draw(
        m_programs.init,
        {
            {"dimensions3D", {sim_params.restDimensions}},
            {"position", {sim_params.initialPosition}},
            {"velocity", {sim_params.initialVelocity}},
            {"axis", {sim_params.initialAxis}},
            {"rotationAxis", {sim_params.initialRotationAxis}},
            {"angularSpeed", {sim_params.initialAngularVelocity}},
            {"mode", init_velocities},
            {"texelDimensions3D", {sim_params.springCountDimensions}},
            {"texelDimensions2D",
                {get_2d_from_3d_dimensions(
                    sim_params.springCountDimensions)}}
        }
    );
};

Simulation::Simulation(
        int window_width, int window_height, SimParams sim_params): 
    m_programs(), m_frames(sim_params, window_width, window_height),
    m_wire_frame(get_springs_wire_frame(
        sim_params.springCountDimensions,
    get_2d_from_3d_dimensions(sim_params.springCountDimensions))),
    m_floor_wire_frame(
        get_floor_wire_frame(sim_params.xyzMin, sim_params.xyzMax)
    ),
    m_box_bounds_wire_frame(
        get_box_bounds_wire_frame(sim_params.xyzMin, sim_params.xyzMax)
    ) {
    this->init_config(sim_params);
}

void Simulation::forward_euler(
    Quad &q1, const Quad &q0, const Quad &q0_dot, float dt) {
    q1.draw(
    m_programs.forward_euler,
    {
            {"dt", {dt}},
            {"qTex", {&q0}}, 
            {"qDotTex", {&q0_dot}}
        }
    );
}

const RenderTarget &Simulation::render_view(
    SimParams sim_params, Quaternion rotation, float scale) {
    glEnable(GL_DEPTH_TEST);
    m_frames.spring_view.clear();
    m_frames.spring_view.draw(
        m_programs.view_static,
        {
            {"rMin", {sim_params.xyzMin}},
            {"rMax", {sim_params.xyzMax}},
            {"scale", {std::min(std::max(0.1F, scale/12.5F), 2.0F)}},
            {"rotation", {rotation}},
            // {"rotation", {Vec4 {.ind={0.0, 0.0, 0.0, 1.0}}}},
            {"screenDimensions", {
                IVec2{.ind={
                    (int)m_frames.main_view_tex_params.width,
                    (int)m_frames.main_view_tex_params.height}
                }
            }},
            {"color", {Vec4 {.ind={0.2, 0.2, 0.2, 1.0}}}}
        },
        m_floor_wire_frame
    );
    m_frames.spring_view.draw(
        m_programs.view_static,
        {
            {"rMin", {sim_params.xyzMin}},
            {"rMax", {sim_params.xyzMax}},
            {"scale", {std::min(std::max(0.1F, scale/12.5F), 2.0F)}},
            {"rotation", {rotation}},
            // {"rotation", {Vec4 {.ind={0.0, 0.0, 0.0, 1.0}}}},
            {"screenDimensions", {
                IVec2{.ind={
                    (int)m_frames.main_view_tex_params.width,
                    (int)m_frames.main_view_tex_params.height}
                }
            }},
            {"color", {Vec4 {.ind={1.0, 1.0, 1.0, 1.0}}}}
        },
        m_box_bounds_wire_frame
    );
    m_frames.spring_view.draw(
        m_programs.view, 
        {
            {"positionsTex", {&m_frames.positions}}, 
            {"rMin", {sim_params.xyzMin}},
            {"rMax", {sim_params.xyzMax}},
            {"scale", {std::min(std::max(0.1F, scale/12.5F), 2.0F)}},
            {"rotation", {rotation}},
            // {"rotation", {Vec4 {.ind={0.0, 0.0, 0.0, 1.0}}}},
            {"screenDimensions", {
                IVec2{.ind={
                    (int)m_frames.main_view_tex_params.width,
                    (int)m_frames.main_view_tex_params.height}
                },}},
            {"extForcesTex", {&m_frames.ext_forces}},
            {"lineToPositionsTex", {&m_frames.line_to_positions}}

        }, 
        m_wire_frame);
    glDisable(GL_DEPTH_TEST);
    return m_frames.spring_view;
}

void Simulation::compute_acceleration(
    Quad &accelerations,
    const Quad &positions, const Quad &velocities,
    const Quad &ext_forces, 
    SimParams sim_params) {
    accelerations.draw(
        m_programs.acceleration,
        {
            {"springConst1", {sim_params.springConst1}},
            {"springConst2", {sim_params.springConst1}},
            {"dragConst", {sim_params.dragConst}},
            {"xyzMin", {sim_params.xyzMin}},
            {"xyzMax", {sim_params.xyzMax}},
            {"wallForceConst", {sim_params.wallForceConst}},
            {"wallFriction", {sim_params.wallFriction}},
            {"restDimensions", {sim_params.restDimensions}},
            {"springCountDimensions", {sim_params.springCountDimensions}},
            {"m", {sim_params.m}},
            {"g", {sim_params.g}},
            {"positionsTex", {&positions}},
            {"velocitiesTex", {&velocities}},
            {"extForcesTex", {&ext_forces}},
            {"texelDimensions2D", 
                {get_2d_from_3d_dimensions(
                    sim_params.springCountDimensions)}},
            {"texelDimensions3D", {
                sim_params.springCountDimensions}}

        }
    );
}

void Simulation::compute_velocity_acceleration(
    Quad &velocities_f, Quad &accelerations,
    const Quad &positions, const Quad &velocities_i,
    const Quad &ext_forces, 
    SimParams sim_params
) {
    compute_acceleration(
        accelerations, positions, velocities_i, ext_forces, sim_params);
    velocities_f.draw(
        m_programs.copy,
        {
            {"tex", {&velocities_i}}
        }
    );

}

void Simulation::compute_rk4_intermediaries(
    int index, SimParams sim_params, float dt
) {
    forward_euler(
        m_frames.tmp[0], m_frames.positions, 
        m_frames.rk4_velocities[index-1], dt
    );
    forward_euler(
        m_frames.tmp[1], m_frames.velocities, 
        m_frames.rk4_accelerations[index-1], dt
    );
    compute_velocity_acceleration(
        m_frames.rk4_velocities[index],
        m_frames.rk4_accelerations[index],
        m_frames.tmp[0],
        m_frames.tmp[1],
        m_frames.ext_forces, sim_params
    );

}

void Simulation::time_step(SimParams sim_params) {
    float dt = sim_params.dt;
    compute_velocity_acceleration(
        m_frames.rk4_velocities[0], 
        m_frames.rk4_accelerations[0],
        m_frames.positions, 
        m_frames.velocities,
        m_frames.ext_forces, sim_params);
    this->compute_rk4_intermediaries(1, sim_params, dt/2.0);
    this->compute_rk4_intermediaries(2, sim_params, dt/2.0);
    this->compute_rk4_intermediaries(3, sim_params, dt);
    m_frames.tmp[0].draw(
        m_programs.copy, {{"tex", {&m_frames.velocities}}}
    );
    m_frames.velocities.draw(
        m_programs.rk4,
        {
            {"qTex", {&m_frames.tmp[0]}},
            {"qDotTex1", {&m_frames.rk4_accelerations[0]}},
            {"qDotTex2", {&m_frames.rk4_accelerations[1]}},
            {"qDotTex3", {&m_frames.rk4_accelerations[2]}},
            {"qDotTex4", {&m_frames.rk4_accelerations[3]}},
            {"dt", {dt}},
        }
    );
    m_frames.tmp[1].draw(
        m_programs.copy, {{"tex", {&m_frames.positions}}}
    );
    m_frames.positions.draw(
        m_programs.rk4,
        {
            {"qTex", {&m_frames.tmp[1]}},
            {"qDotTex1", {&m_frames.rk4_velocities[0]}},
            {"qDotTex2", {&m_frames.rk4_velocities[1]}},
            {"qDotTex3", {&m_frames.rk4_velocities[2]}},
            {"qDotTex4", {&m_frames.rk4_velocities[3]}},
            {"dt", {dt}},
        }
    );
}

void Simulation::set_hold_position(Vec2 interact_pos, 
    SimParams sim_params, Quaternion rotation, float scale) {
    m_frames.line_to_positions.draw(
        m_programs.line_to_positions,
        {
            {"screenPosition", {interact_pos}},
            {"positionsTex", {&m_frames.positions}},
            {"rMin", {sim_params.xyzMin}},
            {"rMax", {sim_params.xyzMax}},
            {"scale", {std::min(std::max(0.1F, scale/12.5F), 2.0F)}},
            {"rotation", {rotation}},
            {"screenDimensions", 
                {m_frames.spring_view.texture_dimensions()}},
        }
    );
}

void Simulation::add_ext_force(Vec2 interact_pos, float size,
    SimParams sim_params, Quaternion rotation, float scale) {
    m_frames.ext_forces.draw(
        m_programs.ext_force,
        {
            {"positionsTex", {&m_frames.positions}},
            {"lineToPositionsTex", {&m_frames.line_to_positions}},
            {"sigma", {size}},
            {"screenPosition", {interact_pos}},
            {"rMin", {sim_params.xyzMin}},
            {"rMax", {sim_params.xyzMax}},
            {"scale", {std::min(std::max(0.1F, scale/12.5F), 2.0F)}},
            {"rotation", {rotation}},
            {"screenDimensions", {m_frames.spring_view.texture_dimensions()}}
        }
    );
}

void Simulation::clear_ext_force() {
    m_frames.ext_forces.clear();
}

IVec2 Simulation::get_dimensions() {
    return IVec2{.ind={
        (int)m_frames.sim_tex_params.width, 
        (int)m_frames.sim_tex_params.height}
    };
}

void Simulation::set_dimensions(IVec3 d_3d) {
    fprintf(stdout, "%d, %d, \n", 
        m_frames.positions.width(), m_frames.positions.height());
    IVec2 d = get_2d_from_3d_dimensions(d_3d);
    m_frames.sim_tex_params.width = d[0];
    m_frames.sim_tex_params.height = d[1];
    m_frames.positions.reset(m_frames.sim_tex_params);
    m_frames.velocities.reset(m_frames.sim_tex_params);
    for (int i = 0; i < 4; i++) {
        if (i < 2)
            m_frames.tmp[i].reset(m_frames.sim_tex_params);
        m_frames.rk4_velocities[i].reset(m_frames.sim_tex_params);
        m_frames.rk4_accelerations[i].reset(m_frames.sim_tex_params);
    }
    m_frames.line_to_positions.reset(m_frames.sim_tex_params);
    m_frames.ext_forces.reset(m_frames.sim_tex_params);
    m_wire_frame = get_springs_wire_frame(d_3d, d);
}


}
