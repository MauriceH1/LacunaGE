#pragma once

#include <cstdint>

#include <glm\vec3.hpp>
#include <glm\vec2.hpp>

namespace lcn { namespace resources
{
	struct Vertex
	{
		Vertex() {};
		Vertex(glm::vec3 a_Position, glm::vec3 a_Normal) { position = a_Position; normal = a_Normal; };
		glm::vec3 position = glm::vec3(0);
		glm::vec3 normal = glm::vec3(0);
		glm::vec3 tangent = glm::vec3(0);
		glm::vec3 biTangent = glm::vec3(0);
		glm::vec2 texCoord = glm::vec2(0);
	};
};}; // lcn::resources