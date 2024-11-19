
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
let controls = document.getElementById('controls');
createScalarParameterSlider(controls, 0, "Steps/Frame", "int", {'value': 30, 'min': 0, 'max': 100});
createScalarParameterSlider(controls, 1, "Time Step (s)", "float", {'value': 0.0005, 'min': 0.0, 'max': 0.00055, 'step': 1e-05});
createScalarParameterSlider(controls, 4, "Spring Strength (N/m)", "float", {'value': 30.0, 'min': 10.0, 'max': 100.0, 'step': 1.0});
createScalarParameterSlider(controls, 6, "Drag Force-Velocity Relation (N s/m)", "float", {'value': 0.0002, 'min': 0.0, 'max': 0.001, 'step': 1e-05});
createVectorParameterSliders(controls, 7, "Spring count dimensions", "IVec3", {'value': [32, 16, 32], 'min': [8, 8, 8], 'max': [100, 100, 100], 'step': [1, 1, 1]});
createVectorParameterSliders(controls, 8, "Rest Dimensions (m)", "Vec3", {'value': [12.0, 6.0, 12.0], 'min': [3.0, 3.0, 3.0], 'max': [15.0, 15.0, 15.0], 'step': [0.1, 0.1, 0.1]});
createScalarParameterSlider(controls, 10, "Friction Force-Velocity Relation (N s/m)", "float", {'value': 1.0, 'min': 0.0, 'max': 2.0, 'step': 0.1});
createScalarParameterSlider(controls, 11, "Gravity (N)", "float", {'value': 9.81, 'min': 0.0, 'max': 20.0, 'step': 0.01});
createVectorParameterSliders(controls, 13, "Initial Position (m)", "Vec3", {'value': [10.0, 12.0, 10.0], 'min': [5.0, 5.0, 5.0], 'max': [15.0, 30.0, 15.0], 'step': [0.1, 0.1, 0.1]});
createVectorParameterSliders(controls, 14, "Initial Velocity (m/s)", "Vec3", {'value': [0.0, 0.0, 0.0], 'min': [-15.0, -15.0, -15.0], 'max': [15.0, 15.0, 15.0], 'step': [0.1, 0.1, 0.1]});
createVectorParameterSliders(controls, 15, "Initial Axis of Orientation", "Vec3", {'value': [1.0, -0.25, 0.0], 'min': [-1.0, -1.0, -1.0], 'max': [1.0, 1.0, 1.0], 'step': [0.01, 0.01, 0.01]});
createVectorParameterSliders(controls, 16, "Initial Axis of Rotation", "Vec3", {'value': [1.0, -0.25, 0.0], 'min': [-1.0, -1.0, -1.0], 'max': [1.0, 1.0, 1.0], 'step': [0.01, 0.01, 0.01]});
createScalarParameterSlider(controls, 17, "Initial Angular Speed (rad/s)", "float", {'value': 0.0, 'min': -8.0, 'max': 8.0, 'step': 0.1});

