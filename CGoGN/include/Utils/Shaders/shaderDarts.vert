// ShaderDarts::vertexShaderText

ATTRIBUTE vec3 VertexPosition;

INVARIANT_POS;
void main ()
{
	gl_Position = vec4 (VertexPosition, 1.0);
}
