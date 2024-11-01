 
#if __VERSION__ <= 120
attribute vec2 position;
varying vec2 UV;
#else
in vec2 position;
out vec2 UV;
#endif

#if (__VERSION__ >= 330) || (defined(GL_ES) && __VERSION__ >= 300)
#define texture2D texture
#else
#define texture texture2D
#endif

#if (__VERSION__ > 120) || defined(GL_ES)
precision highp float;
#endif

uniform sampler2D coordTex;
uniform float xMin;
uniform float xMax;
uniform float yMin;
uniform float yMax;

void main() {
    UV = position;
    vec4 q = texture2D(coordTex, UV);
    vec2 r = q.xy;
    vec2 v = q.zw;
    float domainWidth = xMax - xMin;
    float domainHeight = yMax - yMin;
    float xScaled = (r.x - xMin)/domainWidth;
    float yScaled = (r.y - yMin)/domainHeight;
    gl_Position = vec4(2.0*(vec2(xScaled, yScaled) - vec2(0.5, 0.5)),
                       0.0, 1.0);

}

