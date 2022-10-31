#pragma once
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

enum LightingModel {AMBIENT, LAMBERT, PHONG};