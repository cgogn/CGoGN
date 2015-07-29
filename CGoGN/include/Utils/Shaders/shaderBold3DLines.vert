// ShaderBold3DLines::vertexShaderText

ATTRIBUTE vec3 VertexPosition;

INVARIANT_POS;
void main ()
{
	posClip = VertexPosition;
	gl_Position = vec4 (VertexPosition, 1.0);
}
