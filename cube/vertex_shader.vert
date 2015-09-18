#version 330 core

/* the position variable has attribute position 0 */
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

uniform float angle;
out vec4 vert_color;

void main ()
{
  mat3 rot_x;
  mat3 rot_y;

  /* rotate around the y-axis matrice */
  rot_y[0][0] = cos (angle);
  rot_y[0][1] = 0;
  rot_y[0][2] = sin (angle);
  rot_y[1][0] = 0;
  rot_y[1][1] = 1;
  rot_y[1][2] = 0;
  rot_y[2][0] = -sin (angle);
  rot_y[2][1] = 0;
  rot_y[2][2] = cos (angle);

  rot_x[0][0] = 1;
  rot_x[0][1] = 0;
  rot_x[0][2] = 0;
  rot_x[1][0] = 0;
  rot_x[1][1] = cos (angle);
  rot_x[1][2] = sin (angle);
  rot_x[2][0] = 0;
  rot_x[2][1] = -sin (angle);
  rot_x[2][2] = cos (angle);

  gl_Position = vec4 (rot_x * rot_y * position, 1.0f);
  vert_color = vec4 (color, 1.0f);
}
