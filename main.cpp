#include "FundLibs/glad 46/include/glad/glad.h"
#include "FundLibs/sh_win/keys.h"
#include "FundLibs/sh_win/win.h"
#include <chrono>
#include "FundLibs/vogl/Shader.h"
#include "var_o_vogl/var.h"
#include "FundLibs/noise/Simplex.h"
#include "var_o_vogl/var_renderer.h"
#include "var_o_vogl/mat3x3.h"

#pragma comment(lib, "opengl32.lib")

struct ubo_buf {
	float n;
	float aspect;
	float h;

	float pos[3];
	float ang[3];

	float mod_pos[3];

	fmat3x3 mod_mat;
} ubo_buf;

pipeprog shader;

fwind gr_win;
HDC hdc;
uint32_t cam_ubo;

uint32_t vox_buf;

HGLRC hRC;
var v;

uint32_t get_col(uint32_t r, uint32_t g, uint32_t b, uint32_t a) {
	return (r & 0xFF) | ((g & 0xFF) << 8) | ((b & 0xFF) << 16) | ((a&0xFF) << 24);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_PAINT:
	{
///////////////////////////////////////////////////////////////////////////////////////////////////////////
		static uint32_t old_w, old_h;
		if (gr_win.get_w() != old_w || gr_win.get_h() != old_h) {
			glViewport(0, 0, gr_win.get_w(), gr_win.get_h());
			ubo_buf.aspect = ((float)(gr_win.get_w())) / ((float)(gr_win.get_h()));
			ubo_buf.h = gr_win.get_h();
			old_w = gr_win.get_w();
			old_h = gr_win.get_h();
		}

		//>>set dt 
		static std::chrono::system_clock::time_point tp1 = std::chrono::system_clock::now();
		static std::chrono::system_clock::time_point tp2 = tp1;
		tp2 = std::chrono::system_clock::now();
		std::chrono::duration<float> dTime = tp2 - tp1;
		tp1 = tp2;
		float dt = dTime.count();
		//<<set dt 

		//>>print fps
		wchar_t s[64];
		swprintf_s(s, 64, L"%s : %5.1f fps", gr_win.get_name(), 1.0f / dt);
		SetWindowText(gr_win.get_hwnd(), s);
		//<<print fps

		//>>key zaloop
		key_loop(gr_win.get_hwnd());
		//<<key zaloop

		if (get_key(VK_ESCAPE).held)
			PostQuitMessage(0);

		ubo_buf.ang[1] += (get_key(VK_LEFT).held - get_key(VK_RIGHT).held) * dt;
		ubo_buf.ang[0] += ((get_key(VK_UP).held && ubo_buf.ang[0] < 1.57079632f) - (get_key(VK_DOWN).held && ubo_buf.ang[0] > -1.57079632f)) * dt;

		ubo_buf.ang[1] += 6.2832853f * ((ubo_buf.ang[1] < 3.14159265f) - (ubo_buf.ang[1] > -3.14159265f));

		static float vel[3];

		vel[0] += get_key('D').held - get_key('A').held;
		vel[1] += get_key(VK_SPACE).held - get_key(VK_SHIFT).held;
		vel[2] += get_key('W').held - get_key('S').held;

		ubo_buf.pos[0] += dt * (vel[0] * cos(ubo_buf.ang[1]) - vel[2] * sin(ubo_buf.ang[1]));
		ubo_buf.pos[1] += dt * vel[1];
		ubo_buf.pos[2] += dt * (vel[2] * cos(ubo_buf.ang[1]) + vel[0] * sin(ubo_buf.ang[1]));

		float friction = 0.8f;

		vel[0] -= friction*vel[0];
		vel[1] -= friction*vel[1];
		vel[2] -= friction*vel[2];

		glBindBuffer(GL_UNIFORM_BUFFER, cam_ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ubo_buf), &ubo_buf);

		glBindProgramPipeline(shader.id);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.05, 0.05, 0.05, 1);

		ubo_buf.mod_pos[0] = 0;
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ubo_buf), &ubo_buf);
		//for(uint32_t i(512);i--;)
			var_o_vogl_bind_vox_buf_data(&v);
		var_o_vogl_render_var();

		SwapBuffers(hdc);

///////////////////////////////////////////////////////////////////////////////////////////////////////////
		}
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		gr_win.resize();
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int CALLBACK WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR pCmdLine, int nCmdShow) {
	gr_win.set_w(100*8);
	gr_win.set_h(80*8);
	gr_win.init(create_wc(hInst, L"LALA", WndProc), L"LALA");

	gr_win.show(0);
///////////////////////////////////////////////////////////////////////////////////////////////////////////

	PIXELFORMATDESCRIPTOR pfd;

	hdc = gr_win.get_hdc();

	ZeroMemory(&pfd, sizeof(pfd));

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 24;
	pfd.iLayerType = PFD_MAIN_PLANE;

	SetPixelFormat(hdc, ChoosePixelFormat(hdc, &pfd), &pfd);
	hRC = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hRC);

	gladLoadGL();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);

	float ll = 256;

	ubo_buf.mod_mat(0, 0) = 1.0f/ll; ubo_buf.mod_mat(0, 1) = 0;       ubo_buf.mod_mat(0, 2) = 0;
	ubo_buf.mod_mat(1, 0) = 0;       ubo_buf.mod_mat(1, 1) = 1.0f/ll; ubo_buf.mod_mat(1, 2) = 0;
	ubo_buf.mod_mat(2, 0) = 0;       ubo_buf.mod_mat(2, 1) = 0;       ubo_buf.mod_mat(2, 2) = 1.0f/ll;

	ubo_buf.n = 0.1f; 
	ubo_buf.aspect = ((float)(gr_win.get_w())) / ((float)(gr_win.get_h())); 
	ubo_buf.h = gr_win.get_h();


	//ubo_buf.mod_pos[0] = -0.5f; ubo_buf.mod_pos[1] = -0.5f; ubo_buf.mod_pos[2] = -0.5f;

	ubo_buf.pos[2] = -1;
	ubo_buf.ang[1] = -1;

	shader.gen(2);
	shader.create(GL_VERTEX_SHADER, GL_VERTEX_SHADER_BIT, "Shaders/main.vert.glsl", 0);
	shader.create(GL_FRAGMENT_SHADER, GL_FRAGMENT_SHADER_BIT, "Shaders/main.frag.glsl", 1);
	shader.bind();

	glGenBuffers(1, &cam_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, cam_ubo);
	//glBufferData(GL_UNIFORM_BUFFER, sizeof(ubo_buf), &ubo_buf, GL_STREAM_DRAW);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(ubo_buf), &ubo_buf, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 3, cam_ubo);

	SimplexNoise nois;

	v.init(ll, ll, ll);
	v.zero();

	float inv_a = 1.0f / ll;
	float inv_b = 1.0f / 16;

	for (uint32_t i(v.w); i--;)
		for (uint32_t j(v.h); j--;)
			for (uint32_t k(v.d); k--;)
				if (nois.noise(i * inv_a, k * inv_a)+1 > 2 * j * inv_a)
					v(i, j, k) = get_col(0, 0xFF * 0.333f * (nois.noise(i * inv_b, j * inv_b, k * inv_b) + 2), 0, 0xFF);

	var_o_vogl_init_var_renderer();

	var_o_vogl_bind_vox_buf_data(&v);

///////////////////////////////////////////////////////////////////////////////////////////////////////////
	MSG msg;
	do {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		RedrawWindow(gr_win.get_hwnd(), nullptr, nullptr, RDW_INTERNALPAINT);
	} while (msg.message != WM_QUIT);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	ReleaseDC(gr_win.get_hwnd(), GetDC(gr_win.get_hwnd()));
	DestroyWindow(gr_win.get_hwnd());

	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	return msg.wParam;
}