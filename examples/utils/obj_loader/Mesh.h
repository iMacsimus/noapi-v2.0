#pragma once 
#include <vector>
#include <map>
#include <string>

#include <LiteMath.h>

struct Mesh
{
    std::string name;

    //attributes
    std::vector <LiteMath::float4> vertices;
    std::vector <LiteMath::float2> tex_coords;
    std::vector <LiteMath::float3> normals;
    std::vector <LiteMath::float3> tangent;
    std::vector <LiteMath::float3> bitangent;

    //for PipelineStateObject
    std::vector <unsigned> indices;
};

struct Scene
{
    std::string path;
    std::map<std::string, Mesh> meshes;
};

Scene load_scene_from_obj(const std::string &path);