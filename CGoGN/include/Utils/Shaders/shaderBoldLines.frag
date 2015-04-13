// ShaderBoldLines::fragmentShaderText

PRECISION;
uniform vec4 lineColor;
uniform vec4 planeClip;
VARYING_FRAG vec3 fragClip;
FRAG_OUT_DEF;

void main()
{
	if (dot(planeClip,vec4(fragClip,1.0))>0.0)
		discard;
	FRAG_OUT = lineColor;
}
