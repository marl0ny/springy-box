
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

let controls = document.getElementById('controls');
createSlider(controls, 0, "stepsPerFrame", "int", {'value': 15, 'min': 0, 'max': 100});
createSlider(controls, 1, "dt", "float", {'value': 0.0005, 'min': 0.0, 'max': 0.0006, 'step': 0.0001});
createSlider(controls, 6, "springConst1", "float", {'value': 600.0, 'min': 300.0, 'max': 1000, 'step': 10});
createSlider(controls, 7, "springConst2", "float", {'value': 600.0, 'min': 300.0, 'max': 1000, 'step': 10});
createSlider(controls, 8, "dragConst", "float", {'value': 0.0002, 'min': 0.0, 'max': 0.001, 'step': 1e-05});
createSlider(controls, 9, "restWidth", "float", {'value': 12.0, 'min': 3.0, 'max': 15.0, 'step': 0.1});
createSlider(controls, 10, "restHeight", "float", {'value': 6.0, 'min': 3.0, 'max': 15.0, 'step': 0.1});
createSlider(controls, 12, "wallFriction", "float", {'value': 1.0, 'min': 0.0, 'max': 2.0, 'step': 0.1});
createSlider(controls, 13, "g", "float", {'value': 9.81, 'min': 0.0, 'max': 20.0, 'step': 0.01});
createSlider(controls, 19, "initialAngle", "float", {'value': 1.5707963267948966, 'min': 0.0, 'max': 6.283185307179586, 'step': 0.006283185307179587});
createSlider(controls, 20, "initialAngularVelocity", "float", {'value': -10.0, 'min': -15.0, 'max': 15.0, 'step': -1.0});

