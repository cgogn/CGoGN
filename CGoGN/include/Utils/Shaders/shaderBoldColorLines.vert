// ShaderBoldColorLines::vertexShaderText

ATTRIBUTE vec3 VertexPosition;
ATTRIBUTE vec3 VertexColor;
uniform mat4 ModelViewMatrix;
VARYING_VERT vec3 vcolor;
VARYING_VERT vec3 posClip;

INVARIANT_POS;
void main ()
{
	posClip = VertexPosition;
	vcolor = VertexColor;
	gl_Position = ModelViewMatrix * vec4 (VertexPosition, 1.0);
}
