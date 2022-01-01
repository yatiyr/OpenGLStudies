#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <ShaderProgram.h>

class Object
{
public:
    Object()
    {

    }

    ~Object()
    {

    }

    virtual void Draw(ShaderProgram &shader) = 0;




private:
    bool objectHidden;
}



#endif