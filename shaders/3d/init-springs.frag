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

uniform vec3 dimensions3D;
uniform vec3 position;
uniform vec3 velocity;
uniform vec3 axis;
uniform vec3 rotationAxis;
uniform float angularSpeed;

uniform int mode;
#define INIT_POSITIONS 0
#define INIT_VELOCITIES 1

uniform ivec3 texelDimensions3D;
uniform ivec2 texelDimensions2D;

#define quaternion vec4

#define PI 3.141592653589793

vec3 crossProduct(vec3 a, vec3 b) {
    return vec3(
        a.y*b.z - a.z*b.y, 
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    );
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

quaternion getRotator(float angle, vec3 v) {
    float norm = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    if (norm == 0.0)
        return quaternion(0.0, 0.0, 0.0, 1.0);
    float c = cos(angle/2.0);
    float s = sin(angle/2.0);
    return quaternion(s*v.x/norm, s*v.y/norm, s*v.z/norm, c);
}

quaternion getRotator(vec3 v) {
    float vLength = length(v);
    return getRotator(vLength*PI/2.0, v);
}

vec3 to3DTextureCoordinates(vec2 uv) {
    int width3D = texelDimensions3D[0];
    int height3D = texelDimensions3D[1];
    int length3D = texelDimensions3D[2];
    int width2D = texelDimensions2D[0];
    int height2D = texelDimensions2D[1];
    float wStack = float(width2D)/float(width3D);
    float hStack = float(height2D)/float(height3D);
    float u = mod(uv[0]*wStack, 1.0);
    float v = mod(uv[1]*hStack, 1.0);
    float w = (floor(uv[1]*hStack)*wStack
               + floor(uv[0]*wStack) + 0.5)/float(length3D);
    return vec3(u, v, w);
}

void main() {
    vec3 uvw = to3DTextureCoordinates(UV);
    vec3 r0 = (uvw - vec3(0.5))*dimensions3D;
    fragColor = vec4(r0, 1.0);
    float angle = acos(dot(normalize(axis), vec3(0.0, 1.0, 0.0)));
    quaternion rotator = getRotator(angle,
        crossProduct(normalize(axis), vec3(0.0, 1.0, 0.0)));
    vec4 r = rotate(quaternion(r0, 1.0), rotator);
    vec3 s = r.xyz - dot(r.xyz, rotationAxis)/dot(rotationAxis, rotationAxis);
    vec3 v = (length(s) == 0.0)?
        vec3(0.0): angularSpeed*crossProduct(normalize(rotationAxis), s);
    if (mode == INIT_POSITIONS)
        fragColor = vec4(position, 0.0) + r;
    else
        fragColor = vec4(velocity, 0.0) + vec4(v, 1.0);

}