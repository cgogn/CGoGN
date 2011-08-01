//Strings3D::vertexShaderText

ATTRIBUTE vec4 VertexPosition;
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform vec3 strPos;
uniform float scale;
VARYING_VERT vec2 tex_coord;
INVARIANT_POS;
void main ()
{
	vec4 pos = ModelViewMatrix * vec4(strPos,1.0) + vec4(VertexPosition[0]*scale,VertexPosition[1]*scale,0.0,0.0);
	tex_coord = vec2(VertexPosition[2],VertexPosition[3]);
	gl_Position = ProjectionMatrix * pos;
}

