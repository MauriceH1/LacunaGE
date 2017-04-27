#pragma once

#include <glm\vec3.hpp>
#include <glm\vec4.hpp>
#include <cstdint>

namespace lcn::resources
{
	struct ShaderData {
		glm::vec4 color;
	};

	class Material
	{
	public:
		Material();
		~Material();

		void SetColor(glm::vec3 a_Color);
		void SetColor(glm::vec4 a_Color);
		void SetColor(float a_Red, float a_Green, float a_Blue);
		void SetColor(float a_Red, float a_Green, float a_Blue, float a_Alpha);

		uint32_t GetPipelineState();

	private:
		uint32_t m_PipelineState = 0;

		ShaderData m_Data;
	};
}