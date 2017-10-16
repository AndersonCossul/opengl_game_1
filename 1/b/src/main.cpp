/******************************************************************************\
| OpenGL 4 Example Code.                                                       |
| Accompanies written series "Anton's OpenGL 4 Tutorials"                      |
| Email: anton at antongerdelan dot net                                        |
| First version 27 Jan 2014                                                    |
| Copyright Dr Anton Gerdelan, Trinity College Dublin, Ireland.                |
| See individual libraries for separate legal notices                          |
|******************************************************************************|
| Matrices and Vectors                                                         |
| Note: code discussed in previous tutorials is moved into gl_utils file       |
| On Apple don't forget to uncomment the version number hint in start_gl()     |
\******************************************************************************/
#include "gl_utils.h"		// utility functions discussed in earlier tutorials
#include "maths_funcs.h"
#include <GL/glew.h>		// include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define GL_LOG_FILE "gl.log"

// keep track of window size for things like the viewport and the mouse cursor
int g_gl_width = 640;
int g_gl_height = 480;
GLFWwindow *g_window = NULL;

int main() {
	restart_gl_log();
	// all the GLFW and GLEW start-up code is moved to here in gl_utils.cpp
	start_gl();
	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS);		 // depth-testing interprets a smaller value as "closer"

								 /* OTHER STUFF GOES HERE NEXT */
	GLfloat points[] = {
		0.0f,  0.5f,  0.0f,
		0.5f,  -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f };

	GLfloat colours[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f };

	GLuint points_vbo;
	glGenBuffers(1, &points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), points, GL_STATIC_DRAW);

	GLuint colours_vbo;
	glGenBuffers(1, &colours_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colours, GL_STATIC_DRAW);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	char vertex_shader[1024 * 256];
	char fragment_shader[1024 * 256];
	parse_file_into_str("test_vs.glsl", vertex_shader, 1024 * 256);
	parse_file_into_str("test_fs.glsl", fragment_shader, 1024 * 256);

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	const GLchar *p = (const GLchar *)vertex_shader;
	glShaderSource(vs, 1, &p, NULL);
	glCompileShader(vs);

	// check for compile errors
	int params = -1;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &params);
	if (GL_TRUE != params) {
		fprintf(stderr, "ERROR: GL shader index %i did not compile\n", vs);
		print_shader_info_log(vs);
		return 1; // or exit or something
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	p = (const GLchar *)fragment_shader;
	glShaderSource(fs, 1, &p, NULL);
	glCompileShader(fs);

	// check for compile errors
	glGetShaderiv(fs, GL_COMPILE_STATUS, &params);
	if (GL_TRUE != params) {
		fprintf(stderr, "ERROR: GL shader index %i did not compile\n", fs);
		print_shader_info_log(fs);
		return 1; // or exit or something
	}

	GLuint shader_programme = glCreateProgram();
	glAttachShader(shader_programme, fs);
	glAttachShader(shader_programme, vs);
	glLinkProgram(shader_programme);

	glGetProgramiv(shader_programme, GL_LINK_STATUS, &params);
	if (GL_TRUE != params) {
		fprintf(stderr, "ERROR: could not link shader programme GL index %i\n",
			shader_programme);
		print_programme_info_log(shader_programme);
		return false;
	}

	mat4 pmatrix = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	/*mat4 rmatrix = {
	cos(1), 0.0f, sin(1), 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	-sin(1), 0.0f, cos(1), 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f
	};*/

	mat4 rmatrix = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	mat4 smatrix = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	int pmatrix_location = glGetUniformLocation(shader_programme, "pmatrix");
	glUseProgram(shader_programme);
	glUniformMatrix4fv(pmatrix_location, 1, GL_FALSE, pmatrix.m);

	int smatrix_location = glGetUniformLocation(shader_programme, "smatrix");
	glUseProgram(shader_programme);
	glUniformMatrix4fv(smatrix_location, 1, GL_FALSE, smatrix.m);

	int rmatrix_location = glGetUniformLocation(shader_programme, "rmatrix");
	glUseProgram(shader_programme);
	glUniformMatrix4fv(rmatrix_location, 1, GL_FALSE, rmatrix.m);

	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK);		// cull back face
	glFrontFace(GL_CW);			// GL_CCW for counter clock-wise

	float speed = 1.0f; // move at 1 unit per second
	float last_position = 0.0f;
	float angle = 0.0f;
	float move = 0.0f;
	float scale = 0.0f;
	while (!glfwWindowShouldClose(g_window)) {
		// add a timer for doing animation
		static double previous_seconds = glfwGetTime();
		double current_seconds = glfwGetTime();
		double elapsed_seconds = current_seconds - previous_seconds;
		previous_seconds = current_seconds;

		_update_fps_counter(g_window);
		// wipe the drawing surface clear
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, g_gl_width, g_gl_height);

		//
		// Note: this call is not necessary, but I like to do it anyway before any
		// time that I call glDrawArrays() so I never use the wrong shader programme
		glUseProgram(shader_programme);

		// update the matrix
		// - you could simplify this by just using sin(current_seconds)

		//
		// Note: this call is related to the most recently 'used' shader programme
		glUniformMatrix4fv(pmatrix_location, 1, GL_FALSE, pmatrix.m);
		glUniformMatrix4fv(smatrix_location, 1, GL_FALSE, smatrix.m);
		glUniformMatrix4fv(rmatrix_location, 1, GL_FALSE, rmatrix.m);

		//
		// Note: this call is not necessary, but I like to do it anyway before any
		// time that I call glDrawArrays() so I never use the wrong vertex data
		glBindVertexArray(vao);
		// draw points 0-3 from the currently bound VAO with current in-use shader
		glDrawArrays(GL_TRIANGLES, 0, 3);
		// update other events like input handling
		glfwPollEvents();

		if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(g_window, 1);
		}
		else if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_LEFT)) {
			if (pmatrix.m[12] > -1.0) {
				pmatrix.m[12] -= 0.1;
			}
		}
		else if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_RIGHT)) {
			if (pmatrix.m[12] < 1.0) {
				pmatrix.m[12] += 0.1;
			}
		}
		else if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_DOWN)) {
			if (smatrix.m[0] > 0.1) {
				smatrix.m[0] -= 0.1;
				smatrix.m[5] -= 0.1;
				smatrix.m[10] -= 0.1;
			}
		}
		else if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_UP)) {
			if (smatrix.m[0] < 2.0) {
				smatrix.m[0] += 0.1;
				smatrix.m[5] += 0.1;
				smatrix.m[10] += 0.1;
			}
		}
		else if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_A)) {
			angle += 0.1f;
			if (angle >= 3.14f) {
				angle = -3.14f;
			}
			rmatrix.m[0] = cos(angle);
			rmatrix.m[1] = sin(angle);
			rmatrix.m[4] = -sin(angle);
			rmatrix.m[5] = cos(angle);
		}
		else if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_D)) {
			angle -= 0.1f;
			if (angle <= -3.14f) {
				angle = 3.14f;
			}
			rmatrix.m[0] = cos(angle);
			rmatrix.m[1] = sin(angle);
			rmatrix.m[4] = -sin(angle);
			rmatrix.m[5] = cos(angle);
		}
		// put the stuff we've been drawing onto the display
		glfwSwapBuffers(g_window);
	}

	// close GL context and any other GLFW resources
	glfwTerminate();
	return 0;
}