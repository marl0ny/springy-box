import json


def camel_to_snake(camel: str, scream: bool = False) -> str:
    """ It is assumed that the input string variable is valid camel case.
    """
    snake_list = []
    for i, c in enumerate(camel):
        if c.isupper():
            if i == 0:
                snake_list.append(c if scream else c.lower())
            else:
                snake_list.extend(['_', c if scream else c.lower()])
        else:
            snake_list.append(c.upper() if scream else c)
    return ''.join(snake_list)

CREATE_SLIDER_JS_FUNC = """
function createSlider(
    controls, enumCode, sliderLabelName, type, spec) {
    let label = document.createElement("label");
    label.for = spec['id']
    label.style = "color:white; font-family:Arial, Helvetica, sans-serif";
    label.textContent = `${sliderLabelName} = ${spec.value}`
    controls.appendChild(label);
    let slider = document.createElement("input");
    slider.type = "range";
    slider.style ="width: 95%;"
    for (let k of Object.keys(spec))
        slider[k] = spec[k];
    controls.appendChild(document.createElement("br"));
    controls.appendChild(slider);
    controls.appendChild(document.createElement("br"));
    slider.addEventListener("input", e => {
        if (type === "float") {
            let value = Number.parseFloat(e.target.value);
            label.textContent = `${sliderLabelName} = ${value}`
            Module.set_float_param(enumCode, value);
        }
        if (type === "int") {
            let value = Number.parseInt(e.target.value);
            label.textContent = `${sliderLabelName} = ${value}`
            Module.set_int_param(enumCode, value);
        }
    });
};

"""

def write_sliders_js(parameters):
    file_contents = ""
    file_contents += CREATE_SLIDER_JS_FUNC
    file_contents += "let controls = document.getElementById('controls');\n"
    for i, k in enumerate(parameters.keys()):
        parameter = parameters[k]
        type_ = parameter["type"]
        value = parameter["value"]
        value_str = f'{value}'
        if 'float' in type_:
            value_str += 'F'
        if 'min' in parameter and 'max' in parameter:
            if 'Vec2' in parameter["type"]:
                pass 
                # p1 = {k: parameter[k][0] for k in parameter.keys() 
                #       if k in {'min', 'max', 'value', 'step'}}
                # p2 = {k: parameter[k][0] for k in parameter.keys() 
                #       if k in {'min', 'max', 'value', 'step'}}
                # k1, k2 = k + ' (x)', k + ' (y)'
                # file_contents += \
                #     f'createSlider(controls, {i}, "{k1}", "{type_}",'\
                #         + f' {str(p1)});\n'
                # file_contents += \
                #     f'createSlider(controls, {i}, "{k2}", "{type_}",'\
                #         + f' {str(p2)});\n'
            else:
                p = {k: parameter[k] for k in parameter.keys() 
                     if k in {'min', 'max', 'value', 'step'}}
                file_contents += \
                    f'createSlider(controls, {i}, "{k}", "{type_}", '\
                        + f'{str(p)});\n'
    file_contents += '\n'
    with open("sliders.js", "w") as f:
        f.write(file_contents)


def write_uniform_parameters_hpp(parameters):
    file_contents = """#include "gl_wrappers.hpp"

    namespace sim_2d {

    """
    file_contents += "struct UniformParams {\n"
    for k in parameters.keys():
        parameter = parameters[k]
        type_ = parameter["type"]
        value = parameter["value"]
        value_str = f'{value}'
        if 'Vec' in type_:
            value_str = \
                f'{type_}' + \
                    ' {.ind={' + ', '.join([f'{e}' for e in value])\
                    + '}}'
        if 'float' in type_:
            value_str += 'F'
        file_contents += f"    Uniform {k} = "\
            + f"Uniform(({type_})({value_str}));\n"
    file_contents += '    enum {\n'
    for i, k in enumerate(parameters.keys()):
        file_contents += \
            f'        {camel_to_snake(k, scream=True)}={i},\n'
    file_contents += '    };\n'
    file_contents += '    void set(int enum_val, Uniform val) {\n'
    file_contents += '        switch(enum_val) {\n'
    for i, k in enumerate(parameters.keys()):
        file_contents += \
            f'            case {camel_to_snake(k, scream=True)}:\n'
        file_contents += \
            f'            {k} = val;\n'
        file_contents += \
            f'            break;\n'
    file_contents += '        }\n'
    file_contents += '    }\n'    
    file_contents += "};\n\n}\n"

    with open("uniform_parameters.hpp", "w") as f:
        f.write(file_contents)


with open('parameters.json', 'r') as f:
    parameters = json.loads(''.join([line for line in f]))

write_sliders_js(parameters)
write_uniform_parameters_hpp(parameters)