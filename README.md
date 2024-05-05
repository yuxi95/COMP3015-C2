# Idea:
This time I continued to use the last model, choosing the Low Poly style and cartoon-style coloring. The ideas came from dolphin pirates and Pac-Man. This time, an idea was implemented to gain points by moving the dolphin treasure hunt up, down, left, and right through keyboard control.
# Gameplay:
The player controls the dolphin (using the direction keys) and scores points when touching the block objects in the scene. The score is related to the remaining time for the block to be reset (100 + remaining time * 100). The total score is printed in the window title, every certain time (2 ~3s), the block is reset to a random position and will move immediately when touched by the player.
# initialization:
Create a normal map:normal_texutre ,
Create a projection map:proj_texture,

Generate FBO (frame buffer object), set FBO to only write depth, and shadow only needs depth information
Generate depth-texture, using float format to meet the accuracy required for shadow calculation
Bind depth-texture to depth-attachment of FBO
Use std::vector to save objects that produce shadows in the scene

# Frame start -- Game logic:

Activate FBO and draw depth map
Traverse the scene object vecotr, use depth.vert/frag shader to draw the scene from the light perspective, and obtain the depth map of the light direction.

Cancel the FBO and draw the final scene
Traverse the scene object vecotr, use shadowmap.vert/frag shader to draw the scene from a normal perspective, use the depth map obtained in the previous step combined with the shadow-map algorithm to generate shadows, use normal_texutre to process the normal map, and use the light matrix projection map proj_texture

shadow map viewprot
The generation of the depth map itself belongs to render-to-texture technology. We can set up a 128x128 viewport and then draw a screen-quad of the depth map to view the texture.

# Input processing:
Press the number key 8 to switch the shadow-map rendering pipeline, and the four direction keys control the dolphin
KEY_1:
  Diffuse
KEY_2:
  Specular
KEY_3:
  SpotLight
KEY_4:
  ToonShading
KEY_5:
  FlatShading 
KEY_6:
	  	Fog 
KEY_7:
	  	PBR 
KEY_8:
	  	Shadow
 KEY_SPACE:
	  	Animation
  up down left right control the dolphin move
