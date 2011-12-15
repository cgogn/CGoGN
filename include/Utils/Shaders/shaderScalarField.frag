// ShaderScalarField::fragmentShaderText

PRECISON;
VARYING_FRAG vec3 color;
VARYING_FRAG float scalar;
FRAG_OUT_DEF;
void main()
{
	float s = scalar * 30.0;
	if( s - floor(s) <= 0.01 )
		gl_FragColor = vec4(0.0); 
	else
		gl_FragColor = vec4(color, 0.0);
}
