#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in float wave;

uniform mat4 mvp = mat4(1.0);

void main()
{
	gl_Position = mvp * vec4(position, 1.0, 1.0);
}