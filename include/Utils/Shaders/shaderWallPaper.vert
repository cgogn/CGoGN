// ShaderWallPaper::vertexShaderText

ATTRIBUTE vec3 VertexPosition;
ATTRIBUTE vec2 VertexTexCoord;
VARYING_VERT vec2 texCoord;
INVARIANT_POS;
void main ()
{
	gl_Position = vec4 (VertexPosition, 1.0);
	texCoord = VertexTexCoord;
}

