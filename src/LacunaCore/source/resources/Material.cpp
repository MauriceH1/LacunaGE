#include "resources\Material.h"

using namespace lcn::resources;

Material::Material()
{

}

Material::~Material()
{

}

void Material::SetColor(glm::vec3 a_Color)
{
	m_Data.color = glm::vec4(a_Color, 1.0f);
}

void Material::SetColor(glm::vec4 a_Color)
{
	m_Data.color = a_Color;
}

void Material::SetColor(float a_Red, float a_Green, float a_Blue)
{
	m_Data.color = glm::vec4(a_Red, a_Green, a_Blue, 1.0f);
}

void Material::SetColor(float a_Red, float a_Green, float a_Blue, float a_Alpha)
{
	m_Data.color = glm::vec4(a_Red, a_Green, a_Blue, a_Alpha);
}

uint32_t Material::GetPipelineState()
{
	return m_PipelineState;
}