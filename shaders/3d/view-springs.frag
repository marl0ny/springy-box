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

uniform sampler2D extForcesTex;
uniform sampler2D lineToPositionsTex;


void main() {
    // vec3 r = texture2D(lineToPositionsTex, UV).xyz;
    // vec3 color = vec3(1.0) + vec3(0.0, exp(-0.5*pow(length(r), 2.0)), 0.0);
    // fragColor = vec4(color/max(color.r, max(color.g, color.b)), 1.0);
    vec3 f = texture2D(extForcesTex, UV).xyz;
    vec3 color = vec3(1.0) + vec3(0.0, 5.0*length(f), 0.0);
    fragColor = vec4(color/max(color.r, max(color.g, color.b)), 1.0);

}
