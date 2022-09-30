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

const vec3 vertices[] = {
	vec3(0.0, 0.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(1.0, 1.0, 0.0),
	vec3(0.0, 0.0, 0.0),
	vec3(1.0, 1.0, 0.0),
	vec3(1.0, 0.0, 0.0),

	vec3(0.0, 1.0, 1.0),
	vec3(0.0, 0.0, 1.0),
	vec3(1.0, 1.0, 1.0),
	vec3(1.0, 1.0, 1.0),
	vec3(0.0, 0.0, 1.0),
	vec3(1.0, 0.0, 1.0),

	vec3(0.0, 0.0, 1.0),
	vec3(0.0, 0.0, 0.0),
	vec3(1.0, 0.0, 1.0),
	vec3(1.0, 0.0, 1.0),
	vec3(0.0, 0.0, 0.0),
	vec3(1.0, 0.0, 0.0),

	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 1.0, 1.0),
	vec3(1.0, 1.0, 1.0),
	vec3(0.0, 1.0, 0.0),
	vec3(1.0, 1.0, 1.0),
	vec3(1.0, 1.0, 0.0),

	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.0, 0.0),
	vec3(0.0, 1.0, 1.0),
	vec3(0.0, 1.0, 1.0),
	vec3(0.0, 0.0, 0.0),
	vec3(0.0, 0.0, 1.0),

	vec3(1.0, 0.0, 0.0),
	vec3(1.0, 1.0, 0.0),
	vec3(1.0, 1.0, 1.0),
	vec3(1.0, 0.0, 0.0),
	vec3(1.0, 1.0, 1.0),
	vec3(1.0, 0.0, 1.0)
};

vec2 rot(vec2 v, float ang) {
	return vec2(
		v.x*cos(ang) - v.y*sin(ang),
		v.y*cos(ang) + v.x*sin(ang)
	);
}

void main() {
	vec3 mod_pos = vec3(mod_pos_x, mod_pos_y, mod_pos_z);
	vec3 pos = vec3(pos_x, pos_y, pos_z);
	mat3 mod_mat = mat3(
		mod_mat_x_x, mod_mat_x_y, mod_mat_x_z,
		mod_mat_y_x, mod_mat_y_y, mod_mat_y_z,
		mod_mat_z_x, mod_mat_z_y, mod_mat_z_z);

	vec3 model;
	model = vertices[gl_VertexID]*vec3(voxels[0], voxels[1], voxels[2]);

	model = mod_mat * model + mod_pos;

	model -= pos;

	model.xy = rot(model.xy, ang_z);
	model.zx = rot(model.zx, ang_y);
	model.yz = rot(model.yz, ang_x);

	gl_Position = vec4(model.x/aspect, model.y, 0, model.z);
}