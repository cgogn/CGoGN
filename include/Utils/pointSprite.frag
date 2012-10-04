// PointSprite::fragmentShaderText

uniform sampler2D SpriteTexture;
uniform float size;

#ifdef WITH_COLOR_PER_VERTEX 
	VARYING_FRAG vec3 colorsprite;
#else
	uniform vec3 colorsprite;
#endif


VARYING_FRAG vec2 texCoord;
VARYING_FRAG vec2 positionFragIn;
VARYING_FRAG vec4 mvpFragIn;
void main(void)
{
	float lum = texture2D(SpriteTexture, texCoord).s;
	if (lum==0.0)
		discard;
	vec2 v = texCoord-vec2(0.5,0.5);
	float z = size * sqrt(1.0-dot(v,v));
	vec2 zfrag = positionFragIn + vec2(z,0.0);
	gl_FragDepth = 0.5 + 0.5 * dot(zfrag, mvpFragIn.xy) / dot(zfrag, mvpFragIn.zw);
	gl_FragColor = vec4(colorsprite,0.0)*lum;
}
