#version 330 core
out vec4 fragCol;

in vec2 pos;
uniform vec3 aCol = vec3(33.0f/255, 179.0f/255, 251.0/255);

void main()
{
	//if(pos.y > 560)
	//	fragCol = vec4(1.0);
	//else
	fragCol = vec4(aCol, 1.0);
}