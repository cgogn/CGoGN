// PointSprite::geometryShaderText

uniform float size;
uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
VARYING_OUT vec2 texCoord;
VARYING_OUT vec2 positionFragIn;
VARYING_OUT vec4 mvpFragIn;

#ifdef WITH_COLOR_PER_VERTEX 
	VARYING_IN vec3 color[1];
	VARYING_OUT vec3 colorsprite;
#endif

#ifdef WITH_PLANE
uniform vec3 planeX;
uniform vec3 planeY;
#endif

void main()
{

#ifdef WITH_COLOR_PER_VERTEX 
	colorsprite = color[0];
#endif

	mvpFragIn.x = ProjectionMatrix[2][2];
	mvpFragIn.y = ProjectionMatrix[3][2];
	mvpFragIn.z = ProjectionMatrix[2][3];
	mvpFragIn.w = ProjectionMatrix[3][3];
	
	vec4 posCenter = ModelViewMatrix * POSITION_IN(0);
	positionFragIn = posCenter.zw;

	texCoord = vec2(0.0,1.0);
	#ifdef WITH_PLANE
		vec4 pos = posCenter + vec4(-size, size, 0.0, 0.0);
	#else
		vec4 pos = posCenter - vec4(planeX,0.0) + vec4(planeY,0.0)
	#endif	
	gl_Position = ProjectionMatrix *  pos;
	EmitVertex();

	#ifdef WITH_PLANE
		pos = posCenter + vec4(-size, -size, 0.0, 0.0);
	#else
		pos = posCenter - vec4(planeX,0.0) - vec4(planeY,0.0)
	#endif	
	texCoord = vec2(0.0,0.0);
	gl_Position = ProjectionMatrix *  pos;
	EmitVertex();

	#ifdef WITH_PLANE
		pos = posCenter + vec4(size, size, 0.0, 0.0);
	#else
		pos = posCenter + vec4(planeX,0.0) + vec4(planeY,0.0)
	#endif	
	texCoord = vec2(1.0,1.0);
	gl_Position = ProjectionMatrix *  pos;
	EmitVertex();

	#ifdef WITH_PLANE
		pos = posCenter + vec4(size, -size, 0.0, 0.0);
	#else
		pos = posCenter + vec4(planeX,0.0) - vec4(planeY,0.0)
	#endif	
	texCoord = vec2(1.0,0.0);
	gl_Position = ProjectionMatrix *  pos;
	EmitVertex();
	EndPrimitive();
}
