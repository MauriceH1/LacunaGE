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

const uint32_t MeshComponent::GetMeshGUID()
{
	return m_MeshGUID;
}

void MeshComponent::SetMeshIndexCount(uint32_t a_Count)
{
	m_IndexCount = a_Count;
}

const uint32_t MeshComponent::GetMeshIndexCount()
{
	return m_IndexCount;
}