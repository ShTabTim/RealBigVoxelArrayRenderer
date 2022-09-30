//
// Created by Pomidor on 16.07.2022.
//

#ifndef VAR_O_VOGL_VAR_MAP_H
#define VAR_O_VOGL_VAR_MAP_H

#include "var.h"

typedef struct var_map {
	var* map;
	uint32_t w;
	uint32_t h;
	uint32_t d;

	void init(uint32_t w, uint32_t h, uint32_t d) {
		this->w = w;
		this->h = h;
		this->d = d;
	}

	var& operator()(uint32_t x, uint32_t y, uint32_t z) const {
		return map[d * (h * x + y) + z];
	}
} var_map;

#endif //VAR_O_VOGL_VAR_MAP_H
