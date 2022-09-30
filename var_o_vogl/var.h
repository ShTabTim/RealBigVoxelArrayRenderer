//
// Created by Pomidor on 16.07.2022.
//

#ifndef VAR_O_VOGL_VAR_H
#define VAR_O_VOGL_VAR_H

#include <cinttypes>

typedef struct var {
	uint32_t w;
	uint32_t h;
	uint32_t d;

	uint32_t* vox_buf = 0;

	void init(uint32_t w, uint32_t h, uint32_t d);
	void init_vox_buf();
	void finit();

	bool is_in_var(uint32_t x, uint32_t y, uint32_t z);
	uint32_t& operator() (uint32_t x, uint32_t y, uint32_t z) const;
	void zero();

	~var();
} var;

#endif //VAR_O_VOGL_VAR_H