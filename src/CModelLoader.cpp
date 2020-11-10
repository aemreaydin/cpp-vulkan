#include "CModelLoader.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

using namespace vkTools::vkPrimitives;

SMesh CModelLoader::LoadObjModel(std::string objFile)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn, err;

    auto result = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objFile.c_str());
    if (!warn.empty())
        fprintf(stdout, "%s", warn.c_str());

    if (!err.empty())
        fprintf(stderr, "%s", err.c_str());

    if (!result)
        throw std::runtime_error("Failed to load object.");

    // TODO this only takes the first shape
    SMesh mesh;
    mesh.vertices.resize(attrib.vertices.size() / 3);
    mesh.indices.resize(shapes[0].mesh.indices.size());
    mesh.name = shapes[0].name;

    auto indexIndex = 0;
    for (size_t s = 0; s != 1; ++s)
    {
        size_t index_offset = 0;
        for (size_t f = 0; f != shapes[s].mesh.num_face_vertices.size(); ++f)
        {
            auto fv = shapes[s].mesh.num_face_vertices[f];
            for (size_t v = 0; v < fv; ++v)
            {
                auto ind = shapes[s].mesh.indices[index_offset + v];
                auto vx = attrib.vertices[3 * ind.vertex_index + 0];
                auto vy = attrib.vertices[3 * ind.vertex_index + 1];
                auto vz = attrib.vertices[3 * ind.vertex_index + 2];

                auto nx = attrib.normals[3 * ind.normal_index + 0];
                auto ny = attrib.normals[3 * ind.normal_index + 1];
                auto nz = attrib.normals[3 * ind.normal_index + 2];

                auto tx = attrib.texcoords[2 * ind.texcoord_index + 0];
                auto ty = 1.0f - attrib.texcoords[2 * ind.texcoord_index + 1];

                SVertex vertex{};
                vertex.position = {vx, vy, vz};
                vertex.normal = {nx, ny, nz};
                vertex.uv = {tx, ty};

                mesh.vertices[ind.vertex_index] = vertex;
                mesh.indices[indexIndex++] = ind.vertex_index;
            }
            index_offset += fv;
        }
    }

    return mesh;
}