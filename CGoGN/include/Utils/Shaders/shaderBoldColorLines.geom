// ShaderBoldColorLines::geometryShaderText

uniform vec2 lineWidth;

VARYING_IN vec3 vcolor[];
VARYING_OUT vec3 fragColor;

VARYING_IN vec3 posClip[];
VARYING_OUT vec3 fragClip;

void main()
{

	vec3 A = POSITION_IN(0).xyz / abs(POSITION_IN(0).w);
	vec3 B = POSITION_IN(1).xyz / abs(POSITION_IN(1).w);

	vec3 U = normalize(B-A);
	vec3 V = cross(U,vec3(0.0,0.0,1.0));

	vec3 LW = vec3(lineWidth,1.0);
	fragClip = posClip[0];
	fragColor = vcolor[0];
	gl_Position = vec4(A-U*LW, 1.0);
	EmitVertex();
	gl_Position = vec4(A+V*LW, 1.0);
	EmitVertex();
	gl_Position = vec4(A-V*LW, 1.0);
	EmitVertex();
	fragColor = vcolor[1];
	fragClip = posClip[1];
	gl_Position = vec4(B+V*LW, 1.0);
	EmitVertex();
	gl_Position = vec4(B-V*LW, 1.0);
	EmitVertex();
	gl_Position = vec4(B+U*LW, 1.0);
	EmitVertex();

	EndPrimitive();
}
