#version 330 core

/* the position variable has attribute position 0 */
layout (location = 0) in vec3 position;

/* our vertex shader output a color vector */
out vec4 vert_color;

void main ()
{
  /* Vertex shader shall set the 'global' gl_Position variable */

  /* we can directly build vector from other */
  gl_Position = vec4 (position.x, position.y, position.z, 1.0);
  gl_Position = vec4 (position, 1.0);

  vert_color = vec4 (0.4f, 0.0f, 0.0f, 1.0f);
}
