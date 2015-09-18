#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>

/* 0: vertex buffer object
 * 1: element buffer object
 */
GLuint cube_bo[2];
GLuint cube_vao;

static void
cube_init (void)
{
  /* the first three values are position, the last three colour */
  GLfloat vertices[] = {
    /* front face (red) */
    -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,   /* bottom left */
     0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,   /* bottom right */
     0.5f,  0.5f, 0.5f, 1.0f, 0.0f, 0.0f,   /* top right */
    -0.5f,  0.5f, 0.5f, 1.0f, 0.0f, 0.0f,   /* top left */

    /* right face (green) */
    0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f,   /* bottom front */
    0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f,   /* top front */
    0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f,   /* bottom back */
    0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,   /* top back */

    /* back face (blue) */
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,   /* bottom left */
     0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,   /* bottom right */
     0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,   /* top right */
    -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f,   /* top left */

    /* left face (yellow) */
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.0f,   /* bottom front */
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.0f,   /* top front */
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f,   /* bottom back */
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f,   /* top top */

    /* top face (cyan) */
     0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f,   /* front right */
    -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f,   /* front left */
     0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 1.0f,   /* back right */
    -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 1.0f,   /* back left */

    /* bottom face (magenta) */
    -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f,   /* front left */
     0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f,   /* front right */
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f,   /* back left */
     0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f,   /* back right */
  };

  GLuint indices[] = {
    /* front face */
    0, 1, 2,
    0, 2, 3,

    /* right face */
    4, 5, 6,
    5, 6, 7,

    /* back face */
    8, 9, 10,
    8, 10, 11,

    /* left face */
    12, 13, 14,
    13, 14, 15,

    /* top face */
    16, 17, 18,
    17, 18, 19,

    /* bottom face */
    20, 21, 22,
    21, 22, 23,
  };

  glGenBuffers (2, cube_bo);
  glGenVertexArrays (1, &cube_vao);

  glBindVertexArray (cube_vao);

  /* copy our vertices and our indices to buffer object */
  glBindBuffer (GL_ARRAY_BUFFER, cube_bo[0]);
  glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, cube_bo[1]);
  glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (indices), indices,
      GL_STATIC_DRAW);

  /* set the layout of the vertex data in memory */
  glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof (GLfloat), NULL);
  glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof (GLfloat), (GLvoid *) (3 * sizeof (GLfloat)));

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glBindVertexArray (0);
}

static void
cube_deinit (void)
{
  glDeleteVertexArrays (1, &cube_vao);
  glDeleteBuffers (2, cube_bo);
}

static int
load_shader_from_file (const char * path, GLchar ** shader)
{
  FILE *f;
  struct stat st;
  GLchar *buf;
  size_t rbytes;

  if (stat (path, &st) < 0)
    return -1;

  f = fopen (path, "r");
  if (f == NULL)
    return -1;

  buf = malloc (st.st_size + 1);
  if (buf == NULL) {
    fprintf (stderr, "failed to allocated shader buffer");
    return -1;
  }

  rbytes = fread (buf, 1, st.st_size, f);
  if (rbytes < (size_t) st.st_size) {
    fprintf (stderr, "short read: %zu/%zd", rbytes, st.st_size);
    free (buf);
    return -1;
  }

  buf[st.st_size] = '\0';

  *shader = buf;
  return 0;
}

static GLuint
shader_init (const char * path, GLenum shader_type)
{
  GLchar *shader = NULL;
  GLint success;
  GLuint shader_id = 0;

  printf ("creating shader of type %d from file %s\n", shader_type, path);

  if (load_shader_from_file (path, &shader) < 0) {
    fprintf (stderr, "failed to load shader %s\n", path);
    goto beach;
  }

  shader_id = glCreateShader (shader_type);
  if (!shader_id) {
    fprintf (stderr, "failed to create shader\n");
    goto beach;
  }

  glShaderSource (shader_id, 1, (const GLchar **) &shader, NULL);
  glCompileShader (shader_id);
  glGetShaderiv (shader_id, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar info_log[512];
    glGetShaderInfoLog (shader_id, 512, NULL, info_log);
    fprintf (stderr, "failed to compile shader: %s\n", info_log);
    goto beach;
  }

  free (shader);

  return shader_id;

beach:
  if (shader)
    free (shader);

  if (shader_id)
    glDeleteShader (shader_id);

  return 0;
}

static GLuint
shader_program_create (GLuint vertex_shader_id, GLuint fragment_shader_id)
{
  GLint success;
  GLuint prog_id;

  prog_id = glCreateProgram ();
  if (!prog_id) {
    fprintf (stderr, "failed to create shader program\n");
    return 0;
  }

  glAttachShader (prog_id, vertex_shader_id);
  glAttachShader (prog_id, fragment_shader_id);
  glLinkProgram (prog_id);

  glGetProgramiv (prog_id, GL_LINK_STATUS, &success);
  if (!success) {
    GLchar info_log[512];
    glGetShaderInfoLog (prog_id, 512, NULL, info_log);
    fprintf (stderr, "failed to link program: %s", info_log);
    glDeleteProgram (prog_id);
    return 0;
  }

  return prog_id;
}

static void
render (GLuint shader_prog)
{
  GLint angle_var;
  GLfloat angle;
  GLfloat time;

  /* clear screen */
  glClearColor (0.2f, 0.3f, 0.3f, 1.0f);
  glClear (GL_COLOR_BUFFER_BIT);

  glUseProgram (shader_prog);
  angle_var = glGetUniformLocation (shader_prog, "angle");
  if (angle_var < 0)
    return;

  time = glfwGetTime ();
  angle = sin (time / 5.0) * M_PI;

  printf ("angle is %f\n", angle);
  glUniform1f (angle_var, angle);

  glBindVertexArray (cube_vao);
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  //glDrawArrays (GL_TRIANGLES, 0, 6 * 4);
  glDrawElements (GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, 0);

  glBindVertexArray (0);
}

static void
win_key_callback (GLFWwindow * win, int key, int scancode, int action, int mode)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose (win, GL_TRUE);
}

int
main (int argc, char *argv[])
{
  int ret = 1;
  GLFWwindow *win;
  GLuint vertex_shader = 0;
  GLuint frag_shader = 0;
  GLuint shader_prog = 0;

  glfwInit ();
  glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint (GLFW_RESIZABLE, GL_FALSE);

  /* create window */
  win = glfwCreateWindow (800, 600, "OpenGL test", NULL, NULL);
  if (win == NULL) {
    fprintf (stderr, "failed to create window\n");
    glfwTerminate ();
    return 1;
  }

  glfwMakeContextCurrent (win);
  glfwSetKeyCallback (win, win_key_callback);

  glewExperimental = GL_TRUE;
  if (glewInit () != GLEW_OK) {
    fprintf (stderr, "failed to init glew\n");
    return 1;
  }

  /* set the rendering window position and dimension */
  glViewport (0, 0, 800, 600);

  vertex_shader = shader_init ("vertex_shader.vert", GL_VERTEX_SHADER);
  if (!vertex_shader)
    goto end;

  frag_shader = shader_init ("fragment_shader.frag", GL_FRAGMENT_SHADER);
  if (!frag_shader)
    goto end;

  shader_prog = shader_program_create (vertex_shader, frag_shader);
  if (!shader_prog)
    goto end;

  cube_init ();

  while (!glfwWindowShouldClose (win)) {
    glfwPollEvents ();

    render (shader_prog);

    glfwSwapBuffers (win);
  }

  ret = 0;

  cube_deinit ();

end:
  if (frag_shader)
    glDeleteShader (frag_shader);

  if (vertex_shader)
    glDeleteShader (vertex_shader);

  if (shader_prog)
    glDeleteProgram (shader_prog);


  glfwTerminate ();

  return ret;
}
