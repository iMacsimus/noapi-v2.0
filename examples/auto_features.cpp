#include <iostream>
#include <noapi/vsps_auto_interpolate.h>

int main() 
{
    struct Shader
    {
        struct VariablesData
        {
            int x;
            char y;
            double z;
            LiteMath::float4 vPos;
            auto_interpolate(x, y, z, vPos);
        };

        // static VariablesData interpolate(const VariablesData data[3],  
        //     const LiteMath::float3 &barycentric,
        //     float interpolated_1_w)
        // {
        //     return data[0];
        // }
    };
    Shader::VariablesData arr[3] = {};
    Interpolator<Shader>::interpolate(arr, LiteMath::float3(), 0.0f);
}