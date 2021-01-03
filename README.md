# WaterShader
The goal of this project is to simulate a pixelated wave in OpenGL and C++ using post processing effects.
<img src=demogif.gif width="550"/>

### What's left?
A few things, but most won't be done until I implement this in a game. The goal of this project was to achieve the pixel effect, which was done. I feel
it would be beneficial to implement the following changes when I plan on adding this water to an actual game, since it'll jog my memory on how I did this
in the first place and won't clutter the code with litte addition which could be added later that aren't related ot the base product.

Anyways, the things to add:
<ul>
  <li>Add white line to edge of water to make it look a little more real</li>
  <li>Fix the math in the vertex shader, the waves look a little too sinusodal right now, make them look more chaotic and sharp.</li>
  <li>Add dynamic splashes where something interacts with the water using hooke's law.</li>
  <li>Transparency to water so you could see things behind it</li>
  <li>Maybe distort any objects behind the water? This one sounds a little tough but could definitely make it look really good.</li>
</ul>
