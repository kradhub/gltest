#version 330 core

in vec4 vert_color;

/* The fragment shader shall set a color */
out vec4 color;

void main ()
{
  color = vert_color;
}
