// ShaderBoldColorLines::geometryShaderText

uniform vec2 lineWidths;
uniform mat4 ProjectionMatrix;

VARYING_IN vec3 vcolor[];
VARYING_OUT vec3 fragColor;

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
		
		vec2 U2 = normalize(vec2(lineWidths[1],lineWidths[0])*(B.xy - A.xy));
		vec2 LWCorr =lineWidths * max(abs(U2.x),abs(U2.y));
		vec3 U = vec3(LWCorr*U2,0.0);
		vec3 V = vec3(LWCorr*vec2(U2[1], -U2[0]), 0.0);

		fragClip = posClip[0];
		fragColor = vcolor[0];
		gl_Position = vec4(A.xyz-U, 1.0);
		EmitVertex();
		gl_Position = vec4(A.xyz+V, 1.0);
		EmitVertex();
		gl_Position = vec4(A.xyz-V, 1.0);
		EmitVertex();

		fragClip = posClip[1];
		fragColor = vcolor[1];
		gl_Position = vec4(B.xyz+V, 1.0);
		EmitVertex();
		gl_Position = vec4(B.xyz-V, 1.0);
		EmitVertex();
		gl_Position = vec4(B.xyz+U, 1.0);
		EmitVertex();
		
		EndPrimitive();
	}
}
