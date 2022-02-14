#ifndef __RADIAL_SPHERE_H__
#define __RADIAL_SPHERE_H__


#include <Geometry/Sphere.h>

class RadialSphere : public Sphere
{
public:
    RadialSphere(float radius=1.0f, int sectorCount=64, int stackCount=64, bool smooth = true);
    ~RadialSphere() {}

    float GetSectorCount()      const {return sectorCount;}
    float GetStackCount()       const {return stackCount;}
    
    void Set(float radius, int sectorCount, int stackCount);
    void SetRadius(float radius);
    void SetSectorCount(int sectorCount);
    void SetStackCount(int stackCount);

    void PrintSelf() const;

private:

    virtual void BuildVertices();
    virtual void BuildVerticesFlat();
                                                   
    // member variables
    int sectorCount;
    int stackCount;

    
};






#endif