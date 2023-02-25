// Markus Buchholz, 2023

#include <stdafx.hpp>
#include "imgui_helper.hpp"
#include "ball.h"
#include <tuple>
#include <thread>
#include <chrono>
#include <vector>
#include <math.h>

//---------------------------------------------------------------
std::vector<Point> points = {
	{{140, 400}, 10}};

//----------- system dynamic parameters --------------------

float r = 3.0;
float a = 9.0;
float b = 4.0;

//---------------------------------------------------------------

std::tuple<std::vector<float>, std::vector<float>, std::vector<float>, std::vector<float>, std::vector<float>, std::vector<float>, std::vector<float>, std::vector<float>, std::vector<float>, std::vector<float>, std::vector<float>, std::vector<float>> simulation()
{
	std::vector<float> a_x;
	std::vector<float> a_y;
	std::vector<float> b_x;
	std::vector<float> b_y;
	std::vector<float> c_x;
	std::vector<float> c_y;
	std::vector<float> d_x;
	std::vector<float> d_y;
	std::vector<float> o_x;
	std::vector<float> o_y;
	std::vector<float> p_x;
	std::vector<float> p_y;

	float dt = M_PI / 180.0;
	float Ox = 0.0;
	float Oy = 0.0;
	float Px = Ox - r;
	float Py = Oy;

	float beta = std::acos((float)(a - b) / (2 * r));
	float maxTheta = 2 * beta;
	float maxCy = (a + b) * std::sin(beta);

	float Ax = 0.0;
	float Ay = 0.0;
	float ABx = 0.0;
	float ABy = 0.0;
	float Bx = 0.0;
	float By = 0.0;
	float ADx = 0.0;
	float ADy = 0.0;
	float Dx = 0.0;
	float Dy = 0.0;
	float Cx = 0.0;
	float Cy = 0.0;

	float deg = 67.1;

	std::vector<float> time;

	for (int i = 0; i < 3; i++){

		for (float t = -M_PI/3; t <= M_PI /3; t = t +dt){
			time.push_back (t);
		} 

		for (float t = M_PI/3; t > - M_PI /3; t = t - dt){
			time.push_back (t);
		} 

	}

	for (auto &t : time)
	{

		//------------------------

		Ax = Ox + r * std::cos(t);
		Ay = Oy + r * std::sin(t);

		float AP = std::sqrt(std::pow(Ax - Px, 2) + std::pow(Ay - Py, 2));
		float alpha = std::acos((float)(b * b + AP * AP - a * a) / (2 * b * AP));

		//------------------------

		auto rotX = [](float x, float y, float tt)
		{
			return (float)x * std::cos(tt) - y * std::sin(tt);
		};

		auto rotY = [](float x, float y, float tt)
		{
			return (float)x * std::sin(tt) + y * std::cos(tt);
		};

		ABx = (float)(b / AP) * rotX(Px - Ax, Py - Ay, alpha);
		ABy = (float)(b / AP) * rotY(Px - Ax, Py - Ay, alpha);

		Bx = Ax + ABx;
		By = Ay + ABy;

		//------------------------
		ADx = (float)(b / AP) * rotX(Px - Ax, Py - Ay, -alpha);
		ADy = (float)(b / AP) * rotY(Px - Ax, Py - Ay, -alpha);

		Dx = Ax + ADx;
		Dy = Ay + ADy;
		//------------------------

		Cx = Ax + ABx + ADx;
		Cy = Ay + ABy + ADy;

		//------------------------
		std::cout << Ax << " : " << ABx << " : " << ADx << " : " << Ax + ABx + ADx << "\n";

		a_x.push_back(Ax);
		a_y.push_back(Ay);
		b_x.push_back(Bx);
		b_y.push_back(By);
		c_x.push_back(Cx);
		c_y.push_back(Cy);
		d_x.push_back(Dx);
		d_y.push_back(Dy);
		o_x.push_back(Ox);
		o_y.push_back(Oy);
		p_x.push_back(Px);
		p_y.push_back(Py);
	}
	return std::make_tuple(a_x, a_y, b_x, b_y, c_x, c_y, d_x, d_y, o_x, o_y, p_x, p_y);
}

int main(int argc, char const *argv[])
{

	int w = 1280;
	int h = 720;
	float scale_x = 40.0;
	float scale_y = 40.0;
	std::string title = "Bouncing ball";
	initImgui(w, h, title);

	// simulation state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(89.0f / 255.0, 88.0f / 255.0, 87.0f / 255.0, 1.00f);
	ImVec4 ray_color = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);

	// compute ball path
	auto sim = simulation();

	// Main loop
	int ii = 0;
	bool flag = true;
	std::vector<ImVec2> tail;
	std::vector<ImVec2> tail_circle;
	std::vector<ImVec2> tail_arc;

	int LOOPS = 5;

	while (!glfwWindowShouldClose(window) && flag == true)
	{
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		//----------------------------------------

		ImGui::NewFrame();

		ImGuiWindowFlags window_flags = 0;
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(w, h), ImGuiCond_FirstUseEver);
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoBackground;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGui::Begin("Simulation", nullptr, window_flags);
		ImDrawList *draw_list = ImGui::GetWindowDrawList();

		ImVec2 A = {(float)points[0].position.x + std::get<0>(sim)[ii] * scale_x, (float)points[0].position.y - std::get<1>(sim)[ii] * scale_y};

		//ImVec2 XY0 = {(float)points[0].position.x, (float)points[0].position.y};
		ImVec2 B = {(float)points[0].position.x + std::get<2>(sim)[ii] * scale_x, (float)points[0].position.y - std::get<3>(sim)[ii] * scale_y};

		ImVec2 C = {(float)points[0].position.x + std::get<4>(sim)[ii] * scale_x, (float)points[0].position.y - std::get<5>(sim)[ii] * scale_y};
		ImVec2 D = {(float)points[0].position.x + std::get<6>(sim)[ii] * scale_x, (float)points[0].position.y - std::get<7>(sim)[ii] * scale_y};
		ImVec2 X_Y = {(float)points[0].position.x + std::get<8>(sim)[ii] * scale_x, (float)points[0].position.y - std::get<9>(sim)[ii] * scale_y};
		ImVec2 P = {(float)points[0].position.x + std::get<10>(sim)[ii] * scale_x, (float)points[0].position.y - std::get<11>(sim)[ii] * scale_y};

		float radius = 5;
		float size_line = 3;

		draw_list->AddCircleFilled(C, radius, ImColor(ray_color));

		draw_list->AddLine(D, C, ImColor(ray_color), 2.0);
		draw_list->AddLine(B, C, ImColor(ray_color), 2.0);
		draw_list->AddLine(A, B, ImColor(ray_color), 2.0);
		draw_list->AddLine(A, D, ImColor(ray_color), 2.0);
		draw_list->AddLine(P, D, ImColor(ray_color), 2.0);
		draw_list->AddLine(P, B, ImColor(ray_color), 2.0);
		draw_list->AddLine(X_Y, A, ImColor(ray_color), 2.0);

		tail.push_back(C);
		//tail_circle.push_back(A);
		tail_arc.push_back(A);

		for (int ii = 0; ii < tail.size(); ii++)
		{
			draw_list->AddCircleFilled(tail[ii], 0.5, ImColor(ray_color));
			draw_list->AddCircleFilled(tail_arc[ii], 0.5, ImColor(ray_color));
		//	draw_list->AddCircleFilled(tail_circle[ii], 0.5, ImColor(ray_color));
		}

		ii++;
		if (ii >= std::get<0>(sim).size())
		{
			flag = false;
		}
		ImGui::End();

		//----------------------------------------

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	termImgui();
	return 0;
}
