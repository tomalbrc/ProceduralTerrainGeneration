//
//  NoiseMap.h
//  ProcMapGeneration-macOS
//
//  Created by Tom Albrecht on 27.08.18.
//  Copyright © 2018 Tom Albrecht. All rights reserved.
//

#ifndef NoiseMap_h
#define NoiseMap_h

#include <stdio.h>
#include <vector>
#include <string>
#include <irrlicht.h>

using NoiseMapVector = std::vector<double>;
using NoiseMap = std::vector<NoiseMapVector>;

/**
 * Generates a 2D noise map from width, height and scale
 */
class NoiseMapGenerator {
public:
    /**
     * Generates a 2-Dimensional NoiseMap for terrain-chunk generation with certain size,
     * seed and scale
     * TODO: Scale noisemap from center instead of top right
     */
    static NoiseMap Generate(irr::core::dimension2du size, irr::core::vector2di offset, unsigned seed, float scale = 1.f, float persistance = 0.1f, float lacunarity = 2.f);
};

#endif /* NoiseMap_h */
