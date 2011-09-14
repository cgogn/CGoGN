// ShaderScalarField::vertexShaderText

ATTRIBUTE vec3 VertexPosition;
ATTRIBUTE float VertexScalar;
uniform mat4 ModelViewProjectionMatrix;
VARYING_VERT vec3 color;
VARYING_VERT float scalar;
INVARIANT_POS;

vec3 color_map_blue_white_red(float x)
{
	vec3 c = vec3(0);
	if (x < 0.0)
		c.b = 1.0;
	else if (x < 0.5)
	{
		c.r = 2.0 * x;
		c.g = 2.0 * x;
		c.b = 1.0;
	}
	else if (x < 1.0)
	{
		c.r = 1.0;
		c.g = 2.0 - 2.0 * x;
		c.b = 2.0 - 2.0 * x;
	}
	else
		c.r = 1.0;
	return c;
}

void main ()
{
	gl_Position = ModelViewProjectionMatrix * vec4 (VertexPosition, 1.0);
	color = color_map_blue_white_red(VertexScalar);
	scalar = VertexScalar;
}
