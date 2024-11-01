
/* Write to a texture the distance from a point given by the uniform vec2
r to the distance of each spring as stored in each texel of the
input texture from the the uniform sampler2D coordTex. */
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

uniform vec2 r;
uniform sampler2D coordTex;

void main() {
    vec4 q = texture2D(coordTex, UV);
    fragColor = vec4(q.xy - r, q.xy - r);
}
