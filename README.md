# WaterShader
The goal of this project is to simulate a pixelated wave in OpenGL and C++ using post processing effects.
<img src=demogif.gif width="550"/>

### What's left?
<ul>
  <li>Add white line to edge of water to make it look a little more real</li>
  <li>Fix the math in the vertex shader, the waves look a little too sinusodal right now, make them look more chaotic and sharp.</li>
  <li>Add dynamic splashes where something interacts with the water using hooke's law.</li>
  <li>Transparency to water so you could see things behind it</li>
  <li>Maybe distort any objects behind the water? This one sounds a little tough but could definitely make it look really good.</li>
</ul>

### Where's the executable?
Still working out the semantics of how VS2019 deploys C++ applications from the IDE, but once that's figured out I'll try and release a stable build
