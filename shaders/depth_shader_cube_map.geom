#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 lightSpaceMatrices[6];

in vec2 TextureCoords[];

out VS_OUT
{
	vec4 FragPos;
	vec2 TextureCoords;
}gs_out;

void main()
{
	//	faces
	for (int i = 0; i < 6; i++)
	{
		gl_Layer = i;
		//	Triangle vertices
		for (int j = 0; j < 3; j++)
		{
			gs_out.FragPos = gl_in[j].gl_Position;
			gl_Position = lightSpaceMatrices[i] * gs_out.FragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}