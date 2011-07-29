// ShaderColorPerVertex::fragmentShaderText

PRECISON;
VARYING_FRAG vec3 color;
FRAG_OUT_DEF;
void main()
{
	gl_FragColor=vec4(color,0.0);
}
