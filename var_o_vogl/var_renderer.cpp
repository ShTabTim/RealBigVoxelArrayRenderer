//
// Created by Pomidor on 16.07.2022.
//

#include "var_renderer.h"

#include "ogl.hpp"
#include <cinttypes>

uint32_t var_o_vogl_var_renderer_vox_buf;

void var_o_vogl_init_var_renderer() {
	glGenBuffers(1, &var_o_vogl_var_renderer_vox_buf);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, var_o_vogl_var_renderer_vox_buf);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint32_t) * (256*256*256), 0, GL_STREAM_DRAW);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(uint32_t) * (512*700*512), 0, GL_DYNAMIC_DRAW);
}

void var_o_vogl_bind_vox_buf_data(var* _var) {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, var_o_vogl_var_renderer_vox_buf);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(uint32_t) * (_var->w * _var->h * _var->d + 3), _var->vox_buf);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, var_o_vogl_var_renderer_vox_buf);
}

void var_o_vogl_render_var() {
	glDrawArrays(GL_TRIANGLES, 0, 36);
}