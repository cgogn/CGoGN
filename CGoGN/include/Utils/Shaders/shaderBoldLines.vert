// ShaderBoldLines::vertexShaderText

ATTRIBUTE vec3 VertexPosition;
uniform mat4 ModelViewProjectionMatrix;
VARYING_VERT vec3 posClip;

INVARIANT_POS;
void main ()
{
	posClip = VertexPosition;
	gl_Position = ModelViewProjectionMatrix * vec4 (VertexPosition, 1.0);
}
