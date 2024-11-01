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

uniform float width;
uniform float height;
uniform vec2 position;
uniform vec2 velocity;
uniform float angle;
uniform float omega;

void main() {
    float u = UV[0], v = UV[1];
    float rx0 = (u - 0.5)*width;
    float ry0 = (v - 0.5)*height;
    float rx = position.x + rx0*cos(angle) + ry0*sin(angle);
    float ry = position.y - rx0*sin(angle) + ry0*cos(angle);
    float vx = velocity.x 
        - omega*rx0*sin(angle) + omega*ry0*cos(angle);
    float vy = velocity.y 
        - omega*rx0*cos(angle) - omega*ry0*sin(angle);
    fragColor = vec4(rx, ry, vx, vy);
}