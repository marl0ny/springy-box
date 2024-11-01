/* Compute the forces from the position-velocity to obtain the
velocity-acceleration.
*/
#if (__VERSION__ >= 330) || (defined(GL_ES) && __VERSION__ >= 300)
#define texture2D texture
#else
#define texture texture2D
#endif

#if (__VERSION__ > 120) || defined(GL_ES)
precision highp float;
#endif
    
#if __VERSION__ <= 120
varying vec2 UV;
#define fragColor gl_FragColor
#else
in vec2 UV;
out vec4 fragColor;
#endif

uniform float springConst1;
uniform float springConst2;
uniform float dragConst;
uniform float xMin;
uniform float xMax;
uniform float yMin;
uniform float yMax;
uniform float wallForceConst;
uniform float wallFriction;
uniform float restWidth;
uniform float restHeight;
uniform int wCount;
uniform int hCount;
uniform float m;
uniform float g;
uniform sampler2D coordTex;
uniform sampler2D extForcesTex;

vec4 get(sampler2D srcTex, vec2 pos) {
    // float boundaryFactor =
    //     (!(pos.x < 0.0 || pos.x > 1.0 || pos.y < 0.0 || pos.y > 1.0))?
    //     1.0: 0.0;
    return texture2D(srcTex, pos);
}

vec2 norm(vec2 r) {
    float normVal2 = r.x*r.x + r.y*r.y;
    return (normVal2 == 0.0)? vec2(0.0, 0.0): normalize(r);
}

vec2 getForce(
    vec2 rC, vec2 vC,
    vec2 rU, vec2 rUR, vec2 rR, vec2 rRD,
    vec2 rD, vec2 rDL, vec2 rL, vec2 rLU) {
    float k = springConst1;
    float kDiag = springConst2;
    float wk = wallForceConst;
    float x0 = restWidth/float(wCount);
    float y0 = restHeight/float(hCount);
    float d0 = sqrt(x0*x0 + y0*y0);
    vec2 extForces = texture2D(extForcesTex, UV).xy;
    return k*norm(rU - rC)*(distance(rU, rC) - y0)
        + kDiag*norm(rUR - rC)*(distance(rUR, rC) - d0)
        + k*norm(rR - rC)*(distance(rR, rC) - x0)
        + kDiag*norm(rRD - rC)*(distance(rRD, rC) - d0)
        + k*norm(rD - rC)*(distance(rD, rC) - y0)
        + kDiag*norm(rDL - rC)*(distance(rDL, rC) - d0)
        + k*norm(rL - rC)*(distance(rL, rC) - x0)
        + kDiag*norm(rLU - rC)*(distance(rLU, rC) - d0)
        - m*g*vec2(0.0, 1.0)
        + wk*((rC.x < xMin)? vec2(-pow(rC.x - xMin, 1.0), 0.0): vec2(0.0))
        + wk*((rC.x > xMax)? vec2(-pow(rC.x - xMax, 1.0), 0.0): vec2(0.0))
        + wk*((rC.y < yMin)? vec2(0.0, -pow(rC.y - yMin, 1.0)): vec2(0.0))
        // + wk*((rC.y > yMax)? vec2(0.0, -pow(rC.y - yMax, 1.0)): vec2(0.0))
        - vC*dragConst
        - wallFriction*vec2(0.0, vC.y)*((rC.x < xMin)? 1.0: 0.0)
        - wallFriction*vec2(0.0, vC.y)*((rC.x > xMax)? 1.0: 0.0)
        - wallFriction*vec2(vC.x, 0.0)*((rC.y < yMin)? 1.0: 0.0)
        + extForces
        // - wallFriction*vec2(vC.x, 0.0)*((rC.y > yMax)? 1.0: 0.0)
        ;
}

void main() {
    float du = 1.0/float(wCount);
    float dv = 1.0/float(hCount);
    vec4 qC = get(coordTex, UV);
    // Clockwise starting from the top.
    vec4 qU = get(coordTex, UV + vec2(0.0, dv));
    vec4 qUR = get(coordTex, UV + vec2(du, dv));
    vec4 qR = get(coordTex, UV + vec2(du, 0.0));
    vec4 qRD = get(coordTex, UV + vec2(du, -dv));
    vec4 qD = get(coordTex, UV + vec2(0.0, -dv));
    vec4 qDL = get(coordTex, UV + vec2(-du, -dv));
    vec4 qL = get(coordTex, UV + vec2(-du, 0.0));
    vec4 qLU = get(coordTex, UV + vec2(-du, dv));
    vec2 force = getForce(
        qC.xy, qC.zw,
        qU.xy, qUR.xy, qR.xy, qRD.xy, qD.xy, qDL.xy, qL.xy, qLU.xy);
    vec2 acceleration = force/m;
    vec2 position = qC.xy;
    vec2 velocity = qC.zw;
    fragColor = vec4(velocity, acceleration);
}
