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
		translation = glm::vec3(0.0, 0.0, 0.0);
		scale = glm::vec3(1.0, 1.0, 1.0);
		angles = glm::vec3(0.0f, 0.0f, 0.0f);

		// Normalize mesh into unitary parallelepiped
		size_x = max_x - min_x;
		size_y = max_y - min_y;
		size_z = max_z - min_z;

		scaleFactor = std::max(std::max(size_x, size_y), size_z);
		normalize = glm::vec3(1/ scaleFactor, 1/ scaleFactor, 1/ scaleFactor);


		// Create bounding box
		glm::vec3 size = glm::vec3(size_x, size_y, size_z);
		boundingBox = OBB(glm::vec3(min_x, min_y, min_z),
							glm::vec3(max_x, max_y, max_z), size);

		std::cout << min_x << " " << min_y << " " << min_z << std::endl;
		std::cout << max_x << " " << max_y << " " << max_z << std::endl;

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
		return &scale;
	}

	glm::vec3* getTranslationFactor()
	{
		return &translation;
	}

	glm::vec3* getRotationFactor()
	{
		return &angles;
	}

	glm::mat4 getModelTransformation()
	{
		return
			glm::translate(glm::mat4(1.0f), translation) *
			glm::rotate(glm::mat4(1.0f), glm::radians(angles.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
			glm::rotate(glm::mat4(1.0f), glm::radians(angles.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
			glm::rotate(glm::mat4(1.0f), glm::radians(angles.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
			glm::scale(glm::mat4(1.0f), scale);
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

	bool TestRayOBBIntersection(
		glm::vec3 ray_origin,        // Ray origin, in world space
		glm::vec3 ray_direction,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.		
		float& intersection_distance // Output : distance between ray_origin and the intersection with the OBB
	) {

		glm::mat4 ModelMatrix = getModelTransformation();

		glm::vec4 aabb_min = ModelMatrix * glm::vec4(*boundingBox.getMin(), 1.0);
		glm::vec4 aabb_max = ModelMatrix * glm::vec4(*boundingBox.getMax(), 1.0);

		

		

		float tMin = 0.0f;
		float tMax = 100000.0f;

		glm::vec3 OBBposition_worldspace(ModelMatrix[3].x, ModelMatrix[3].y, ModelMatrix[3].z);

		glm::vec3 delta = OBBposition_worldspace - ray_origin;

		// Test intersection with the 2 planes perpendicular to the OBB's X axis
		{
			glm::vec3 xaxis(ModelMatrix[0].x, ModelMatrix[0].y, ModelMatrix[0].z);
			float e = glm::dot(xaxis, delta);
			float f = glm::dot(ray_direction, xaxis);

			if (fabs(f) > 0.001f) { // Standard case

				float t1 = (e + aabb_min.x) / f; // Intersection with the "left" plane
				float t2 = (e + aabb_max.x) / f; // Intersection with the "right" plane
				// t1 and t2 now contain distances betwen ray origin and ray-plane intersections

				// We want t1 to represent the nearest intersection, 
				// so if it's not the case, invert t1 and t2
				if (t1 > t2) {
					float w = t1; t1 = t2; t2 = w; // swap t1 and t2
				}

				// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
				if (t2 < tMax)
					tMax = t2;
				// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
				if (t1 > tMin)
					tMin = t1;

				// And here's the trick :
				// If "far" is closer than "near", then there is NO intersection.
				// See the images in the tutorials for the visual explanation.
				if (tMax < tMin)
					return false;

			}
			else { // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
				if (-e + aabb_min.x > 0.0f || -e + aabb_max.x < 0.0f)
					return false;
			}
		}


		// Test intersection with the 2 planes perpendicular to the OBB's Y axis
		// Exactly the same thing than above.
		{
			glm::vec3 yaxis(ModelMatrix[1].x, ModelMatrix[1].y, ModelMatrix[1].z);
			float e = glm::dot(yaxis, delta);
			float f = glm::dot(ray_direction, yaxis);

			if (fabs(f) > 0.001f) {

				float t1 = (e + aabb_min.y) / f;
				float t2 = (e + aabb_max.y) / f;

				if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

				if (t2 < tMax)
					tMax = t2;
				if (t1 > tMin)
					tMin = t1;
				if (tMin > tMax)
					return false;

			}
			else {
				if (-e + aabb_min.y > 0.0f || -e + aabb_max.y < 0.0f)
					return false;
			}
		}


		// Test intersection with the 2 planes perpendicular to the OBB's Z axis
		// Exactly the same thing than above.
		{
			glm::vec3 zaxis(ModelMatrix[2].x, ModelMatrix[2].y, ModelMatrix[2].z);
			float e = glm::dot(zaxis, delta);
			float f = glm::dot(ray_direction, zaxis);

			if (fabs(f) > 0.001f) {

				float t1 = (e + aabb_min.z) / f;
				float t2 = (e + aabb_max.z) / f;

				if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

				if (t2 < tMax)
					tMax = t2;
				if (t1 > tMin)
					tMin = t1;
				if (tMin > tMax)
					return false;

			}
			else {
				if (-e + aabb_min.z > 0.0f || -e + aabb_max.z < 0.0f)
					return false;
			}
		}

		intersection_distance = tMin;
		return true;
	}
private:
	// Object basic components
	std::string name;
	std::vector<Mesh> meshes;

	// Model Tranformation
	glm::vec3 normalize, scale, translation, angles;

	// Bounding box
	OBB boundingBox;

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

struct ObjectDistance
{
	Object* obj;
	float intersectionDistance;
	int index;

	ObjectDistance() = default;

	ObjectDistance(Object* obj, float intersectionDistance, int index)
	{
		this->obj = obj;
		this->intersectionDistance = intersectionDistance;
		this->index = index;
	}
};

std::vector<Object> objects;
bool draw = false;
Object *selectedObject;