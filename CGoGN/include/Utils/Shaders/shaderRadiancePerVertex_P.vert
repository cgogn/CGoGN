// ShaderRadiancePerVertex_P::vertexShaderText

ATTRIBUTE vec3 VertexPosition;
ATTRIBUTE vec3 VertexTangent;
ATTRIBUTE vec3 VertexNormal;
ATTRIBUTE vec3 VertexBiNormal;
ATTRIBUTE ivec2 VertexParam;
uniform mat4 ModelViewProjectionMatrix ;
uniform sampler2D texture;
uniform vec3 camera;
VARYING_VERT vec3 vxColor;
INVARIANT_POS;

float evaluate(float u, float v, int i)
{
	switch(i)
	{
	case 0 :
		return 1;
	case 1 :
		return u;
	case 2 :
		return v;
	case 3 :
		return u*u;
	case 4 :
		return u*v;
	case 5 :
		return v*v;
	case 6 :
		return u*u*u;
	case 7 :
		return u*u*v;
	case 8 :
		return u*v*v;
	case 9 :
		return v*v*v;
	case 10 :
		return u*u*u*u;
	case 11 :
		return u*u*u*v;
	case 12 :
		return u*u*v*v;
	case 13 :
		return u*v*v*v;
	case 14 :
		return v*v*v*v;
	default:
		return 0;
	}
}

void main ()
{
	int size = (textureSize(texture,0)).x; // supposed square matrix

	vec3 eyeV = normalize(camera - VertexPosition); // normalized outgoing line-of-sight vector
//	eyeV = 2*dot(VertexNormal,eyeV)*VertexNormal-eyeV ; // symmetrize

	vec2 uv = -1. * vec2(dot(eyeV,VertexTangent), dot(eyeV,VertexBiNormal));
	if (dot(eyeV,VertexNormal) < 0.)
	{
		uv = normalize(uv);
	}

	vxColor = vec3(0.,0.,0.) ;

	// evaluate function
	ivec2 param = VertexParam ;
	for(int l=0; l<15; l++)
	{
		// compute texture index
		if (param.y >= size) // if texture newline
		{
			param.y -= size ;
			param.x += 1 ;
		}

		// get corresponding coef
		vec3 coefLM = (texelFetch(texture,param,0)).rgb;

		// multiply by basis function
		vxColor += coefLM * evaluate(uv.x, uv.y, l);

		param.y ++ ;
	}

	// vxColor = eyeV ; // Debug camera position
	// vxColor = VertexNormal ; // Debug normals
	// vxColor = (VertexPosition+vec3(1.,1.,1.))/2.0 ; // Debug positions
	gl_Position = ModelViewProjectionMatrix * vec4 (VertexPosition, 1.0);
}
