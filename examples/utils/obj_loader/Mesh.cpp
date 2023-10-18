#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>

#include <Mesh.h>

Scene load_scene_from_obj(const std::string &path)
{
    Scene scn;

    std::ifstream fin;
    fin.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    std::stringstream ss_fin;

    try {
        fin.open(path);
        ss_fin << fin.rdbuf();
    } catch(std::ifstream::failure e) {
        std::cout << "Couldn't load .obj file!" << std::endl;
        std::cout << "\tpath: " << path << std::endl;
    }

    std::string line;
    
    std::vector<LiteMath::float4> vertices;
    std::vector<unsigned> indices;
    std::vector<LiteMath::float3> normals;
    std::vector<LiteMath::float2> tex_coords;
    std::string obj_name;

    LiteMath::float4 cur_vertex(0.0f, 0.0f, 0.0f, 1.0f);
    LiteMath::float3 cur_nomal(0.0f);
    LiteMath::float2 cur_tex_coords;
    unsigned cur_index = 0;

    int v_indx = -1;
    int t_indx = -1;
    int n_indx = -1;
    int v_count = 0;
    std::string tmp;

    while(std::getline(ss_fin, line)) {
        std::stringstream ss_line(line);
        
        char l0, l1;
        ss_line >> l0;

        switch (l0)
        {
        case 'o':
            cur_index = 0;
            ss_line >> obj_name;
            break;
        case 'v':
            l1 = ss_line.get();
            switch(l1)
            {
            case ' ':
                ss_line >> cur_vertex[0];
                ss_line >> cur_vertex[1];
                ss_line >> cur_vertex[2];
                vertices.push_back(cur_vertex);
                break;
            case 't':
                ss_line >> cur_tex_coords[0];
                ss_line >> cur_tex_coords[1];
                tex_coords.push_back(cur_tex_coords);
                break;
            case 'n':
                ss_line >> cur_nomal[0];
                ss_line >> cur_nomal[1];
                ss_line >> cur_nomal[2];
                normals.push_back(cur_nomal);
                break;
            }
            break;
        case 'f':
            v_count = 0;
            while(ss_line >> tmp) {
                v_indx = n_indx = t_indx = -1;
                sscanf(tmp.c_str(), "%d/%d/%d", &v_indx, &t_indx, &n_indx);
                scn.meshes[obj_name].vertices.push_back(vertices[v_indx-1]);
                scn.meshes[obj_name].tex_coords.push_back((t_indx != -1) ? tex_coords[t_indx-1] : LiteMath::float2(0.0f));
                scn.meshes[obj_name].normals.push_back((n_indx != -1) ? normals[n_indx-1] : LiteMath::float3(0.0f));
                scn.meshes[obj_name].indices.push_back(cur_index++);
                ++v_count;
            }
            for (int i = 0; i < v_count; ++i) {
                LiteMath::float3 p = LiteMath::to_float3(scn.meshes[obj_name].vertices[cur_index-v_count+i]);
                LiteMath::float3 p1 = LiteMath::to_float3(scn.meshes[obj_name].vertices[cur_index-v_count+(i+1)%v_count]);
                LiteMath::float3 p2 = LiteMath::to_float3(scn.meshes[obj_name].vertices[cur_index-v_count+(i+2)%v_count]);
                LiteMath::float2 uv = scn.meshes[obj_name].tex_coords[cur_index-v_count+i];
                LiteMath::float2 uv1 = scn.meshes[obj_name].tex_coords[cur_index-v_count+(i+1)%v_count];
                LiteMath::float2 uv2 = scn.meshes[obj_name].tex_coords[cur_index-v_count+(i+2)%v_count];
                
                auto edge1 = p1-p;
                auto edge2 = p2-p;
                auto duv1 = uv1-uv;
                auto duv2 = uv2-uv;
                float f = 1.0f / (duv1.x * duv2.y - duv2.x * duv1.y);
                
                LiteMath::float3 tangent;
                LiteMath::float3 bitangent;

                tangent.x = f * (duv2.y * edge1.x - duv1.y * edge2.x);
                tangent.y = f * (duv2.y * edge1.y - duv1.y * edge2.y);
                tangent.z = f * (duv2.y * edge1.z - duv1.y * edge2.z);
                tangent = LiteMath::normalize(tangent);

                bitangent.x = f * (-duv2.x * edge1.x + duv1.x * edge2.x);
                bitangent.y = f * (-duv2.x * edge1.y + duv1.x * edge2.y);
                bitangent.z = f * (-duv2.x * edge1.z + duv1.x * edge2.z);
                bitangent = LiteMath::normalize(bitangent);

                scn.meshes[obj_name].tangent.push_back(tangent);
                scn.meshes[obj_name].bitangent.push_back(bitangent);
            }
            break;
        }
    }

    return scn;
};