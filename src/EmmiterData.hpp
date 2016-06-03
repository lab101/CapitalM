//
//  EmmiterData.hpp
//  BabyM
//
//  Created by Kris Meeusen on 31/05/16.
//
//

#ifndef EmmiterData_hpp
#define EmmiterData_hpp

#include <stdio.h>
#include "cinder/Rand.h"

class EmitterData{
    
public:
    std::vector<std::vector<float> > data;
    
    
    EmitterData crossover(EmitterData& d){
        
        EmitterData child;
        
        int dataSize = d.data.size();
        int crossover = ci::randInt(dataSize);
        
        //PVector[] child = new PVector[genes.length];
        // Pick a midpoint
        //int crossover = int(random(genes.length));
        // Take "half" from one and "half" from the other
        for (int i = 0; i < dataSize; i++) {
        
            if (i > crossover) child.data.push_back(data[i]);
            else               child.data.push_back(d.data[i]);
        }
        
        return child;
    }
    
    
    
    void mutate(float m) {
        for (int i = 0; i < data.size(); i++) {
            if (ci::randFloat(1.0) < m) {

                    // emmitters
                for (int j = 0; j < data[i].size(); j++) {
                    data[i][j] = ci::randFloat(0, 160);
                }
                
                
            }
        }
    }

};


#endif /* EmmiterData_hpp */
