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

// uniform sampler2D coordTexF;
// uniform sampler2D extForcesTex;

void main() {
    fragColor = vec4(0.75, 0.75, 0.75, 1.0);
    /* vec4 q = texture2D(coordTexF, UV);
    vec2 pos = q.xy;
    vec2 f = texture2D(extForcesTex, UV).xy;
    vec3 color = vec3(1.0) + vec3(0.0, 5.0*length(f), 0.0);
    fragColor = vec4(color/max(color.r, max(color.g, color.b)), 1.0);*/
}
