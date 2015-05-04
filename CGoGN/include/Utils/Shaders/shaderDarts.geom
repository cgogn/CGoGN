// ShaderDarts::geometryShaderText

uniform mat4 ProjectionMatrix;
uniform vec2 lineWidths;
VARYING_IN vec3 posClip[];
VARYING_OUT vec3 fragClip;


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
		vec3 V = vec3(lineWidths*vec2(U2[1], -U2[0]), 0.0)*3.0;

		U2 *= (lineWidths[0]+lineWidths[1])/2.0;
		vec3 U = vec3(U2,0.0)*3.0;
		
//		vec3 U3 = normalize(B.xyz - A.xyz);
//		U3 *= (lineWidths[0]+lineWidths[1])/2.0;
		
		fragClip = posClip[0];
		
		gl_Position = vec4(A.xyz-U+V, 1.0);
		EmitVertex();
		gl_Position = vec4(A.xyz-U-V, 1.0);
		EmitVertex();
		gl_Position = vec4(A.xyz+U+V, 1.0);
		EmitVertex();
		gl_Position = vec4(A.xyz+U-V, 1.0);
		EmitVertex();
		
		V /= 3.0;
		
		gl_Position = vec4(A.xyz+V, 1.0);
		EmitVertex();
		gl_Position = vec4(A.xyz-V, 1.0);
		EmitVertex();
		
		fragClip = posClip[1];
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