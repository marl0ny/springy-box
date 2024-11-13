#include "glfw_window.hpp"
#include "box_2d.hpp"
#include "box_3d.hpp"
#include "interactor.hpp"
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;
#endif
#include <functional>

static std::function <void()> s_loop;
#ifdef __EMSCRIPTEN__
static void s_main_loop() {
    s_loop();
}
#endif

static std::function <void(int, Uniform)> s_sim_params_set;
static std::function <Uniform(int)> s_sim_params_get;

void box_3d(GLFWwindow *window, 
            int window_width, int window_height,
            Interactor interactor,
            sim_3d::SimParams &sim_params
            ) {
    auto simulation = sim_3d::Simulation(
        window_width, window_height, sim_params);
    Quaternion rotation = Quaternion{.i=0.0, .j=0.0, .k=0.0, .real=1.0}; 
    s_loop = [&] {
        int steps_frame = sim_params.stepsPerFrame;
        for (int i = 0; i < steps_frame; i++)
            simulation.time_step(sim_params);
        simulation.render_view(sim_params, rotation, Interactor::get_scroll());
        // simulation.render_view(sim_params);
        glfwPollEvents();
        {
            interactor.click_update(window);
            if (interactor.left_pressed()) {
                Vec2 pos = interactor.get_mouse_position();
                if (pos.x > 0.0 && pos.x < 1.0 && 
                    pos.y > 0.0 && pos.y < 1.0) {
                    Vec2 delta_2d = interactor.get_mouse_delta();
                    Vec3 delta {.ind={delta_2d[0], delta_2d[1], 0.0}};
                    Vec3 view_vec {.ind={0.0, 0.0, -1.0}};
                    Vec3 axis = cross_product(delta, view_vec);
                    Quaternion rot = Quaternion::rotator(
                        3.0*axis.length(), axis);
                    rotation = rotation*rot;
                }
            } else {

            }
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
                simulation.init_config(sim_params);
        };
        glfwSwapBuffers(window);
    };
    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(s_main_loop, 0, true);
    #else
    while (!glfwWindowShouldClose(window))
        s_loop();
    #endif
}

void box_2d(GLFWwindow *window, 
            int window_width, int window_height,
            Interactor interactor,
            sim_2d::SimParams &sim_params
            ) {
    float x_min = sim_params.xMin.f32;
    float y_min = sim_params.yMin.f32;
    Vec2 mouse_pos = Vec2{
        .ind={-10.0F + x_min,
              -10.0F + y_min}};
    int n_clicks = 0;
    auto simulation = sim_2d::Simulation(
        window_width, window_height, sim_params);
    s_loop = [&] {
        /* Quad q {TextureParams {
            .width=100, .height=100, .format=34836,
            .mag_filter=GL_LINEAR, .min_filter=GL_LINEAR,
            .wrap_s=GL_CLAMP_TO_EDGE, .wrap_t=GL_CLAMP_TO_EDGE}};*/
        int steps_frame = sim_params.stepsPerFrame.i32;
        for (int i = 0; i < steps_frame; i++)
            simulation.time_step(sim_params);
        simulation.render_view(sim_params);
        glfwPollEvents();
        {
            interactor.click_update(window);
            if (interactor.left_pressed()) {
                std::cout << "x: " << 
                    interactor.get_mouse_position().x << std::endl;
                std::cout << "y: " << 
                    interactor.get_mouse_position().y << std::endl;
                mouse_pos = interactor.get_mouse_position();
                float x_min = sim_params.xMin.f32;
                float x_max = sim_params.xMax.f32;
                float y_min = sim_params.xMin.f32;
                float y_max = sim_params.xMax.f32;
                Vec2 interact_pos = {
                    .ind={
                        x_min
                        + mouse_pos[0]
                            *(x_max - x_min),
                        y_min
                        + mouse_pos[1]
                            *(y_max - y_min)
                    }
                };
                float sigma = 0.05*(x_max - x_min);
                if (n_clicks++ == 0) {
                    simulation.set_hold_position(interact_pos);
                }
                simulation.add_ext_force(interact_pos, sigma);

            } else {
                n_clicks = 0;
            }
            if (interactor.left_released()) {
                simulation.clear_ext_force();
            }
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
                simulation.init_config(sim_params);
        };
        glfwSwapBuffers(window);
    };

    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(s_main_loop, 0, true);
    #else
    while (!glfwWindowShouldClose(window))
        s_loop();
    #endif
}

/*
 Can pass command line arguments to main. This is primarily
 introduced so that the dimensions of the window can be
 chosen before launch, which is particularly useful for 
 the WASM build where the html file page can vary in size.

*/
int main(int argc, char *argv[]) {
    int window_width = 1500, window_height = 1500;
    int which = 0;
    if (argc >= 3) {
        window_width = std::atoi(argv[1]);
        window_height = std::atoi(argv[2]);
        if (argc >= 4) which = std::atoi(argv[3]);
    }
    GLFWwindow *window = init_window(window_width, window_height);
    Interactor interactor(window);
    if (which % 2 == 0) {
        sim_2d::SimParams sim_params_2d {};
        s_sim_params_set = [&sim_params_2d](int c, Uniform u) {
            sim_params_2d.set(c, u);
        };
        s_sim_params_get = [&sim_params_2d](int c) -> Uniform {
            return sim_params_2d.get(c);
        };
        box_2d(window, window_width, window_height, 
                interactor, sim_params_2d);
    } else {
        sim_3d::SimParams sim_params_3d {};
        s_sim_params_set = [&sim_params_3d](int c, Uniform u) {
            sim_params_3d.set(c, u);
        };
        s_sim_params_get = [&sim_params_3d](int c) -> Uniform {
            return sim_params_3d.get(c);
        };
        box_3d(window, window_width, window_height,
                interactor, sim_params_3d);
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 1;
}

#ifdef __EMSCRIPTEN__

/* Setters for the global static simulation parameters struct, where they act
as the exposed entry point for JavaScript code in the WASM build.
There are multiple functions, one for each type. They all take as the first
argument a param_code representing each field of the parameter struct,
where these codes must be written and enumerated separately in JavaScript.
For the function setters of scalar quantities, the next and final argument is
just the quantity itself. But for those functions that set a vector quantity,
these next arguments in order must be passed into the function: 
the number of elements the vector contains, the index of the vector to change
the value, and lastly the value itself. Meanwhile, the actual vector structs
themselves are not passed as argument: this is to avoid the complexity of 
getting non-primitive objects to be passed between JS/C++.
*/

void set_int_param(int param_code, int i) {
    s_sim_params_set(param_code, Uniform((int)i));
}

void set_float_param(int param_code, float f) {
    s_sim_params_set(param_code, Uniform((float)f));
}

void set_bool_param(int param_code, bool b) {
    s_sim_params_set(param_code, Uniform((bool)b));
}

void set_vec_param(int param_code, int elemCount, int index, float val) {
    auto u = s_sim_params_get(param_code);
    if (elemCount == 2) {
        u.vec2[index] = val;
    } else if (elemCount == 3) {
        u.vec3[index] = val;
    } else {
        u.vec4[index] = val;
    }
    s_sim_params_set(param_code, u);
}

void set_ivec_param(int param_code, int elemCount, int index, float val) {
    auto u = s_sim_params_get(param_code);
    if (elemCount == 2) {
        u.ivec2[index] = val;
    } else if (elemCount == 3) {
        u.ivec3[index] = val;
    } else {
        u.ivec4[index] = val;
    }
    s_sim_params_set(param_code, u);
}

EMSCRIPTEN_BINDINGS(my_module) {
    function("set_float_param", set_float_param);
    function("set_int_param", set_int_param);
    function("set_bool_param", set_bool_param);
    function("set_vec_param", set_vec_param);
    function("set_ivec_param", set_ivec_param);
}
#endif