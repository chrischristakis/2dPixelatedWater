#version 330 core
in vec2 texCoords;
out vec4 fragCol;

uniform sampler2D tex;

void main() 
{
	vec3 texCol = texture(tex, texCoords).rgb;
	fragCol = vec4(texCol, 1.0);
}
