#pragma once
struct vec2
{
	float x, y;
};

struct vec3
{
	float x, y, z;
};

struct Vertex
{
	vec3 position;
	vec3 normal;
	vec3 color;

	Vertex(vec3 position, vec3 normal)
	{
		this->position = position;
		this->normal = normal;
	}
};