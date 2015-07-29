// ShaderColorDarts::fragmentShaderText

PRECISION;

VARYING_FRAG vec3 fragColor;
uniform float alpha;

FRAG_OUT_DEF;

void main()
{
	FRAG_OUT = vec4(fragColor,alpha);
}
