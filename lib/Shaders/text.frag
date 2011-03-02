varying vec2 tex_coord;
uniform sampler2D FontTexture;
uniform vec3 color;

void main (void)
{
	float lum = texture2D(FontTexture, tex_coord).s;
	gl_FragColor = vec4(color,0.0)*lum;
}
