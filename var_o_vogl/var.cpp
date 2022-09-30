//
// Created by Pomidor on 16.07.2022.
//

#include <malloc.h>
#include <memory.h>
#include <cassert>
#include "var.h"

void var::init(uint32_t w, uint32_t h, uint32_t d) {
	this->w = w;
	this->h = h;
	this->d = d;
	init_vox_buf();
}

void var::init_vox_buf() {
	vox_buf = (uint32_t*)((!vox_buf) ? malloc((3 + w * h * d) * sizeof(uint32_t)) : realloc(vox_buf, (3 + w * h * d) * sizeof(uint32_t)));
	assert(vox_buf);
	vox_buf[0] = w; vox_buf[1] = h; vox_buf[2] = d;
}

void var::finit() {
	if (vox_buf)
		free(vox_buf);
}



bool var::is_in_var(uint32_t x, uint32_t y, uint32_t z) {
	return x < w&& y < h&& z < d;
}

uint32_t& var::operator() (uint32_t x, uint32_t y, uint32_t z) const {
	return vox_buf[3 + d * (h * x + y) + z];
}

void var::zero() {
	memset(vox_buf+3, 0, w * h * d * sizeof(uint32_t) / sizeof(uint8_t));
}



var::~var() {
	finit();
}