// PointSprite::geometryShaderText

uniform float size;
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
#ifdef WITH_PLANE
	uniform vec3 eyePos;
	VARYING_OUT vec4 eyePosFrag;
#endif

VARYING_OUT vec2 texCoord;
VARYING_OUT vec3 sphereCenter;
#ifdef WITH_COLOR_PER_VERTEX 
	VARYING_IN vec3 color[1];
	VARYING_OUT vec3 colorsprite;
#endif

#ifdef WITH_PLANE
	void corner( vec4 center, vec3 planeX, vec3 planeY, float x, float y)
	{
		texCoord = vec2(1.4*x,1.4*y);
		vec4 pos = center + size( x*vec4(planeX,0.0) + y*vec4(planeY,0.0)+ vec4(0.0,0.0,0.5,0.0)); 
		gl_Position = ProjectionMatrix *  pos;
		EmitVertex();
	}
#else
	void corner( vec4 center, float x, float y)
	{
		texCoord = vec2(1.4*x,1.4*y);
		vec4 pos = center + vec4( size*x, size*y, 0.0, 0.0);
		gl_Position = ProjectionMatrix *  pos;
		EmitVertex();
	}
#endif

void main()
{

#ifdef WITH_COLOR_PER_VERTEX 
	colorsprite = color[0];
#endif

	vec4 posCenter = ModelViewMatrix * POSITION_IN(0);

	sphereCenter = posCenter.xyz;

#ifdef WITH_PLANE
	vec4 EPF = ModelViewMatrix * vec4(eyePos,1.0);
	eyePosFrag = EPF.xyz;

	vec3 V = sphereCenter-eyePosFrag;
	V.normalize();
//	vec3 planeX = cross(V, vec3(0.0,1.0,0.0));
	vec3 planeX = vec3(-V[2],0.0,V[0]);
	vec3 planeY = cross(X,V);
	corner(posCenter, planeX, planeY, -1.0, 1.0);
	corner(posCenter, planeX, planeY, -1.0,-1.0);
	corner(posCenter, planeX, planeY,  1.0, 1.0);
	corner(posCenter, planeX, planeY,  1.0,-1.0);
#else
	corner(posCenter, -1.0, 1.0);
	corner(posCenter, -1.0,-1.0);
	corner(posCenter,  1.0, 1.0);
	corner(posCenter,  1.0,-1.0);
#endif	
	
	
	EndPrimitive();
}
