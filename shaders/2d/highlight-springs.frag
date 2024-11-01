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

uniform vec2 r0;
uniform float sigma;
uniform sampler2D coordTex;

void main() {
    vec4 q = texture2D(coordTex, UV);
    fragColor = vec4(
        exp(-0.5*dot(q.xy - r0, q.xy - r0)/pow(sigma, 2.0)));
}
