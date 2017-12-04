#version 330 core
out vec4 FragColor;

uniform float time;
uniform vec2 resolution;

uniform float checkboxSize;
uniform float scrollSpeed;
uniform float rotateSpeed;
uniform float geometry;
uniform float horizon;

vec3 checkerBoard( vec2 uv, vec2 pp )
{
    vec2 p = floor( uv * checkboxSize);
    float t = mod( p.x + p.y, 2.2);
    vec3 c = vec3(t+pp.x, t+pp.y, t+(pp.x*pp.y));

    return c;
}

vec3 tunnel( vec2 p)
{    
    float a = 2.0 * atan(p.x, p.y);
    float po = geometry;
    float px = pow( p.x*p.x, po );
    float py = pow( p.y*p.y, po );
    float r = pow( px + py, horizon/(2.0*po) );    
    vec2 uvp = vec2( 1.0/r + (time*scrollSpeed), a + (time*rotateSpeed));	
    vec3 finalColor = checkerBoard( uvp, p ).xyz;
    finalColor *= r;

    return finalColor;
}

void main()
{
    vec2 uv = gl_FragCoord.xy / resolution.xy;

    vec2 p = uv + vec2(-0.5, -0.5);

    vec3 finalColor = tunnel(p);

    FragColor = vec4( finalColor, 1.0 );
} 