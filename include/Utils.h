#ifndef __UTILS__
#define __UTILS__

#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>

// flip loaded textures images vertically
// according to parameter val
static void setVerticalFlip(bool val)
{
    stbi_set_flip_vertically_on_load(val);
}

// to prevent segmentation faults when reading images
static void pixelStoreICalls() {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);    
}

// TODO : it may change for different texture parameters
// for now it does not matter.
static unsigned int loadTexture(const char* path, int rgbSetting)
{

    unsigned int texture;
    
    // texture initialization
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // set wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load and generate the texture and produce mipmaps
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, rgbSetting, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        // may add error throwing in the future
        std::cout << "Failed to load texture" << '\n';
    }
    
    stbi_image_free(data);

    return texture;
}



#endif