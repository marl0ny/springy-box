/* Create the external forces as dictated by user mouse/touch input.
Transforming the normalized screen positions of mouse/touch to a position
in simulation space are handled in this shader.
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

uniform sampler2D positionsTex;
uniform sampler2D lineToPositionsTex;
uniform float sigma;

#define quaternion vec4

uniform vec2 screenPosition;
uniform vec3 rMin;
uniform vec3 rMax;
uniform float scale;
uniform quaternion rotation;
uniform ivec2 screenDimensions;

vec3 norm(vec3 r) {
    float normVal2 = dot(r, r);
    return (normVal2 == 0.0)? vec3(0.0): normalize(r);
}


quaternion mul(quaternion q1, quaternion q2) {
    quaternion q3;
    q3.w = q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z;
    q3.x = q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y; 
    q3.y = q1.w*q2.y + q1.y*q2.w + q1.z*q2.x - q1.x*q2.z; 
    q3.z = q1.w*q2.z + q1.z*q2.w + q1.x*q2.y - q1.y*q2.x;
    return q3; 
}

quaternion conj(quaternion q) {
    return quaternion(-q.x, -q.y, -q.z, q.w);
}

quaternion rotate(quaternion x, quaternion r) {
    return quaternion(mul(conj(r), mul(x, r)).xyz, 1.0);
}

vec4 deProject(vec4 y) {
    float a = 4.0;
    float b = float(screenDimensions[0])/float(screenDimensions[1]);
    float c = 4.0;
    vec4 x;
    x[0] = y[0]*(a*y[2] + c)/a;
    x[1] = y[1]*(a*y[2] + c)/(a*b);
    x[2] = y[2]*a;
    x[3] = 1.0;
    return x;
}

vec3 toViewSpace(vec3 x) {
    vec4 w = vec4(2.0*x - vec3(1.0), 1.0);
    return (rotate(deProject(w), conj(rotation))/scale).xyz;
}

vec3 toSimSpace(vec3 rViewSpace) {
    vec3 domainDimensions = rMax - rMin;
    return domainDimensions*(rViewSpace/2.0 + vec3(0.5)) + rMin;
}

void main() {
    vec3 l0 = toSimSpace(toViewSpace(vec3(screenPosition, 0.0)));
    vec3 l1 = toSimSpace(toViewSpace(vec3(screenPosition, 1.0)));
    vec3 d = l1 - l0;
    vec3 s1 = texture2D(lineToPositionsTex, UV).xyz;
    vec3 r = texture2D(positionsTex, UV).xyz - l0;
    vec3 s2 = d*dot(d, r)/dot(d, d) - r;
    vec3 dir = s1 + s2;
    float strength = 0.05*exp(-0.5*dot(s1, s1)/pow(sigma, 2.0));
    vec3 force = dir*strength;
    fragColor = vec4(force, 1.0);
}
