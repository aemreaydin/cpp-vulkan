#include "CModel.hpp"
uint32_t CModel::GetVerticesSize()
{
    return static_cast<uint32_t>(sizeof(SVertex) * vertices.size());
}
uint32_t CModel::GetIndicesSize()
{
    return static_cast<uint32_t>(indices.size());
}