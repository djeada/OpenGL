## Question
I sort of knew this was the case from making polygon meshes from 3D scanning/point maps. The light vectors from the light source bounce off these polygons to render them to the user.

Anyways,

* In video games, the computer doesn't need to recompute every surface for every frame, it only recomputes for objects that have moved. How does the graphics processor "know" what to redraw? Is this held in VRAM or something?

* When people talk about computer graphics being "triangles," is this what they're talking about? Does this only work for polygonal graphics?
* Are the any other rendering techniques a beginner needs to know about? Surely we didn't go from bitmap -> raster graphics -> vector graphics -> polygons.

## Answer 1

Usual consumer-grade 3D video rendering does not use raytracing, but rather rasterizing. They just project a 3D triangle into a 2D "viewport" surface, and then assign this a color based on texture and some other local parameters. For more information, see the Wikipedia article on the graphics pipeline.

Most video games re-render the the entire screen all the time. In particular, you need to do this anyway when the camera moves, and since most games don't expect you to not move for extended periods of time, there is no point in optimizing for this. Apart from this, the graphics processor "knows" that it should re-render something because the application tells it to. The application of course knows what needs to be re-rendered because it's programmed by someone with a brain. I unfortunately don't know how video games manage to efficiently transform high-polygon character models to e.g. make their arms move realistically.

All models using the above technique, and most models in general are just polygons, but the trianges are so small that you do not notice. While raytracing allows you to render some other objects like mathematically perfect spheres, this is rarely used

There are several approaches to getting colors onto a screen, depending on your hardware
* Early consumer hardware just did 2D rendering
* You now have more fancy consumer hardware that does 3d rasterization
* ray tracing is used for "high end" graphics that need to look great. Some consumer GPUs are supposed to do be able to ray-trace games in real time, but it's rather new and I'm not sure how much it is used
* vector graphics are usually associated with "infinite-zoom" 2D images
* 
## Answer 2

1) In video games, the computer doesn't need to recompute every surface for every frame, it only recomputes for objects that have moved

No, modern video games redraw everything at each frame, including static objects (but only things visible, of course, and there are some optinmisations). However, they don't use raytracing (yet?). They draw a bunch of triangles, multiple times, from multiple angles with various shader applied to vertices and pixels and combination of the resulting buffers. It is extremely sophisticated..

2) When people talk about computer graphics being "triangles," is this what they're talking about?

The core primitive of a GPU is to draw series of triangles. The window displaying the content of this web page is probably displayed by your computer as two triangles, with a texture that is the content (and another one to manage the rounded corners/shadows). Your complex video game is a bunch of 3d triangles for everything.

3) Are the any other rendering techniques a beginner needs to know about? Surely we didn't go from bitmap -> raster graphics -> vector graphics -> polygons.

Not sure what this means. we didn't go bitmap -> raster -> vector -> polygons (for instance, vector was before bitmap as it needs less memory and maps well to cathodic tube rendering), so the question makes little sense to me. There are many rendering techniques, but right now you have ray tracing for high quality shadow/reflections, and rasterization for real-time. There are also things like radiosity, but rendering is a very large subject, so open-ended questions are not very useful there... depends on what that beginner wants to concentrate on
