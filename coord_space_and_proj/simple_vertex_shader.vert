#version 330 core

/* the position variable has attribute position 0 */
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 tex_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

/* our vertex shader output a color vector */
out vec4 vert_color;

out vec2 tex_coordo;

void main ()
{
  /* Vertex shader shall set the 'global' gl_Position variable */
  gl_Position = projection * view * model * vec4 (position, 1.0);

  vert_color = vec4 (color, 1.0f);
  tex_coordo = tex_coord;
}
