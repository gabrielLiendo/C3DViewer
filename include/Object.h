#pragma once
class Object
{
public:
	Object() = default;

	Object(std::vector<Mesh> meshes)
	{
		name = "";
		this->meshes = meshes;
	
		// Set bounding box's corners values
		float min_x, max_x, min_y, max_y, min_z, max_z, size_x, size_y, size_z, scaleFactor;

		min_x = max_x = meshes[0].vertices[0].position.x;
		min_y = max_y = meshes[0].vertices[0].position.y;
		min_z = max_z = meshes[0].vertices[0].position.z;

		int n = meshes.size();
		for (int i = 0; i < n; i++)
		{
			int m = meshes[i].vertices.size();
			for (int j = 0; j < m; j++)
			{
				Vertex vertex = meshes[i].vertices[j];
				if (vertex.position.x < min_x) min_x = vertex.position.x;
				if (vertex.position.x > max_x) max_x = vertex.position.x;
				if (vertex.position.y < min_y) min_y = vertex.position.y;
				if (vertex.position.y > max_y) max_y = vertex.position.y;
				if (vertex.position.z < min_z) min_z = vertex.position.z;
				if (vertex.position.z > max_z) max_z = vertex.position.z;
			}
		}

		// Initialize model matrix components
		translation = std::make_shared<glm::vec3>(0.0, 0.0, 0.0);
		scale = std::make_shared<glm::vec3>(1.0, 1.0, 1.0);
		angles = std::make_shared <glm::vec3>(0.0f, 0.0f, 0.0f);

		// Normalize mesh into unitary parallelepiped
		size_x = max_x - min_x;
		size_y = max_y - min_y;
		size_z = max_z - min_z;

		scaleFactor = std::max(std::max(size_x, size_y), size_z);
		normalize = std::make_shared <glm::vec3> (1/ scaleFactor, 1/ scaleFactor, 1/ scaleFactor);


		// Create bounding box
		glm::vec3 size = glm::vec3(size_x, size_y, size_z);
		boundingBox = BoundingBox(glm::vec3(min_x, min_y, min_z), glm::vec3(max_x, max_y, max_z),
								size, normalize, translation, scale, angles);

		useDepthTest = true;
		useCullFace = false;
		useMultisample = false;

		pointSize = 3;
		showWireframe = false;
		showVertices = false;
		showNormals = false;
		wireframeColor = glm::vec3(1.0f, 1.0f, 1.0f);
		verticesColor = glm::vec3(0.0f, 0.0f, 0.0f);
		normalsColor = glm::vec3(0.0f, 1.0f, 0.0f);
	}

	void setName(std::string name) 
	{
		this->name = name;
	}

	std::string getName()
	{
		return name;
	}

	glm::vec3* getScaleFactor()
	{
		return &(*scale);
	}

	glm::vec3* getTranslationFactor()
	{
		return &(*translation);
	}

	glm::vec3* getRotationFactor()
	{
		return &(*angles);
	}

	glm::mat4 getModelTransformation()
	{
		return
			glm::translate(glm::mat4(1.0f), *translation) *
			glm::rotate(glm::mat4(1.0f), glm::radians(angles->x), glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::rotate(glm::mat4(1.0f), glm::radians(angles->y), glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::rotate(glm::mat4(1.0f), glm::radians(angles->z), glm::vec3(0.0f, 0.0f, 1.0f)) *
			glm::scale(glm::mat4(1.0f), *scale) * 
			glm::scale(glm::mat4(1.0f), *normalize);
	}

	glm::mat4 getBoxModel()
	{
		return boundingBox.getModelTransformation();
	}

	glm::vec3* getBoxColor()
	{
		return boundingBox.getBoxColor();
	}

	glm::vec3* getNormalsColor()
	{
		return &normalsColor;
	}

	float getScaleNormal()
	{
		return boundingBox.scaleNormal(getModelTransformation());
	}

	int* getPointSize()
	{
		return &pointSize;
	}

	bool* getZBufferBool()
	{
		return &useDepthTest;
	}

	bool* getCullFaceBool()
	{
		return &useCullFace;
	}

	bool* getWireframeBool()
	{
		return &showWireframe;
	}

	bool* getVerticesBool()
	{
		return &showVertices;
	}

	bool* getNormalsBool()
	{
		return &showNormals;
	}

	bool* getMultisampleBool()
	{
		return &useMultisample;
	}

	glm::vec3* getWireframeColor()
	{
		return &wireframeColor;
	}

	glm::vec3* getVerticesColor()
	{
		return &verticesColor;
	}

	void draw(int colorLoc)
	{
		glm::vec3 color;

		if (useDepthTest)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);

		if (useCullFace)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);

		if (useMultisample)
			glEnable(GL_MULTISAMPLE);
		else
			glDisable(GL_MULTISAMPLE);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0);
		for (int i = 0; i < meshes.size(); i++)
		{
			color = meshes[i].mtl->getDiffuse();
			glUniform3f(colorLoc, color.x, color.y, color.z);
			meshes[i].draw();
		}
		glDisable(GL_POLYGON_OFFSET_FILL);

		if (showVertices)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			glPointSize(pointSize);
			for (int i = 0; i < meshes.size(); i++)
			{
				glUniform3f(colorLoc, verticesColor.x, verticesColor.y, verticesColor.z);
				meshes[i].draw();
			}
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		if (showWireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			for (int i = 0; i < meshes.size(); i++)
			{
				glUniform3f(colorLoc, wireframeColor.x, wireframeColor.y, wireframeColor.z);
				meshes[i].draw();
			}
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
	
	void drawNormals()
	{
		for (int i = 0; i < meshes.size(); i++)
			meshes[i].drawVertex();
	}

	void drawBoundingBox()
	{
		boundingBox.draw();
	}

private:
	// Object basic components
	std::string name;
	std::vector<Mesh> meshes;

	// Model Tranformation
	std::shared_ptr<glm::vec3> normalize, scale, translation, angles;

	// Bounding box
	BoundingBox boundingBox;

	// Rendering State
	bool useDepthTest;
	bool useCullFace;
	bool useMultisample;

	int pointSize;
	bool showWireframe;
	bool showVertices;
	bool showNormals;

	glm::vec3 wireframeColor;
	glm::vec3 verticesColor;
	glm::vec3 normalsColor;
};

std::vector<Object> objects;
bool draw = false;
Object *selectedObject;