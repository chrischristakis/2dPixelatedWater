#version 330 core
#define PI 3.14159
layout(location = 0) in vec2 position;
layout(location = 1) in float wave;

out vec2 pos;
uniform mat4 mvp = mat4(1.0);
uniform float time = 0;
uniform float disturbance = 0;
uniform float wave_height;

uniform float window_width = 0.0;
uniform float water_height;

void main()
{
	//wave effect
	vec2 wavePos = position;
	if(wave == 1.0) //If the vertex is a wave.
		//It is shifted down by 1 because otherwise the wave would go over its normal height by 0.5.
		wavePos.y+=wave_height*( sin((position.x/window_width * disturbance)*PI + time) - 1
								-0.7*sin((position.x/window_width * disturbance*0.3)*PI + time*1.3) - 1
								+1.2*sin((position.x/window_width * disturbance*0.57)*PI + time*0.112) - 1
								+2.6*cos((position.x/window_width * disturbance*1.823)*PI + time*0.7) - 1);
	wavePos.y = max(wavePos.y, position.y-water_height);
	gl_Position = mvp * vec4(wavePos, 1.0, 1.0);
	pos = position;
}