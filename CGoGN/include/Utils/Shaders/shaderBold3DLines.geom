// ShaderBold3DLines::geometryShaderText

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform float lineWidth;
uniform vec4 planeClip;

void main()
{
	float v0 = dot(planeClip,POSITION_IN(0));
	float v1 = dot(planeClip,POSITION_IN(1));

	vec4 A = vec4(0.0,0.0,0.0,0.0);
	vec4 B = vec4(0.0,0.0,0.0,0.0);

	if (v0 <= 0.0) //A not clipped
	{
		A =  ModelViewMatrix *POSITION_IN(0);
		if (v1 <= 0.0)
			B = ModelViewMatrix *POSITION_IN(1);
		else
			B = ModelViewMatrix * vec4(POSITION_IN(0).xyz * v1/(v1-v0) - POSITION_IN(1).xyz * v0/(v1-v0) ,1.0);	
	}
	else
	{
		if (v1 <= 0.0) //B not clipped
		{
			B = ModelViewMatrix *POSITION_IN(1);
			A = ModelViewMatrix * vec4(POSITION_IN(1).xyz * v0/(v0-v1) - POSITION_IN(0).xyz * v1/(v0-v1) ,1.0);		
		}
	}


	vec4 Ap = (ProjectionMatrix*A);
	vec4 Bp = (ProjectionMatrix*B);

	vec3 U3 = normalize((Bp.xyz/Bp.w - Ap.xyz/Ap.w));
	vec3 U = vec3(lineWidth*U3);
	vec3 V = vec3(lineWidth*normalize(vec2(U[1],-U[0])), 0.0);

	gl_Position = ProjectionMatrix*vec4(A.xyz-U, 1.0);
	EmitVertex();
	gl_Position = ProjectionMatrix*vec4(A.xyz+V, 1.0);
	EmitVertex();
	gl_Position = ProjectionMatrix*vec4(A.xyz-V, 1.0);
	EmitVertex();
	gl_Position = ProjectionMatrix*vec4(B.xyz+V, 1.0);
	EmitVertex();
	gl_Position = ProjectionMatrix*vec4(B.xyz-V, 1.0);
	EmitVertex();
	gl_Position = ProjectionMatrix*vec4(B.xyz+U, 1.0);
	EmitVertex();
	EndPrimitive();
}
