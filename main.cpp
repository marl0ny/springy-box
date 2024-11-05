#include "glfw_window.hpp"
#include "box_2d.hpp"
#include "interactor.hpp"
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;
#endif
#include <functional>

static std::function <void()> loop;
#ifdef __EMSCRIPTEN__
static void main_loop() {
    loop();
}
#endif

using namespace::sim_2d;

// bool in_window(Vec2 x) {
//     return (x.x >= 0.0 && x.x < 1.0 && x.y >= 0.0 && x.y < 1.0);
// }

SimParams s_sim_params {};

/*
 Can pass command line arguments to main. This is primarily
 introduced so that the dimensions of the window can be
 chosen before launch, which is particularly useful for 
 the WASM build where the html file page can vary in size.

*/
int main(int argc, char *argv[]) {
    int window_width = 1500, window_height = 1500;
    if (argc >= 2) {
        window_width = std::atoi(argv[1]);
        window_height = std::atoi(argv[2]);
    }
    GLFWwindow *window = init_window(window_width, window_height);
    auto simulation = Simulation(
        window_width, window_height, s_sim_params);
    Interactor interactor(window);
    float x_min = s_sim_params.xMin.f32;
    float y_min = s_sim_params.yMin.f32;
    Vec2 mouse_pos = Vec2{
        .ind={-10.0F + x_min,
              -10.0F + y_min}};
    int n_clicks = 0;
    
    loop = [&] {
        int steps_frame = s_sim_params.stepsPerFrame.i32;
        for (int i = 0; i < steps_frame; i++)
            simulation.time_step(s_sim_params);
        simulation.render_view(s_sim_params);
        glfwPollEvents();
        {
            interactor.click_update(window);
            if (interactor.left_pressed()) {
                std::cout << "x: " << interactor.get_mouse_position().x << std::endl;
                std::cout << "y: " << interactor.get_mouse_position().y << std::endl;
                mouse_pos = interactor.get_mouse_position();
                float x_min = s_sim_params.xMin.f32;
                float x_max = s_sim_params.xMax.f32;
                float y_min = s_sim_params.xMin.f32;
                float y_max = s_sim_params.xMax.f32;
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
                simulation.init_config(s_sim_params);
        };
        glfwSwapBuffers(window);
    };

    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, true);
    #else
    while (!glfwWindowShouldClose(window))
        loop();
    #endif

    glfwDestroyWindow(window);
    return 1;
}

#ifdef __EMSCRIPTEN__

/* Setters for the global static simulation parameters
struct, where they are the exposed entry point for
JavaScript code in the WASM build.
There are multiple functions, one for each type.
They take as argument a param_code, where these codes
must be written separately in JavaScript, followed by a number
of float or int arguments. For Vector arguments, multiple floats
and ints must be passed, instead of passing the Vector structs
themselves. This is done so that I won't
have to deal with the complexity of getting non-primitive objects
to be passed between JS/C++.
*/

void set_int_param(int param_code, int i) {
    s_sim_params.set(param_code, Uniform((int)i));
}

void set_float_param(int param_code, float f) {
    s_sim_params.set(param_code, Uniform((float)f));
}

void set_bool_param(int param_code, bool b) {
    s_sim_params.set(param_code, Uniform((bool)b));
}

void set_vec2_param(int param_code, float x, float y) {
    s_sim_params.set(
        param_code, 
        Uniform(Vec2 {.x=x, .y=y}));
}

void set_vec3_param(int param_code, float x, float y, float z) {
    s_sim_params.set(
        param_code, 
        Uniform(Vec3 {.x=x, .y=y, .z=z}));
}

void set_vec4_param(int param_code, float x, float y, float z, float w) {
    s_sim_params.set(
        param_code, 
        Uniform(Vec4 {.x=x, .y=y, .z=z, .w=w}));
}

void set_vec_param(int param_code, int elemCount, int index, float val) {
    auto u = s_sim_params.get(param_code);
    if (elemCount == 2) {
        u.vec2[index] = val;
    } else if (elemCount == 3) {
        u.vec3[index] = val;
    } else {
        u.vec4[index] = val;
    }
    s_sim_params.set(param_code, u);
}

void set_ivec_param(int param_code, int elemCount, int index, float val) {
    auto u = s_sim_params.get(param_code);
    if (elemCount == 2) {
        u.ivec2[index] = val;
    } else if (elemCount == 3) {
        u.ivec3[index] = val;
    } else {
        u.ivec4[index] = val;
    }
    s_sim_params.set(param_code, u);
}

EMSCRIPTEN_BINDINGS(my_module) {
    function("set_float_param", set_float_param);
    function("set_int_param", set_int_param);
    function("set_bool_param", set_bool_param);
    function("set_vec2_param", set_vec2_param);
    function("set_vec3_param", set_vec3_param);
    function("set_vec4_param", set_vec4_param);
    function("set_vec_param", set_vec_param);
    function("set_ivec_param", set_ivec_param);
}
#endif