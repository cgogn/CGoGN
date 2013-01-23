// ShaderExplodeSmoothVolumes::fragmentShaderText

uniform vec4 ambient;
uniform vec4 backColor;

VARYING_FRAG vec4 normalFS;
VARYING_FRAG vec4 lightFS;
VARYING_FRAG vec3 colorVert;


void main()
{
	float lambertTerm = dot(normalize(normalFS),normalize(lightFS));
	if (lambertTerm > 0.0)
		gl_FragColor = ambient + vec4(colorVert*lambertTerm, 1.0);
	else
		gl_FragColor = ambient - backColor*lambertTerm;

}
