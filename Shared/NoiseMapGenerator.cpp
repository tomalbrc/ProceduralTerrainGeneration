//
//  NoiseMapGenerator.cpp
//  ProcMapGeneration-macOS
//
//  Created by Mac ERPA on 27.08.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#include "NoiseMapGenerator.h"
#include <stdio.h>
#include "PerlinNoise.hpp"

NoiseMap NoiseMapGenerator::Generate(irr::core::dimension2du size, irr::core::vector2di offset, unsigned seed, float scale) {

    int octaves = 3;
    float persistance = 0.6f;
    float lacunarity = 2.6f;
    
    float frequency = 1.f;
    float amplitude = 1.f;
    
    std::vector<irr::core::vector2df> octaveOffsets;
    
    float maxPossibleHeight = 0.f; // for octaves
    
    srand(seed);
    for (int i = 0; i < octaves; i++) {
        irr::core::vector2df o{	static_cast<float>(((rand()%10000)-5000)/10000.0) + offset.X*((int)size.Width-1), 
								static_cast<float>(((rand()%10000)-5000)/10000.0) + offset.Y*((int)size.Height-1)};
        octaveOffsets.push_back(o);

        maxPossibleHeight += amplitude;
        amplitude *= persistance;
    }
    
    float minHeightMapValue = FLT_MAX;
    float maxHeightMapValue = FLT_MIN;
    
    if (scale <= 0.f) scale = 0.001f;
    
    siv::PerlinNoise _Perlin{seed};
    NoiseMap map;
    for (int i = 0; i < size.Height; i++) {
        map.push_back(NoiseMapVector{});
        for (int j = 0; j < size.Width; j++) {
            frequency = 1.f;
            amplitude = 1.f;
            
            float finalHeight = 0.f;
            
            for (int oct = 0; oct < octaves; oct++) {
                float sampleX = ((j - (float)size.Width/2.f) + octaveOffsets[oct].X) / scale * frequency;
                float sampleY = ((i - (float)size.Height/2.f) + octaveOffsets[oct].Y) / scale * frequency;
                
                float value = (_Perlin.noise(sampleX,sampleY,4));
                finalHeight += value * amplitude;
                
                amplitude *= persistance;
                frequency *= lacunarity;
            }
            
            map.back().push_back(finalHeight);
            
            if (finalHeight > maxHeightMapValue) maxHeightMapValue = finalHeight;
            else if (finalHeight < minHeightMapValue) minHeightMapValue = finalHeight;
        }
    }
    
    // gotta normalize those values! loop again...
    for (int i = 0; i < size.Height; i++) {
        for (int j = 0; j < size.Width; j++) {
            // inverse lerp here
            //auto newValue = (map[i][j] - minHeightMapValue) / (maxHeightMapValue - minHeightMapValue);
            // function to smooth everythgin out: f(x) = x^(1/x^3)
            //auto ttt = newValue;//pow(newValue, .5/newValue);

            auto mapVal = map[i][j];
            auto newValue = (mapVal+1.0) / (maxPossibleHeight);
            //auto ttt = pow(newValue, 1.f/newValue);
            auto ttt = irr::core::clamp<float>(newValue, 0, 1.f);
            ttt = pow(ttt, 1.f/ttt);
            
            
            //printf("VAAL: %f (orig: %f)\n", ttt, map[i][j]);
            
            map[i][j] = ttt;
        }
    }
    printf("Max possible height: %f\n", maxPossibleHeight);
    return map;
}

