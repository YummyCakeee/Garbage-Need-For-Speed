#version 450 core
out vec4 outColor;

void main()
{
	gl_FragDepth = gl_FragCoord.z;
}