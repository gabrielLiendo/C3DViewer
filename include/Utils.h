#pragma once
struct vec2
{
	float x, y;
};

struct vec3
{
	float x, y, z;
};

struct TriangleIndexes
{
	int v1, v2, v3;

	TriangleIndexes(int v1, int v2, int v3)
	{
		this->v1 = v1;
		this->v2 = v2;
		this->v1 = v3;
	}
};


struct Vertex
{
	glm::vec3 position, normal;

	Vertex() = default;

	Vertex(glm::vec3 position, glm::vec3 normal)
	{
		this->position = position;
		this->normal = normal;
	}
};


struct Triangle
{
	int pos1, pos2, pos3;
	glm::vec3 v1, v2, v3, normal;

	Triangle(int pos1, int pos2, int pos3, glm::vec3  v1, glm::vec3 v2, glm::vec3  v3)
	{
		this->pos1 = pos1;
		this->pos2 = pos2;
		this->pos3 = pos3;
		this->v1 = v1;
		this->v2 = v2;
		this->v3 = v3;
		normal = normalize(cross(v2 - v1, v3 - v1));
	}
};