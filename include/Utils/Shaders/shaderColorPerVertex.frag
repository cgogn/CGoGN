// ShaderColorPerVertex::fragmentShaderText

PRECISON;
VARYING_FRAG vec3 color;
FRAG_OUT_DEF;
void main()
{
#ifdef BLACK_TRANSPARENCY
	if (dot(color,color) == 0.0)
		discard;
#endif
	gl_FragColor=vec4(color,0.0);
}
