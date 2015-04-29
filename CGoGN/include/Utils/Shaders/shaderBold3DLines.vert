// ShaderBold3DLines::vertexShaderText

ATTRIBUTE vec3 VertexPosition;
uniform mat4 ModelViewMatrix;
VARYING_VERT vec3 posClip;

INVARIANT_POS;
void main ()
{
	posClip = VertexPosition;
	gl_Position = ModelViewMatrix * vec4 (VertexPosition, 1.0);
}
