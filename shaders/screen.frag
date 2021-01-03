#version 330 core
in vec2 texCoords;
out vec4 fragCol;

uniform sampler2D tex;

void main() 
{
	fragCol = vec4(texture(tex, texCoords).rgb, 1.0);
}
