#include "game_objects/Components/MeshComponent.h"

using namespace lcn::object;

MeshComponent::MeshComponent()
{

}

MeshComponent::~MeshComponent()
{

}

void MeshComponent::Update()
{

}

void MeshComponent::SetMeshGUID(uint32_t a_GUID)
{
	m_MeshGUID = a_GUID;
}

uint32_t MeshComponent::GetMeshGUID()
{
	return m_MeshGUID;
}