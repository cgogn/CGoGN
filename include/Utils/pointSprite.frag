// PointSprite::fragmentShaderText

uniform mat4 ProjectionMatrix;
uniform vec3 lightPos;
uniform vec3 ambiant;
uniform float size;
#ifdef WITH_PLANE
	uniform vec3 eyePos;
	uniform vec3 eyeY;
	VARYING_FRAG vec3 eyePosFrag;
#endif


#ifdef WITH_COLOR_PER_VERTEX 
	VARYING_FRAG vec3 colorsprite;
#else
	uniform vec3 colorsprite;
#endif


VARYING_FRAG vec2 texCoord;
VARYING_FRAG vec3 sphereCenter;



void main(void)
{

	vec3 billboard_frag_pos = sphereCenter + vec3(texCoord, 0.0) * size;
#ifdef WITH_PLANE
	vec3 ray_direction = normalize(billboard_frag_pos - eyePosFrag;);
#else
	vec3 ray_direction = normalize(billboard_frag_pos);
#endif

	float TD = -dot(ray_direction,sphereCenter);
	float c = dot(sphereCenter, sphereCenter) - size * size;
	float arg = TD * TD - c;
      
	if (arg < 0.0)
		discard;
  
	float t = -c / (TD - sqrt(arg));
	vec3 frag_position_eye = ray_direction * t;
	vec4 pos = ProjectionMatrix * vec4(frag_position_eye, 1.0);
	gl_FragDepth = (pos.z / pos.w + 1.0) / 2.0;

	vec3 N = normalize(frag_position_eye - sphereCenter);
	vec3 L = normalize (lightPos - frag_position_eye);
	float lambertTerm = dot(N,L);
	
	gl_FragColor = vec4(colorsprite*lambertTerm + ambiant ,1.0);

}
