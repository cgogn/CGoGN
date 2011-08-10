// ShaderSimpleColor::fragmentShaderText

PRECISON;
uniform vec4 color;
FRAG_OUT_DEF;
void main()
{
	gl_FragColor=color;
}
