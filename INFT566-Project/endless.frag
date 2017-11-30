#version 330 core
out vec4 FragColor;

uniform float time;
uniform vec2 resolution;

void main()
{
    vec2 position = ( gl_FragCoord.xy / resolution.xy );
    float colour = 0.0f;
    colour += sin( position.x * cos( time / 15.0 ) * 80.0 ) + cos( position.y * cos( time / 15.0 ) * 10.0 );
	colour += sin( position.y * sin( time / 10.0 ) * 40.0 ) + cos( position.x * sin( time / 25.0 ) * 40.0 );
	colour += sin( position.x * sin( time / 5.0 ) * 10.0 ) + sin( position.y * sin( time / 35.0 ) * 80.0 );
	colour *= sin( time / 10.0 ) * 0.5;
    FragColor = vec4( vec3( colour, colour * 0.5, sin( colour + time / 3.0 ) * 0.75 ), 1.0 );
} 