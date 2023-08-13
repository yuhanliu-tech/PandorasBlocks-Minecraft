# Mini-Minecraft: Pandora's Blocks

Final Demo Video: https://vimeo.com/820781978?share=copy

The overall goal of this project is to create an interactive 3D world exploration and alteration program in the style of the popular computer game Minecraft. 
Inspired by Avatar's Pandora and created by Claire Lu, Diana Ouyang, and Yuhan Liu 

## Milestone 3

### Claire Lu 

* Additional Biomes 

	* Two additional biomes using fbm noise and perlin noise

	* Water Mountains

		* Large bodies of water and hills that are submerged in water; also incorporated new dark dirt block types for this biomes.

	* Spiky Mountains

		* Steep mountains that incorporate three new block types. 

		* A noise function was used to make the transition between the three blocks less uniform, as well as a generated random number. 

	* Biome Mixing (6 biomes mixed)

		* I placed all 6 biomes on a grid. I generated values for temperature and moisture levels for each biome using
two different noise functions. Both were based on fbm and perlin noise. I made them different by distorting the 
noise by creating offset values and then changing the input based on this. For temperature and moisture, I created 
threshold values so that if the temp and moisture level of the player's position falls within the threshold values,
I would interpolate the heights among the appropriate surrounding biomes. Otherwise, I would use 100% of the current
biome height. I used smoothstep to calculate the weights of each biome when interpolating among biomes.


### Diana Ouyang

* Shadow Mapping

	* KIND of a beast; depth pass and stuff (thank you Aditya) 

	* Day/Night-cycle dependent; dependent on time of day/position on the sun
	
		* Dynamic biasing & daytime/nighttime buffer value to indicate shadow modes/which light source to reference 

	* Dependency on player position/movement 

	* Different shadow intensities depending on time of day

* Perspective Depth of Field Buffer

	* Gaussian blur based on nearest object and idle time (where is the camera pointing + how long have we been idle?)

	* Passing in time of idling and nearest block using gridmarch

	* Increasing intensity of blur the longer you "stare" 

* Distance Fog Frame Buffer

	* Also based on distance; blended on the final overlay pass

	* Done in the same pass as perspective depth of field

	* Two passes to not see the sun through mountains  

* Final Post-Process Frame Buffer (Overlay)

	* Brings together all the perspective depth and funny stuff (gaussian blur)

	* Lerps between distance fog and distance value

	* Gaussian blurs depending on distance/depth of field map  

* Additional Wiggle styles

	* Static vertical base wiggles for vines, tree-leaves, etc.

	* Making sure wiggling doesnt create holes in our terrain 

* Stars (preeettty) 

	* Worley noise brrr; Day/night cycle dependent

	* Separate frame buffer pass so they dont screw up the distance fog

* Floating Islands + Tianzi Biome modification

	* floating islands are so funny !! hallelujah mountains

	* Done with a second pass of worley noise; with different tops and bottoms; 

	* Hanging vines underneath the island 

* Water Waves and Depth-based Water/Lava Color

* Minor details

	* Floaty particles (they just hover) to make things magical

### Yuhan Liu

* Procedural Sky

	* Procedural sky background drawn using the raycast method

	* Day/night cycle, including sun and moon that move across sky and time of day color blending 

	* Palettes for times of day that interpolate based on look vector. Color also gradiates depending on proximity to horizon 

* L-Systems (Shape-Grammar Trees/Logs)

	* Implemented turtle class

	* Implemented L-system tree class that parses axioms and drawing rules

	* Uses Bresenham's line algorithm to create blocks out of L-system decided coordinates

	* L-System Tree: tree with procedural vines hanging down from branches

	* L-System Mossy Overgrown Log: flat-lying log with mossy texture and procedural plants 

* Procedurally Placed Assets

	* Small coded objects that spawn procedurally as chunks are drawn 

	* Cave Environments: procedurally-placed cave plants and cave vines (vary in height, location, etc) 

	* Plants: biome-specific i.e. blue plants in spiky mountains, lily flowers in islands biome 

	* Hard-coded trees that grow on grass, hard-coded palm trees that grow on sand

* Custom Textures

	* Responsible for drawing all non-Minecraft-specific textures in Procreate and uploading to Mini-Minecraft

## Milestone 2 

### Claire Lu 

* Multithreading 

	* Implemented multithreading by having fbm and vbo spawners and then mutexes that lock and unlock threads. 

	* create terrain zone border blocks based on prev and curr pos.

	* checks if a terrain zone exists, if so, creates fbm for a zone if it does not have block type data. 

	* if terrain zone does have block type data then create vbo data

### Diana Ouyang

* Textures

	* Made ways to buffer textures (and normal maps) into the shaderprograms

	* Added new Textures (Yuhan's making) 

	* Normal mapping is kind of hard

	* Split buffering into opaque and transparent blocks :) 

* Texture Animation 

	* Implemented different water depth values; as well as water deformation, also added water specularity calculations

	* added a vec4 buffer for each vertex for SFX (specifies whether the block glows, wiggle, etc.) 

	* Made X-Blocks (like Grass) and made them WIGGLE

		*New block type map to tell the cube buffers how many faces to buffer and stuff!

### Yuhan Liu

* Caves

	* Extends 2D Perlin noise function to 3D to implement caves (sampling eight surflets + trilinear interpolation). 

	* Layer of unbreakable bedrock at y = 0, layers of lava up to y = 20. 

* Underwater/lava Overlay

	* When the player's Camera is inside a WATER block, the screen is tinged blue, while a LAVA block causes a red tinge.

	* Implemented post-processing effects using a new overlay shader program and the frame buffer class. 

* Underwater/lava Physics

	* Water/lava will not collide with player. Instead, new physics rules are implemented to allow the player to swim. 

## Milestone 1 

### Claire Lu 

* Player Physics

	* Implemented the flight mode and ground mode by updating the input key pressed in mygl. Then, based on these values, I updated the value of acceleration in processInputs based on which keys were pressed. Then, in computePhysics, I used these values and whether or not the player was in flight mode to update velocity.

	* In computePhysics, I implemented collision detection. I used grid marching to see whether there was a collision or not. The outDist variable acts as a threshold and I use it to check if its value is beyond a certain threshold. 

	* For building and removing blocks, I used grid marching to cast a ray and see if it intersectsany blocks within 3 units. 

	* For building: If it does, the gridmarchblockbefore function returns the block before the intersection (i.e. the adjacent block). This block then changes type.

	* For removing: If gridmarch returns true, then the block type returned will be set to empty.

### Diana Ouyang

* Procedural Noise Terrain

	* I did most of my noise generation tweaking on the instancedVertex shader and then continued later with chunk

	* All terrain generation functions (put in an xz coordinate, calculate a height) are part of the TerrainNoise namespace

	* There are some general noise functions, such as fbm and perlinNoise2D which are pretty much word for word the same as given to us in the slides

	* There are also biome-specific noise generation functions: more specifically, for four biomes; though for this milestone we'll only be using the two dry terrains: calcYOffsetHills() and calcYOffsetMountains(). The other two, which are wet terrains, will be implemented in the future when i am not dying inside ;(

	* Height-based bioming (which are only height-dependent rn) are calculated using an FBM function. I forget the exact details but i think it's like either black for high and white for low or the other way around. The appropriate block per biome is determined by getAppropriateBiomeBlock() which takes in whether the biome is high and returns the correct blocktype depending on whether it's the tallest block or not

### Yuhan Liu

* Chunking and Efficient Rendering

	* Make chunk inherit from Drawable + implement create(). 

	* Chunk creates VBO data by iterating through all the blocks in the chunk. In each iteration, if the current block is not empty, we need to check which faces to render. The function checks neighbors to see if they're in the same chunk, and if not, if they exist in a neighboring chunk. After checking for the neighbor in each direction, if that neighbor is empty, the per-vertex data is buffered (one pos, nor, and col for each face). 

	* Store per-vertex data in interleaved VBOs and Make updates to drawable, shaderprogram in order to process interleaved VBOs. 

	* Terrain expansion function attempts to add chunks to every direction of the current chunk. A helper checks to see if the chunk exists, if not, it's instantiated using existing functions and its neighbors' VBOs are updated. 
