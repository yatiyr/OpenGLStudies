# OpenGLStudies
To be a real graphics programmer, I need to understand OpenGL really well. This repo will help me track where I left.

# Update 13th March 2021 - BASIC LIGHTING

Made some progress with basic stuff.

![alt text](./gifs/specular.gif)

With playing materials

![alt text](./gifs/specular_material.gif)

# Update 14th March 2021 - Diffuse, Specular and Emission Maps

Different textures for diffuse, specular and emissive properties have been implemented in shader and application. with time uniform, green emission is animated.

![alt text](./gifs/textureMaps.gif)


Later that day, I was able to implement directional, point and spot lights

![alt text](./gifs/spot_light.gif)

# Midnight Update 15th March 2021 - Multiple Light sources

As in the tutorial

* there is one directional light
* one flashligt
* 4 point lights having different colors

![alt text](./gifs/multiple_lights.gif)


# Midnight Update 19th March 2021 - First Model

Model and mesh classes implemented using Assimp library. I can load models and apply glsl shaders and use it with my point lights, directional light and flashlight. 

Now, I guess I need to dive in some advanced stuff in OpenGL. At least tutorial says that :D

![alt text](./gifs/model.gif)


# Midnight Update 23th March 2021 - New Model Types

Before diving into advanced stuff, I saw that I have to learn how to add new models instead of the modified one in the tutorial. New scene contains;

  *  davy jones    => Batuhan13 (https://sketchfab.com/Batuhan13)
  *  backpack      => Berk Gedik (https://sketchfab.com/berkgedik)
  *  bristleback   => Nikolay_Tsys (https://sketchfab.com/Tolst)
  *  sponza scene  => kevin.kayu (https://sketchfab.com/kevin.kayu)

![alt text](./gifs/new_model_types.gif)
