// ShaderBoldLines::geometryShaderText

uniform float lineWidth;
VARYING_IN vec3 posClip[];
VARYING_OUT vec3 fragClip;

void main()
{
	vec3 A = POSITION_IN(0).xyz / POSITION_IN(0).w;
	vec3 B = POSITION_IN(1).xyz / POSITION_IN(1).w;

	vec3 U = normalize(B-A);
	vec3 V = cross(U,vec3(0.0,0.0,1.0));

	fragClip = posClip[0];
	gl_Position = vec4(A-U*lineWidth, 1.0);
	EmitVertex();
	gl_Position = vec4(A+V*lineWidth, 1.0);
	EmitVertex();
	gl_Position = vec4(A-V*lineWidth, 1.0);
	EmitVertex();
	fragClip = posClip[1];
	gl_Position = vec4(B+V*lineWidth, 1.0);
	EmitVertex();
	gl_Position = vec4(B-V*lineWidth, 1.0);
	EmitVertex();
	gl_Position = vec4(B+U*lineWidth, 1.0);
	EmitVertex();
	EndPrimitive();
}
