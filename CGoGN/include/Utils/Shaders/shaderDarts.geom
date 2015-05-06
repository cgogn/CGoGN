// ShaderDarts::geometryShaderText

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform vec2 lineWidths;
uniform vec4 planeClip;

void main()
{
	float v0 = dot(planeClip,POSITION_IN(0));
	float v1 = dot(planeClip,POSITION_IN(1));
	float scalePoint = 3.0;

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
			scalePoint = 1.0;
		}
	}
	
	float nearZ = 1.0;
	if (ProjectionMatrix[2][2] !=  1.0)
		nearZ = - ProjectionMatrix[3][2] / (ProjectionMatrix[2][2] - 1.0); 

	if ((A.z < nearZ) || (B.z < nearZ))
	{
		if (A.z >= nearZ)
			A = B + (A-B)*(nearZ-B.z)/(A.z-B.z);
		if (B.z >= nearZ)
			B = A + (B-A)*(nearZ-A.z)/(B.z-A.z);

		A = ProjectionMatrix*A;
		B = ProjectionMatrix*B;
		A = A/A.w;
		B = B/B.w;
		
		vec2 U2 = normalize(vec2(lineWidths[1],lineWidths[0])*(B.xy - A.xy));
		vec3 U = vec3(lineWidths*U2,0.0)*scalePoint;
		vec3 V = vec3(lineWidths*vec2(U2[1], -U2[0]), 0.0) * scalePoint;
		
		gl_Position = vec4(A.xyz-U+V, 1.0);
		EmitVertex();
		gl_Position = vec4(A.xyz-U-V, 1.0);
		EmitVertex();
		gl_Position = vec4(A.xyz+U+V, 1.0);
		EmitVertex();
		gl_Position = vec4(A.xyz+U-V, 1.0);
		EmitVertex();
		
		V /= scalePoint;
		
		gl_Position = vec4(A.xyz+V, 1.0);
		EmitVertex();
		gl_Position = vec4(A.xyz-V, 1.0);
		EmitVertex();
		
		gl_Position = vec4(B.xyz+V, 1.0);
		EmitVertex();
		gl_Position = vec4(B.xyz-V, 1.0);
		EmitVertex();
		
		EndPrimitive();	
	}
}

/*

void main()
{
	vec4 A = POSITION_IN(0);
	vec4 B = POSITION_IN(1);
	
	float nearZ = 1.0;
	if (ProjectionMatrix[2][2] !=  1.0)
		nearZ = - ProjectionMatrix[3][2] / (ProjectionMatrix[2][2] - 1.0); 

	if ((A.z < nearZ) || (B.z < nearZ))
	{
		if (A.z >= nearZ)
			A = B + (A-B)*(nearZ-B.z)/(A.z-B.z);
		if (B.z >= nearZ)
			B = A + (B-A)*(nearZ-A.z)/(B.z-A.z);

		A = ProjectionMatrix*A;
		B = ProjectionMatrix*B;
		
		A = A/A.w;
		B = B/B.w;
		vec2 U2 = normalize((B.xyz - A.xyz).xy);
		vec3 V = vec3(lineWidths*vec2(U2[1], -U2[0]), 0.0);
		vec3 U3 = normalize(B.xyz - A.xyz);
		U3 *= (lineWidths[0]+lineWidths[1])/2.0;

		fragClip = posClip[0];
		A -= vec4(3.0*U3,0.0);
		gl_Position = vec4(A.xyz+V, 1.0);
		EmitVertex();
		gl_Position = vec4(A.xyz-V, 1.0);
		EmitVertex();
		
		A += vec4(2.0*U3,0.0);
		gl_Position = vec4(A.xyz+3.0*V, 1.0);
		EmitVertex();
		gl_Position = vec4(A.xyz-3.0*V, 1.0);
		EmitVertex();

		A += vec4(2.0*U3,0.0);
		gl_Position = vec4(A.xyz+3.0*V, 1.0);
		EmitVertex();
		gl_Position = vec4(A.xyz-3.0*V, 1.0);
		EmitVertex();

		A += vec4(2.0*U3,0.0);
		gl_Position = vec4(A.xyz+V, 1.0);
		EmitVertex();
		gl_Position = vec4(A.xyz-V, 1.0);
		EmitVertex();
		
		fragClip = posClip[1];
		B += vec4(U3,0.0);
		gl_Position = vec4(B.xyz+V, 1.0);
		EmitVertex();
		gl_Position = vec4(B.xyz-V, 1.0);
		EmitVertex();
		
		EndPrimitive();	
	}
}

*/