
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
createScalarParameterSlider(controls, 1, "Time Step (s)", "float", {'value': 0.0005, 'min': 0.0, 'max': 0.0006, 'step': 1e-05});
createScalarParameterSlider(controls, 6, "Spring Strength 1 (N/m)", "float", {'value': 600.0, 'min': 300.0, 'max': 1000.0, 'step': 10.0});
createScalarParameterSlider(controls, 7, "Spring Strength 2 (N/m)", "float", {'value': 600.0, 'min': 50.0, 'max': 1000.0, 'step': 10.0});
createScalarParameterSlider(controls, 8, "Drag Force-Velocity Relation (N s/m)", "float", {'value': 0.0002, 'min': 0.0, 'max': 0.001, 'step': 1e-05});
createScalarParameterSlider(controls, 9, "Rest Width (m)", "float", {'value': 12.0, 'min': 3.0, 'max': 15.0, 'step': 0.1});
createScalarParameterSlider(controls, 10, "Rest Height (m)", "float", {'value': 6.0, 'min': 3.0, 'max': 15.0, 'step': 0.1});
createScalarParameterSlider(controls, 12, "Friction Force-Velocity Relation (N s/m)", "float", {'value': 1.0, 'min': 0.0, 'max': 2.0, 'step': 0.1});
createScalarParameterSlider(controls, 13, "Gravity (N)", "float", {'value': 9.81, 'min': 0.0, 'max': 20.0, 'step': 0.01});
createVectorParameterSliders(controls, 17, "Initial Position (m)", "Vec2", {'value': [10.0, 10.0], 'min': [5.0, 5.0], 'max': [15.0, 15.0], 'step': [0.1, 0.1]});
createVectorParameterSliders(controls, 18, "Initial Velocity (m/s)", "Vec2", {'value': [10.0, 10.0], 'min': [-15.0, -15.0], 'max': [15.0, 15.0], 'step': [0.1, 0.1]});
createScalarParameterSlider(controls, 19, "Initial Angle Offset (rad)", "float", {'value': 1.5707963267948966, 'min': 0.0, 'max': 6.283185307179586, 'step': 0.006283185307179587});
createScalarParameterSlider(controls, 20, "Initial Angular Speed (rad/s)", "float", {'value': -10.0, 'min': -15.0, 'max': 15.0, 'step': -1.0});

