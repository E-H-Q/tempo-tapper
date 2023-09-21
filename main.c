// BPM tapper: made by EHQ
#include <stdio.h>
#include <time.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <string.h>
#include <math.h>

#include "src/draw.c"
#include "src/font.h"

#define size 25 //size of the "blips"
#define WINDOW_WIDTH 200
#define WINDOW_HEIGHT 200

int clicks = 0;
int sig = 4; // time signature
float bpm;

void makeRasterFont(void) {
	GLuint i, j;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	fontOffset = glGenLists (128);
	for (i = 0,j = 'A'; i < 26; i++,j++) { // letters
		glNewList(fontOffset + j, GL_COMPILE);
		glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, letters[i]); // width, height, xorig, yorig, kerning(?), angle(?)
		glEndList();
	}
	for (i = 0, j = '0'; i < 10; i++, j++) { // numbers
		glNewList(fontOffset + j, GL_COMPILE);
		glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, numbers[i]);
		glEndList();
	}
	glNewList(fontOffset + ' ', GL_COMPILE); // space
	glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, space);
	glEndList();

	glNewList(fontOffset + '.', GL_COMPILE); // period
	glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, period);
	glEndList();
}

void printString(char *s, int x, int y) {
	glRasterPos2i(x, y);
	glPushAttrib (GL_LIST_BIT);
	glListBase(fontOffset);
	glCallLists((GLsizei) strlen(s), GL_UNSIGNED_BYTE, (GLubyte *) s);
	glPopAttrib();
}

void render() {
	glClearColor(0.8f, 0.79f, 0.54f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(0.22f, 0.58f, 0.22f);
	// outlines
	for (int i = 0; i < sig; i++) {
		rect((i * size) + 5 + (i * 5), 5, (i * size) + 5 + (i * 5) + size, 5 + size);
	}
	// fills
	for (int i = 0; i < clicks; i++) {
		square((i * size) + 5 + (i * 5), 5, size);
	}

	float black[3] = {0, 0, 0}; // text color def.
	glColor3fv(black); // text color call

	char buf[100];
	if (clicks == 0) {
		char buf[100] = "...";
		bpm = 0;
	} else {
		if (!isinf(bpm)) {
			gcvt(bpm, 5, buf);
		}
	}
	printString(buf, (WINDOW_WIDTH / 2) - (4 * 8), WINDOW_HEIGHT / 2); // text, X, Y
	// scary hardcoded vars here ^^^
	printString("BPM", (WINDOW_WIDTH / 2) - (4 * 8), WINDOW_HEIGHT / 2 - 20);

	glFlush();
}

int main() {
	// Inititalize GLFW
	if (!glfwInit()) {
		return -1;
	}
	// Define the window
	GLFWwindow* window;
	// Create a windowed mode window & its OpenGL context
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "BMP tapper", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	// Set up the "camera" view
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Create the canvas to draw on
	glOrtho(0.0, WINDOW_WIDTH, 0.0, WINDOW_HEIGHT, 0.0, 1.0);

	if (!window) {
		glfwTerminate();
		return -1;
	}
	// "focuses" the window
	glfwMakeContextCurrent(window);

	// Font rendering setup
	makeRasterFont();

	float *times;
	times = malloc(sig * sizeof(float));

	// MAIN LOOP
	while (!glfwWindowShouldClose(window)) {
		float now = glfwGetTime();
		if (clicks == sig || clicks == 0) {
			glfwSetTime(0);
		}

		void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
		    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
				if (clicks == sig) {
					clicks = 1;
				} else {
					clicks++;
				}
				times[clicks] = now;

				// :^)
				bpm = 60 / times[sig] * sig;
			}
		    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
				clicks = 0;
				times = malloc(sig * sizeof(float));
			}
		}

		glfwSetMouseButtonCallback(window, mouse_button_callback);

		// ESC -> EXIT
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			
			//printf("\nBPM:\t%f\n", bpm);

			glfwSetWindowShouldClose(window, 1);
		}

		// Render
		render();

		// Swap front & back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
		//glfwWaitEvents();
	}
	glfwTerminate();

	return 0;
}
