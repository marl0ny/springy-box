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

void set_steps_frame(int val) {
    s_sim_params.steps_frame = val;
}

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_BINDINGS(my_module) {
    function("set_steps_frame", set_steps_frame);
}
#endif

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
    Vec2 mouse_pos = Vec2{
        .ind={-10.0F + s_sim_params.x_min,
              -10.0F + s_sim_params.y_min}};
    int n_clicks = 0;
    
    loop = [&] {
        for (int i = 0; i < s_sim_params.steps_frame; i++)
            simulation.time_step(s_sim_params);
        simulation.render_view(s_sim_params);
        glfwPollEvents();
        {
            interactor.click_update(window);
            if (interactor.left_pressed()) {
                std::cout << "x: " << interactor.get_mouse_position().x << std::endl;
                std::cout << "y: " << interactor.get_mouse_position().y << std::endl;
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