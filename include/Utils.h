#pragma once
struct Vertex
{
	glm::vec3 position, normal;
	glm::vec2 textCoord;

	Vertex() = default;

	Vertex(glm::vec3 position, glm::vec3 normal)
	{
		this->position = position;
		this->normal = normal;
	}

	Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 textCoord)
	{
		this->position = position;
		this->normal = normal;
		this->textCoord = textCoord;
	}
};

enum LightingModel {AMBIENT, LAMBERT, PHONG};
enum TextureMap {AMBIENT_MAP, DIFFUSE_MAP, SPECULAR_MAP};