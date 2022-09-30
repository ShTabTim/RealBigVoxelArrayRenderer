#version 450 core

layout(std140, binding = 3) uniform inputs{
	float n;
	float aspect;
	float h;

	float pos_x;
	float pos_y;
	float pos_z;

	float ang_x;
	float ang_y;
	float ang_z;

	float mod_pos_x;
	float mod_pos_y;
	float mod_pos_z;

	float mod_mat_x_x;
	float mod_mat_x_y;
	float mod_mat_x_z;

	float mod_mat_y_x;
	float mod_mat_y_y;
	float mod_mat_y_z;

	float mod_mat_z_x;
	float mod_mat_z_y;
	float mod_mat_z_z;
};

layout(std430, binding = 0) readonly buffer voxels_block {
    uint voxels[];
};

out vec4 frag_color;

float cube_intersect_0(vec3 neg_ro, vec3 inv_rd, vec3 size) {
    neg_ro = inv_rd * (neg_ro + vec3(inv_rd.x < 0, inv_rd.y < 0, inv_rd.z < 0) * size);
    return max(max(neg_ro.x, neg_ro.y), max(neg_ro.z, 0));
}

vec2 cube_intersect(vec3 neg_ro, vec3 inv_rd, vec3 size) {
    vec3 t0 = inv_rd * neg_ro;
    vec3 t1 = t0 + inv_rd * size;
    vec3 t2 = min(t0, t1);
    float f1 = max(max(t2.x, t2.y), t2.z);
    t2 = max(t0, t1);
    float f2 = min(min(t2.x, t2.y), t2.z);
    if (f1 > f2 || f2 <= 0) return vec2(0);
    return vec2(f1, f2);
}

vec2 rot(vec2 v, float ang) {
	return vec2(
		v.x * cos(ang) - v.y * sin(ang),
		v.y * cos(ang) + v.x * sin(ang)
	);
}

vec4 getColor(uint id) {
    return vec4(id & 0xFF, (id >> 8) & 0xFF, (id >> 16) & 0xFF, (id >> 24) & 0xFF) * 0.0039215686274509803921568627451;
}

struct ray_hit {
    ivec3 norm;
    vec4 col;
    float l;
};

ivec3 ppp(vec3 v, vec3 w) { return ivec3(v.x <= 0 && w.x <= 0, v.y <= 0 && w.y <= 0, v.z <= 0 && w.z <= 0); }

bool cast_ray(vec3 ro, vec3 rd, inout ray_hit hit) {
    vec3 ro_0 = ro;

    vec3 inv_rd = 1.0 / rd;

    ro += rd * (cube_intersect_0(-ro, inv_rd, vec3(voxels[0], voxels[1], voxels[2])) + 0.001);

    ivec3 vox_pos = ivec3(ro);
    ivec3 step = -ivec3(sign(inv_rd));
    vec3 tmax = inv_rd*((1 - step) * 0.5 - ro + vox_pos);

    while (true) {
        uint id = voxels[3 + (vox_pos.x * voxels[1] + vox_pos.y) * voxels[2] + vox_pos.z];

        if (bool(0xFF000000 & id)) {
            vec4 new_col = getColor(id);
            hit.col = (new_col + hit.col) / (1 + new_col * hit.col);

            if (hit.col.a > 0.95) {
                hit.l = cube_intersect_0(vox_pos - ro_0, inv_rd, vec3(1));
                hit.col.a = 1;
                return true;
            }
        }

        hit.norm = step * ppp(tmax - tmax.yzx, tmax - tmax.zxy);
        vox_pos -= hit.norm;

        if (vox_pos.x < 0 || vox_pos.x >= voxels[0] || vox_pos.y < 0 || vox_pos.y >= voxels[1] || vox_pos.z < 0 || vox_pos.z >= voxels[2])
            return false;

        tmax -= inv_rd * hit.norm;
    }

    return false;
}

void main() {
    mat3 inv_mod_mat = inverse(mat3(
        mod_mat_x_x, mod_mat_x_y, mod_mat_x_z,
        mod_mat_y_x, mod_mat_y_y, mod_mat_y_z,
        mod_mat_z_x, mod_mat_z_y, mod_mat_z_z));

	vec3 rd = normalize(vec3(2*gl_FragCoord.xy-vec2(aspect*h, h), h));

	rd.yz = rot(rd.yz, -ang_x);
	rd.zx = rot(rd.zx, -ang_y);
	rd.xy = rot(rd.xy, -ang_z);

    rd = inv_mod_mat * rd;

    float inv_l = length(rd);
    if (inv_l < 0.0001)
        discard;
    inv_l = 1.0 / inv_l;
    rd = rd*inv_l;

    vec3 ro = vec3(pos_x - mod_pos_x, pos_y - mod_pos_y, pos_z - mod_pos_z);
    ro = inv_mod_mat * ro;

    ray_hit hit;

    bool intersect = cast_ray(ro, rd, hit);

    if (!intersect && hit.col.a < 0.1)
        discard;

    hit.l *= inv_l;

	gl_FragDepth = (hit.l - n) / hit.l;
	frag_color = hit.col;
}