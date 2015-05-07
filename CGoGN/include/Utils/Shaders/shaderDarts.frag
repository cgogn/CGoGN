// ShaderDarts::fragmentShaderText

PRECISION;
uniform vec4 lineColor;

FRAG_OUT_DEF;

void main()
{
	FRAG_OUT = lineColor;
}
