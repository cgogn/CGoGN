// ShaderExplodeVolumes::geometryShaderText
uniform float explodeV;
uniform mat4 ModelViewProjectionMatrix;
uniform mat4 NormalMatrix;
uniform mat4 ModelViewMatrix;
uniform vec3 lightPosition;
uniform vec4 diffuse;
uniform vec4 ambient;
uniform vec4 plane;
VARYING_OUT vec4 ColorFS;

void main(void)
{
	float d = dot(plane,POSITION_IN(0));
	
	if (d<=0.0)
	{
		vec3 v1 = POSITION_IN(2).xyz - POSITION_IN(1).xyz;
		vec3 v2 = POSITION_IN(3).xyz - POSITION_IN(1).xyz;
		vec3 N  = cross(v1,v2);
		N  =  normalize (vec3(NormalMatrix*vec4(N,0.0))); 
		
	// compute face center & lighting informations
		vec4 newPos =  ModelViewMatrix * POSITION_IN(1);
		vec3 L =  normalize (lightPosition - newPos.xyz);
		float lambertTerm = dot(N,L);
		ColorFS = ambient;
		
		if (lambertTerm > 0.0)
#ifdef WITH_COLORPF	
			ColorFS += vec4(POSITION_IN(4).xyz,1.0) * lambertTerm;
#else
			ColorFS += diffuse * lambertTerm;		
#endif 
		
	// Explode in face 	
		for (int i=1; i<=3; i++)
		{
			vec4 P = explodeV * POSITION_IN(i) + (1.0-explodeV)* POSITION_IN(0);
			gl_Position = ModelViewProjectionMatrix *  P;
			EmitVertex();
		}
		EndPrimitive();
	}

	
}
