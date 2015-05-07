// ShaderBold3DColorLines::vertexShaderText

ATTRIBUTE vec3 VertexPosition;
ATTRIBUTE vec3 VertexColor;
VARYING_VERT vec3 vcolor;


INVARIANT_POS;
void main ()
{
	vcolor = VertexColor;
	gl_Position = vec4 (VertexPosition, 1.0);
}
