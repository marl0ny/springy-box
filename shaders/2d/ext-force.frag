/* Create the external forces as dictated by user mouse/touch input
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

uniform vec2 p1;
uniform sampler2D coordTex;
uniform sampler2D pointToCoordTex;
uniform float sigma;

vec2 norm(vec2 r) {
    float normVal2 = r.x*r.x + r.y*r.y;
    return (normVal2 == 0.0)? vec2(0.0, 0.0): normalize(r);
}

void main() {
    vec2 p0ToR = texture2D(pointToCoordTex, UV).xy;
    vec2 r = texture2D(coordTex, UV).xy;
    vec2 dst = p1 + p0ToR; 
    float strength = 0.2*exp(-0.5*dot(p0ToR, p0ToR)/pow(sigma, 2.0));
    vec2 dir = norm(dst - r);
    vec2 force = dir*strength;
    fragColor = vec4(force, force);
}
