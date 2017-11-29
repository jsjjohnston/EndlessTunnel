#version 410

uniform float time;

in vec2 fTexCoord;

out vec4 fragColour;

uniform sampler2D target;

// just sample the target and return the colour
vec4 Simple() {

	vec2 position = ( gl_FragCoord.xy / fTexCoord.xy ) / 4.0;

	vec4 pixelColour = texture(target, fTexCoord);
	vec4 colours[3];
	colours[0] = vec4(sin( position.x * cos( time / 15.0 ) * 80.0 ),0.0,1.0,1.0);
	colours[1] = vec4(1.0,1.0,0.0,1.0);
	colours[2] = vec4(1.0,0.0,0.0,1.0);
	
	float lum = (pixelColour.r+pixelColour.g+pixelColour.b) /3;
	int ix = (lum < 0.5f)? 0:1;
	
	return mix(colours[ix],colours[ix+1],(lum-float(ix)*0.5)/0.5);
}

void main()
{
	fragColour = Simple();
}