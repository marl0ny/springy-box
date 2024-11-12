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
uniform vec3 xyzMin;
uniform vec3 xyzMax;
uniform float wallForceConst;
uniform float wallFriction;
uniform vec3 restDimensions;
uniform ivec3 springCountDimensions;
uniform float m;
uniform float g;
uniform sampler2D positionsTex;
uniform sampler2D velocitiesTex;
uniform sampler2D extForcesTex;

uniform ivec2 texelDimensions2D;
uniform ivec3 texelDimensions3D;

vec2 to2DTextureCoordinates(vec3 uvw) {
    int width2D = texelDimensions2D[0];
    int height2D = texelDimensions2D[1];
    int width3D = texelDimensions3D[0];
    int height3D = texelDimensions3D[1];
    int length3D = texelDimensions3D[2];
    float wStack = float(width2D)/float(width3D);
    // float hStack = float(height2D)/float(height3D);
    float xIndex = float(width3D)*mod(uvw[0], 1.0);
    float yIndex = float(height3D)*mod(uvw[1], 1.0);
    float zIndex = mod(floor(float(length3D)*uvw[2]), float(length3D));
    float uIndex = mod(zIndex, wStack)*float(width3D) + xIndex; 
    float vIndex = floor(zIndex / wStack)*float(height3D) + yIndex; 
    return vec2(uIndex/float(width2D), vIndex/float(height2D));
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

vec3 get(sampler2D srcTex, vec3 pos) {
    return texture2D(srcTex, to2DTextureCoordinates(pos)).xyz;
}

vec3 get(sampler2D srcTex, vec2 pos) {
    return texture2D(srcTex, pos).xyz;
}

vec3 norm(vec3 r) {
    float normVal2 = r.x*r.x + r.y*r.y + r.z*r.z;
    return (normVal2 == 0.0)? vec3(0.0, 0.0, 0.0): normalize(r);
}

bool inDomain(vec3 uvw) {
    return (uvw[0] < 1.0 && uvw[0] > 0.0 &&
            uvw[1] < 1.0 && uvw[1] > 0.0 &&
            uvw[2] < 1.0 && uvw[2] > 0.0);
}

vec3 getSpringForce(vec3 r0, ivec3 offset) {
    vec3 uvw = to3DTextureCoordinates(UV);
    float du = float(offset[0])/float(springCountDimensions[0]);
    float dv = float(offset[1])/float(springCountDimensions[1]);
    float dw = float(offset[2])/float(springCountDimensions[2]);
    float dx = restDimensions[0]*du;
    float dy = restDimensions[1]*dv;
    float dz = restDimensions[2]*dw;
    // vec3 r0 = get(positionsTex, uvw);
    vec3 r1 = get(positionsTex, uvw + vec3(du, dv, dw));
    float k = springConst1;
    if ((offset[0] + offset[1] + offset[2]) >= 2)
        k = springConst2;
    // if ((offset[0] + offset[1] + offset[2]) > 2)
    //     k = 0.0;
    float d = sqrt(dx*dx + dy*dy + dz*dz);
    return (inDomain(uvw + vec3(du, dv, dw)))?
        k*norm(r1 - r0)*(distance(r1, r0) - d): vec3(0.0);
}

vec3 getForce(vec3 rC, vec3 vC) {
    // float k = springConst1;
    // float kDiag = springConst2;
    // float wk = wallForceConst;
    // float x0 = restDimensions[0]/float(springCountDimensions[0]);
    // float y0 = restDimensions[1]/float(springCountDimensions[1]);
    // float z0 = restDimensions[2]/float(springCountDimensions[2]);
    // float d0 = sqrt(x0*x0 + y0*y0 + z0*z0);
    float xMin = xyzMin[0], yMin = xyzMin[1], zMin = xyzMin[2];
    float xMax = xyzMax[0], yMax = xyzMax[1], zMax = xyzMax[2];
    vec3 extForces = texture2D(extForcesTex, UV).xyz;
    vec3 springForces = vec3(0.0);
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            for (int k = -1; k <= 1; k++) {
                springForces += getSpringForce(rC, ivec3(i, j, k));
            }
        }
    }
    float wk = wallForceConst;
    float wf = wallFriction;
    return (
        springForces
        - m*g*vec3(0.0, 1.0, 0.0)
        + wk*((rC.x < xMin)? 
            vec3(-pow(rC.x - xMin, 1.0), 0.0, 0.0): vec3(0.0))
        + wk*((rC.x > xMax)?
            vec3(-pow(rC.x - xMax, 1.0), 0.0, 0.0): vec3(0.0))
        + wk*((rC.y < yMin)?
            vec3(0.0, -pow(rC.y - yMin, 1.0), 0.0): vec3(0.0))
        // + wk*((rC.y > yMax)?
        //     vec3(0.0, -pow(rC.y - yMax, 1.0), 0.0): vec3(0.0))
        + wk*((rC.z < zMin)?
            vec3(0.0, 0.0, -pow(rC.z - zMin, 1.0)): vec3(0.0))
        + wk*((rC.z > zMax)?
            vec3(0.0, 0.0, -pow(rC.z - zMax, 1.0)): vec3(0.0))
        - vC*dragConst
        - wf*vec3(0.0, vC.yz)*((rC.x < xMin || rC.x > xMax)? 1.0: 0.0)
        - wf*vec3(vC.x, 0.0, vC.z)*((rC.y < yMin)? 1.0: 0.0)
        // - wf*vec3(vC.x, 0.0, vC.z)*((rC.y > yMax)? 1.0: 0.0)
        - wf*vec3(vC.xy, 0.0)*((rC.z < zMin || rC.z > zMax)? 1.0: 0.0)
        + extForces
    );
}

void main() {
    vec3 rC = get(positionsTex, UV).xyz;
    vec3 vC = get(velocitiesTex, UV).xyz;
    vec3 force = getForce(rC, vC);
    vec3 acceleration = force/m;
    fragColor = vec4(acceleration, 1.0);
}
