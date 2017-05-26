#include "resources\SceneResource.h"

using namespace lcn::resources;

SceneResource::SceneResource()
{
}

SceneResource::~SceneResource()
{

}

void SceneResource::Update()
{

}

void SceneResource::AddEntity(lcn::object::Entity* a_Entity)
{
	m_Entities.push_back(a_Entity);
}

const std::vector<lcn::object::Entity*>* SceneResource::GetEntities() const
{
	return &m_Entities;
}

void SceneResource::AddChild(lcn::object::Entity* a_Entity)
{
	m_Children.push_back(a_Entity);
}

std::vector<lcn::object::Entity*>* SceneResource::GetChildren()
{
	return &m_Children;
}