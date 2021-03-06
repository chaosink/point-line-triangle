#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>

#include "shader.hpp"
#include "play_wav.hpp"

#define max(a, b) (a) > (b) ? (a) : (b)
#define PI 3.14159265358979323846

int between          = 1;    // b
int circle           = 1;    // c
int focus            = 1;    // o
int triangle         = 1;    // t
int vertex           = 1;    // v

float max_dist_pp    = 0.1;  // e
float max_dist_pm    = 0.4;  // l
int mouse            = 0;    // m
int particle_num     = 200;  // n
float particle_speed = 1;    // s

int decoration       = 1;    // d
int fullscreen       = 0;    // f
int window_width     = 1024; // w
int window_height    = 768;  // w

char *input          = NULL; // i
int print            = 0;    // p



float fps                  = 60;
const int max_particle_num = 100000;

struct Particle {
	glm::vec2 pos, speed;
	glm::vec4 color;
};

int digit(int n) {
	int i = 0;
	while(n) {
		n /= 10;
		i++;
	}
	return i;
}

int main(int argc, char** argv) {
	int oc; // option character
	while((oc = getopt(argc, argv, "bcotv e:l:mn:s: dfw: hi:p")) != -1) {
		switch(oc) {
			case 'b':
				between = 0;
				break;
			case 'c':
				circle = 0;
				break;
			case 'o':
				focus = 0;
				break;
			case 't':
				triangle = 0;
				break;
			case 'v':
				vertex = 0;
				break;

			case 'e':
				max_dist_pp = atof(optarg);
				break;
			case 'l':
				max_dist_pm = atof(optarg);
				break;
			case 'm':
				mouse = 1;
				break;
			case 'n':
				particle_num = atoi(optarg);
				break;
			case 's':
				particle_speed = atof(optarg);
				break;

			case 'd':
				decoration = 0;
				break;
			case 'f':
				fullscreen = 1;
				break;
			case 'w':
				window_width = atoi(optarg);
				window_height = atoi(optarg + digit(window_width) + 1);
				break;

			case 'h':
				printf(
"Usage   : point-line [OPTION]\n"
"\n"
"Options :\n"
"    Rendering objects:\n"
"        -b        don't draw lines between nearby points\n"
"        -c        don't draw power circle around focus\n"
"        -o        don't draw lines between points and focus\n"
"        -t        don't draw triangles around focus\n"
"        -v        don't draw vertexes(points)\n"
"    Rendering arguments:\n"
"        -l DIS    set max distance between points and focus\n"
"        -m        control focus by mouse cursor\n"
"        -n NUM    set point number\n"
"        -s SPEED  set point speed\n"
"    Window:\n"
"        -d        remove window decoration\n"
"        -f        enable fullscreen\n"
"        -w SIZE   set window size(for example, 1024x768)\n"
"    Others:\n"
"        -h        show this help message and exit\n"
"        -i WAV    set input WAV file(can be empty)\n"
"        -p        print time information in terminal\n"
"\n"
"Default options:\n"
"    Rendering objects:\n"
"        b: draw lines between nearby points\n"
"        c: draw power circle around focus\n"
"        o: draw lines between points and focus\n"
"        t: draw triangles around focus\n"
"        v: draw vertexes(points)\n"
"    Rendering arguments:\n"
"        l: max distance between points and focus is 0.4\n"
"        m: focus moves automatically\n"
"        n: point number is 200\n"
"        s: point speed is 1.0\n"
"    Window:\n"
"        d: enable window decoration\n"
"        f: disable fullscreen\n"
"        w: window size is 1024x768\n"
"    Others:\n"
"        h: no help message\n"
"        i: no input WAV file\n"
"        p: don't print time information in terminal\n");
				return 0;
				break;
			case 'i':
				input = optarg;
				break;
			case 'p':
				print = 1;
				break;
		}
	}

	if(!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 10);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DECORATED, decoration);

	window = glfwCreateWindow(window_width, window_height, "point-line-triangle", fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
	if(window == NULL){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.1, 0.1, 0.1, 1.0);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	GLuint programID_line = LoadShaders("vertex.glsl", "fragment.glsl", "geometry_line.glsl");
	GLuint programID_point = LoadShaders("vertex.glsl", "fragment.glsl");
	GLuint programID_triangle = LoadShaders("vertex.glsl", "fragment.glsl", "geometry_triangle.glsl");

	GLuint xposID_line = glGetUniformLocation(programID_line, "xpos");
	GLuint yposID_line = glGetUniformLocation(programID_line, "ypos");
	GLuint window_widthID_line = glGetUniformLocation(programID_line, "window_width");
	GLuint window_heightID_line = glGetUniformLocation(programID_line, "window_height");
	GLuint max_dist_pmID_line = glGetUniformLocation(programID_line, "max_dist_pm");

	GLuint xposID_triangle = glGetUniformLocation(programID_triangle, "xpos");
	GLuint yposID_triangle = glGetUniformLocation(programID_triangle, "ypos");
	GLuint window_widthID_triangle = glGetUniformLocation(programID_triangle, "window_width");
	GLuint window_heightID_triangle = glGetUniformLocation(programID_triangle, "window_height");
	GLuint max_dist_pmID_triangle = glGetUniformLocation(programID_triangle, "max_dist_pm");
	GLuint max_dist_ppID_triangle = glGetUniformLocation(programID_triangle, "max_dist_pp");

	Particle particles[max_particle_num];
	srand(time(NULL));
	for(int i = 0; i < particle_num; i++) {
		particles[i].pos = glm::vec2((rand() % 2000 - 1000.0f) / 1000.0f, (rand() % 2000 - 1000.0f) / 1000.0f);
		particles[i].speed = glm::vec2((rand() % 2000 - 1000.0f) / 1000000.0f * particle_speed, (rand() % 2000 - 1000.0f) / 1000000.0f * particle_speed);
		particles[i].color = glm::vec4(rand() % 2000 / 2000.0f, rand() % 2000 / 2000.0f, rand() % 2000 / 2000.0f, rand() % 2000 / 2000.0f);
	}

	GLfloat g_vertex_buffer_data[max_particle_num][2];
	GLfloat g_color_buffer_data[max_particle_num][4];

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



	audio_data data;
	int bpf;
	int data_index = 0;
	double time_total;
	int res;
	pthread_t a_thread;
if(input) {
	data = get_audio_data(input);
	bpf = data.sampling_rate / fps;
	time_total = data.size / data.sampling_rate / 4;

	res = pthread_create(&a_thread, NULL, play_wav_d, input);
	if(res != 0) {
		perror("Thread creation failed!");
		exit(EXIT_FAILURE);
	}
}



	double time_current;	
	int frame_count = 0;
	glfwSetTime(0);
	do {
		glClear(GL_COLOR_BUFFER_BIT);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		float sum_l = 0, sum_r = 0;
		for(int i = 0; i < bpf; i++) {
			sum_l = max(sum_l, abs(((short*)data.data)[data_index]));
			data_index++;
			sum_r = max(sum_r, abs(((short*)data.data)[data_index]));
			data_index++;
		}

		for(int i = 0; i < particle_num; i++) {
//			particles[i].speed += glm::vec2(sin(glfwGetTime()) / 100000, cos(glfwGetTime()) / 100000);
//			particles[i].color += glm::vec4(std::abs(sin(glfwGetTime())) / 1000, std::abs(cos(glfwGetTime())) / 1000, std::abs(sin(glfwGetTime())) / 1000 * std::abs(cos(glfwGetTime())) / 1000, 0);
//			particles[i].color += glm::vec4(sin(particles[i].pos.x * 10), sin(particles[i].pos.y * 10), 0, 0);
			particles[i].pos += particles[i].speed + glm::normalize(particles[i].speed) * (sum_l / 32768 / 50);
	//		particles[i].pos += particles[i].speed + (sum_l / 32768 / 50);
	//		particles[i].pos += particles[i].speed * (sum_l / 32768 * 20);
			if(std::abs(particles[i].pos.x) > 1.0 || std::abs(particles[i].pos.y) > 1.0 || particles[i].speed == glm::vec2(0.0, 0.0)) {
				switch(rand() % 4) {
					case 0 :
						particles[i].pos = glm::vec2((rand() % 2000 - 1000.0f) / 1000.0f,-1.0);
						particles[i].speed = glm::vec2((rand() % 2000 - 1000.0f) / 1000000.0f * particle_speed, std::abs((rand() % 2000 - 1000.0f) / 1000000.0f * particle_speed));
						break;
					case 1 :
						particles[i].pos = glm::vec2((rand() % 2000 - 1000.0f) / 1000.0f, 1.0);
						particles[i].speed = glm::vec2((rand() % 2000 - 1000.0f) / 1000000.0f * particle_speed, -std::abs((rand() % 2000 - 1000.0f) / 1000000.0f * particle_speed));
						break;
					case 2 :
						particles[i].pos = glm::vec2(-1.0, (rand() % 2000 - 1000.0f) / 1000.0f);
						particles[i].speed = glm::vec2(std::abs((rand() % 2000 - 1000.0f) / 1000000.0f * particle_speed), (rand() % 2000 - 1000.0f) / 1000000.0f * particle_speed);
						break;
					case 3 :
						particles[i].pos = glm::vec2(1.0, (rand() % 2000 - 1000.0f) / 1000.0f);
						particles[i].speed = glm::vec2(-std::abs((rand() % 2000 - 1000.0f) / 1000000.0f * particle_speed), (rand() % 2000 - 1000.0f) / 1000000.0f * particle_speed);
						break;
				}
				particles[i].color = glm::vec4(rand() % 2000 / 2000.0f, rand() % 2000 / 2000.0f, rand() % 2000 / 2000.0f, rand() % 2000 / 2000.0f);
			}
		}

		double xpos, ypos;
//		glfwSetCursorPos(window, (sin(time_current * 3 / 4) + 1) / 2 * window_width, (1 - cos(time_current * 5 / 4)) / 2 * window_height);
		glfwGetCursorPos(window, &xpos, &ypos);
		float focus_x = !mouse ? sin(time_current * 3 / 4) : xpos / window_width * 2 - 1;
		float focus_y = !mouse ? cos(time_current * 5 / 4) : 1 - ypos / window_height * 2;



	if(focus || vertex) {
		for(int i = 0; i < particle_num; i++) {
			g_vertex_buffer_data[i][0] = particles[i].pos.x;
			g_vertex_buffer_data[i][1] = particles[i].pos.y;
			g_color_buffer_data[i][0] = particles[i].color.r;
			g_color_buffer_data[i][1] = particles[i].color.g;
			g_color_buffer_data[i][2] = particles[i].color.b;
			g_color_buffer_data[i][3] = particles[i].color.a;
		}

		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, particle_num * sizeof(float) * 2, g_vertex_buffer_data, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(
			0,        // The attribute we want to configure
			2,        // size
			GL_FLOAT, // type
			GL_FALSE, // normalized?
			0,        // stride
			(void*)0  // array buffer offset
		);

		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glBufferData(GL_ARRAY_BUFFER, particle_num * sizeof(float) * 4, g_color_buffer_data, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(
			1,        // The attribute we want to configure
			4,        // size
			GL_FLOAT, // type
			GL_FALSE, // normalized?
			0,        // stride
			(void*)0  // array buffer offset
		);
	}



		float max_dist_pm_d = max_dist_pm;
	if(input)
		max_dist_pm_d = max_dist_pm / 2 + sum_l / 32768 * (1 - max_dist_pm / 2);
	if(focus) {
		glUseProgram(programID_line);
		glUniform1f(xposID_line, focus_x);
		glUniform1f(yposID_line, focus_y);
		glUniform1i(window_widthID_line, window_width);
		glUniform1i(window_heightID_line, window_height);
		glUniform1f(max_dist_pmID_line, max_dist_pm_d);
		glDrawArrays(GL_POINTS, 0, particle_num);
	}



	if(vertex) {
		glUseProgram(programID_point);
		glDrawArrays(GL_POINTS, 0, particle_num);
	}



		int vertex_num = 0;

	if(between || triangle) {
		float dist;
		for(int i = 0; i < particle_num; i++)
			for(int j = i + 1; j < particle_num; j++)
				if((dist = glm::distance(glm::vec2(particles[i].pos.x * 1024 / 768, particles[i].pos.y), glm::vec2(particles[j].pos.x * 1024 / 768, particles[j].pos.y))) <= max_dist_pp) {
					g_vertex_buffer_data[vertex_num][0] = particles[i].pos.x;
					g_vertex_buffer_data[vertex_num][1] = particles[i].pos.y;
					g_color_buffer_data[vertex_num][0] = particles[i].color.r;
					g_color_buffer_data[vertex_num][1] = particles[i].color.g;
					g_color_buffer_data[vertex_num][2] = particles[i].color.b;
					g_color_buffer_data[vertex_num++][3] = (1.0 - dist / max_dist_pp);
					g_vertex_buffer_data[vertex_num][0] = particles[j].pos.x;
					g_vertex_buffer_data[vertex_num][1] = particles[j].pos.y;
					g_color_buffer_data[vertex_num][0] = particles[j].color.r;
					g_color_buffer_data[vertex_num][1] = particles[j].color.g;
					g_color_buffer_data[vertex_num][2] = particles[j].color.b;
					g_color_buffer_data[vertex_num++][3] = (1.0 - dist / max_dist_pp);
				}

		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, vertex_num * sizeof(float) * 2, g_vertex_buffer_data, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(
			0,		 // The attribute we want to configure
			2,		 // size
			GL_FLOAT,  // type
			GL_FALSE,  // normalized?
			0,		 // stride
			(void*)0 // array buffer offset
		);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glBufferData(GL_ARRAY_BUFFER, vertex_num * sizeof(float) * 4, g_color_buffer_data, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(
			1,		 // The attribute we want to configure
			4,		 // size
			GL_FLOAT,  // type
			GL_FALSE,  // normalized?
			0,		 // stride
			(void*)0 // array buffer offset
		);
	}

	if(between) {
		glUseProgram(programID_point);
		glDrawArrays(GL_LINES, 0, vertex_num);
	}

/*		for(int i = 0; i < particle_num; i++)
			for(int j = i + 1; j < particle_num; j++)
				if((dist = glm::distance(glm::vec2(particles[i].pos.x * 1024 / 768, particles[i].pos.y), glm::vec2(particles[j].pos.x * 1024 / 768, particles[j].pos.y))) <= max_dist_pp) {
					vertex_num = 0;
					g_vertex_buffer_data[vertex_num][0] = particles[i].pos.x;
					g_vertex_buffer_data[vertex_num][1] = particles[i].pos.y;
					g_color_buffer_data[vertex_num][0] = particles[i].color.r;
					g_color_buffer_data[vertex_num][1] = particles[i].color.g;
					g_color_buffer_data[vertex_num][2] = particles[i].color.b;
					g_color_buffer_data[vertex_num++][3] = (1.0 - dist / max_dist_pp);
					g_vertex_buffer_data[vertex_num][0] = particles[j].pos.x;
					g_vertex_buffer_data[vertex_num][1] = particles[j].pos.y;
					g_color_buffer_data[vertex_num][0] = particles[j].color.r;
					g_color_buffer_data[vertex_num][1] = particles[j].color.g;
					g_color_buffer_data[vertex_num][2] = particles[j].color.b;
					g_color_buffer_data[vertex_num++][3] = (1.0 - dist / max_dist_pp);
					glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
					glBufferData(GL_ARRAY_BUFFER, vertex_num * sizeof(float) * 2, g_vertex_buffer_data, GL_DYNAMIC_DRAW);
					glVertexAttribPointer(
						0,		 // The attribute we want to configure
						2,		 // size
						GL_FLOAT,  // type
						GL_FALSE,  // normalized?
						0,		 // stride
						(void*)0 // array buffer offset
					);
					glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
					glBufferData(GL_ARRAY_BUFFER, vertex_num * sizeof(float) * 4, g_color_buffer_data, GL_DYNAMIC_DRAW);
					glVertexAttribPointer(
						1,		 // The attribute we want to configure
						4,		 // size
						GL_FLOAT,  // type
						GL_FALSE,  // normalized?
						0,		 // stride
						(void*)0 // array buffer offset
					);
					glDrawArrays(GL_LINES, 0, vertex_num);
				}*/

	if(triangle) {
		glUseProgram(programID_triangle);
		glUniform1f(xposID_triangle, focus_x);
		glUniform1f(yposID_triangle, focus_y);
		glUniform1i(window_widthID_triangle, window_width);
		glUniform1i(window_heightID_triangle, window_height);
		glUniform1f(max_dist_pmID_triangle, max_dist_pm_d);
		glUniform1f(max_dist_ppID_triangle, max_dist_pp);
		glDrawArrays(GL_LINES, 0, vertex_num);
	}



	if(circle) {
		glUseProgram(programID_point);
		vertex_num = 64;
		g_vertex_buffer_data[0][0] =  focus_x;
		g_vertex_buffer_data[0][1] =  focus_y;
		g_color_buffer_data[0][0] = 1.0;
		g_color_buffer_data[0][1] = 1.0;
		g_color_buffer_data[0][2] = 1.0;
		g_color_buffer_data[0][3] = 0.01;
		for(int i = 1; i < vertex_num + 2; i++) {
			g_vertex_buffer_data[i][0] =  focus_x + cos(2.0 * PI / vertex_num * i) * max_dist_pm_d * window_height / window_width;
			g_vertex_buffer_data[i][1] =  focus_y + sin(2.0 * PI / vertex_num * i) * max_dist_pm_d;
			g_color_buffer_data[i][0] = 1.0;
			g_color_buffer_data[i][1] = 1.0;
			g_color_buffer_data[i][2] = 1.0;
			g_color_buffer_data[i][3] = 0.01;
		}
		vertex_num += 2;
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, vertex_num * sizeof(float) * 2, g_vertex_buffer_data, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(
			0,		 // The attribute we want to configure
			2,		 // size
			GL_FLOAT,  // type
			GL_FALSE,  // normalized?
			0,		 // stride
			(void*)0 // array buffer offset
		);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glBufferData(GL_ARRAY_BUFFER, vertex_num * sizeof(float) * 4, g_color_buffer_data, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(
			1,		 // The attribute we want to configure
			4,		 // size
			GL_FLOAT,  // type
			GL_FALSE,  // normalized?
			0,		 // stride
			(void*)0 // array buffer offset
		);
		glDrawArrays(GL_TRIANGLE_FAN, 0, vertex_num);
	}



		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);



		time_current = glfwGetTime();
	if(input) {
		double left_time = time_total - time_current;
		if(left_time <= 0) break;
		if(data_index >= data.size / 2) break;
	}
//		double time_accurate = data_index / 2.0 / bpf / fps;
		double time_accurate = (++frame_count) / fps;
		double time_delta = time_accurate - time_current;
		if(print) printf("frame_count:%d time_accurate:%lf time_current:%lf time_delta:%lf\n", frame_count, time_accurate, time_current, time_delta);
		time_delta = time_delta > 0 ? time_delta : 0;
		usleep(time_delta * 1000000);
		glfwSwapBuffers(window);
		glfwPollEvents();
	} while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);



if(input) {
	pthread_cancel(a_thread);
	void *thread_result;
	res = pthread_join(a_thread, &thread_result);
	if(res != 0) {
		perror("Thread join failed!");
		exit(EXIT_FAILURE);
	}
}



	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID_line);

	glfwTerminate();

	return 0;
}
