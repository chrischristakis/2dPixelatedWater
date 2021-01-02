#version 330 core
out vec4 fragCol;

uniform vec3 aCol = vec3(33.0f/255, 179.0f/255, 251.0/255);

void main()
{
	fragCol = vec4(aCol, 1.0);
}