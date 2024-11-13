import json


def camel_to_snake(camel: str, scream: bool = False) -> str:
    """It is assumed that the input string variable is valid camel case.
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


SLIDER_JS_START = """
function createScalarParameterSlider(
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
    slider.value = spec.value;
    controls.appendChild(document.createElement("br"));
    controls.appendChild(slider);
    controls.appendChild(document.createElement("br"));
    slider.addEventListener("input", e => {
        let valueF = Number.parseFloat(e.target.value);
        let valueI = Number.parseInt(e.target.value);
        if (type === "float") {
            label.textContent = `${sliderLabelName} = ${valueF}`
            Module.set_float_param(enumCode, valueF);
        } else if (type === "int") {
            label.textContent = `${sliderLabelName} = ${valueI}`
            Module.set_int_param(enumCode, valueI);
        }
    });
};

let gVecParams = {};

function createVectorParameterSliders(
    controls, enumCode, sliderLabelName, type, spec) {
    let label = document.createElement("label");
    label.style = "color:white; font-family:Arial, Helvetica, sans-serif";
    label.textContent = `${sliderLabelName} = (${spec.value})`
    gVecParams[sliderLabelName] = spec.value;
    controls.appendChild(label);
    controls.appendChild(document.createElement("br"));
    for (let i = 0; i < spec.value.length; i++) {
        let slider = document.createElement("input");
        slider.type = "range";
        slider.style ="width: 95%;"
        for (let k of Object.keys(spec))
            slider[k] = spec[k][i];
        slider.value = spec.value[i];
        controls.appendChild(slider);
        controls.appendChild(document.createElement("br"));
        slider.addEventListener("input", e => {
            let valueF = Number.parseFloat(e.target.value);
            let valueI = Number.parseInt(e.target.value);
            if (type === "Vec2" || 
                type === "Vec3" || type === "Vec4") {
                gVecParams[sliderLabelName][i] = valueF;
                label.textContent 
                    = `${sliderLabelName} = (${gVecParams[sliderLabelName]})`
                Module.set_vec_param(
                    enumCode, spec.value.length, i, valueF);
            } else if (type === "IVec2" || 
                        type === "IVec3" || type === "IVec4") {
                gVecParams[sliderLabelName][i] = valueI;
                label.textContent 
                    = `${sliderLabelName} = (${gVecParams[sliderLabelName]})`
                Module.set_ivec_param(
                    enumCode, spec.value.length, i, valueI);
            }
        });
    }
};
"""

def write_sliders_js(parameters, dst_file_name):
    file_contents = ""
    file_contents += SLIDER_JS_START
    file_contents += "let controls = document.getElementById('controls');\n"
    for i, k in enumerate(parameters.keys()):
        parameter = parameters[k]
        type_ = parameter["type"]
        value = parameter["value"]
        value_str = f'{value}'
        if 'float' in type_:
            value_str += 'F'
        name = parameter["name"] if "name" in parameter.keys() else k
        if 'min' in parameter and 'max' in parameter:
            p = {k: parameter[k] for k in parameter.keys() 
                     if k in {'min', 'max', 'value', 'step'}}
            if parameter['type'] in ['Vec2', 'Vec3', 'Vec4', 
                                     'IVec2', 'IVec3', 'IVec4']:
                file_contents += \
                    'createVectorParameterSliders(controls, '\
                        f'{i}, "{name}", "{type_}", '\
                            + f'{str(p)});\n'
            else:
                file_contents += \
                    'createScalarParameterSlider(controls, '\
                        f'{i}, "{name}", "{type_}", '\
                            + f'{str(p)});\n'
    file_contents += '\n'
    with open(dst_file_name, "w") as f:
        f.write(file_contents)

HEADER_START = """#include "gl_wrappers.hpp"

namespace {} {}
"""

def write_uniform_parameters_hpp(parameters, name_space, dst_file_name):
    file_contents = HEADER_START.format(name_space, '{')
    file_contents += "\nstruct UniformParams {\n"
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
    file_contents += '    Uniform get(int enum_val) const {\n'
    file_contents += '        switch(enum_val) {\n'
    for i, k in enumerate(parameters.keys()):
        file_contents += \
            f'            case {camel_to_snake(k, scream=True)}:\n'
        file_contents += \
            f'            return {k};\n'
        # file_contents += \
        #    f'            break;\n'
    file_contents += '        }\n'
    file_contents += '    return Uniform(0);\n'
    file_contents += '    }\n'   
    file_contents += "};\n\n}\n"

    with open(dst_file_name, "w") as f:
        f.write(file_contents)

def uniform_member_type(type_: str) -> str:
    if type_ == 'float':
        return 'f32'
    if type_ == 'int':
        return 'i32'
    if type_ == 'bool':
        return 'b32'
    if type_ == 'Vec2':
        return 'vec2'
    if type_ == 'Vec3':
        return 'vec3'
    if type_ == 'Vec4':
        return 'vec4'
    if type_ == 'IVec2':
        return 'ivec2'
    if type_ == 'IVec3':
        return 'ivec3'
    if type_ == 'IVec4':
        return 'ivec4'

def write_typed_sim_parameters_hpp(parameters, name_space, dst_file_name):
    file_contents = HEADER_START.format(name_space, '{')
    file_contents += "\nstruct SimParams {\n"
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
        file_contents += f"    {type_} {k} = "\
            + f"({type_})({value_str});\n"
    file_contents += '    enum {\n'
    for i, k in enumerate(parameters.keys()):
        file_contents += \
            f'        {camel_to_snake(k, scream=True)}={i},\n'
    file_contents += '    };\n'
    file_contents += '    void set(int enum_val, Uniform val) {\n'
    file_contents += '        switch(enum_val) {\n'
    for i, k in enumerate(parameters.keys()):
        type_ = parameters[k]['type']
        file_contents += \
            f'            case {camel_to_snake(k, scream=True)}:\n'
        file_contents += \
            f'            {k} = val.{uniform_member_type(type_)};\n'
        file_contents += \
            f'            break;\n'
    file_contents += '        }\n'
    file_contents += '    }\n' 
    file_contents += '    Uniform get(int enum_val) const {\n'
    file_contents += '        switch(enum_val) {\n'
    for i, k in enumerate(parameters.keys()):
        type_ = parameters[k]['type']
        file_contents += \
            f'            case {camel_to_snake(k, scream=True)}:\n'
        file_contents += \
            '            return {' + f'({type_}){k}' + '};\n'
        # file_contents += \
        #    f'            break;\n'
    file_contents += '        }\n'
    file_contents += '    return Uniform(0);\n'
    file_contents += '    }\n'   
    file_contents += "};\n\n}\n"

    with open(dst_file_name, "w") as f:
        f.write(file_contents)


with open('parameters.json', 'r') as f:
    parameters = json.loads(''.join([line for line in f]))

write_sliders_js(parameters, "sliders.js")
write_uniform_parameters_hpp(parameters, "sim_2d", "uniform_parameters.hpp")
write_typed_sim_parameters_hpp(parameters, "sim_2d", "parameters.hpp")

with open('parameters-3d.json', 'r') as f:
    parameters = json.loads(''.join([line for line in f]))
write_sliders_js(parameters, "sliders-3d.js")
write_uniform_parameters_hpp(parameters, "sim_3d", "uniform_parameters3d.hpp")
write_typed_sim_parameters_hpp(parameters, "sim_3d", "parameters3d.hpp")