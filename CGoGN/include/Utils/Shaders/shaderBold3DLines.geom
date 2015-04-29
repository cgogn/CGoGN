// ShaderBold3DLines::geometryShaderText

uniform mat4 ProjectionMatrix;
uniform float lineWidth;
VARYING_IN vec3 posClip[];
VARYING_OUT vec3 fragClip;

void main()
{
	vec4 Ap = (ProjectionMatrix*POSITION_IN(0));
	vec4 Bp = (ProjectionMatrix*POSITION_IN(1));

	vec3 U3 = normalize((Bp.xyz/Bp.w - Ap.xyz/Ap.w));
	vec3 U = vec3(lineWidth*U3);
	vec3 V = vec3(lineWidth*normalize(vec2(U[1],-U[0])), 0.0);

	fragClip = posClip[0];
	gl_Position = ProjectionMatrix*vec4(POSITION_IN(0).xyz-U, 1.0);
	EmitVertex();
	gl_Position = ProjectionMatrix*vec4(POSITION_IN(0).xyz+V, 1.0);
	EmitVertex();
	gl_Position = ProjectionMatrix*vec4(POSITION_IN(0).xyz-V, 1.0);
	EmitVertex();
	fragClip = posClip[1];
	gl_Position = ProjectionMatrix*vec4(POSITION_IN(1).xyz+V, 1.0);
	EmitVertex();
	gl_Position = ProjectionMatrix*vec4(POSITION_IN(1).xyz-V, 1.0);
	EmitVertex();
	gl_Position = ProjectionMatrix*vec4(POSITION_IN(1).xyz+U, 1.0);
	EmitVertex();
	EndPrimitive();
}
