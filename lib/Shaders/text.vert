uniform vec3 strPos;
uniform float scale;

varying vec2 tex_coord;

void main()
{	

	vec4 pos = gl_ModelViewMatrix * vec4(strPos,1.0) + vec4(gl_Vertex[0]*scale,gl_Vertex[1]*scale,0.0,0.0);
	tex_coord = vec2(gl_Vertex[2],gl_Vertex[3]);
	gl_Position = gl_ProjectionMatrix * pos;
}
