#version 330 core

/* import color defined in vertex shader by declaring it as input */
in vec4 vert_color;

/* The fragment shader shall set a color */
out vec4 color;

void main ()
{
  color = vert_color;
}
