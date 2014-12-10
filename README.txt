<--------------------Instructions-------------------------------------->

Can be made using the 'make' command.

<---------------------Credits------------------------------------------>
The textures are courtesy of filterforge.com,
https://filterforge.com/filters/
and from quite a few other random internet sources.

The object loader is borrowed from Prof. Schreuder.

The wooden logs are attributed to
Emmanuel PUYBARET / eTeks <info@eteks.com>  and
Scopia Visual Interfaces Systems, s.l. (http://www.scopia.es)
under the Creative Commons Attribution 3.0 Unported license.

A PNG loading library called lodePng is used.
http://lodev.org/lodepng/

What was previously loadtexbmp.c is now "loadtex.c" and has loadPng
included inside it.

<----------------------Project Awesomeness--------------------------------->

I'm particularly proud of the fact that I got a lot of alpha based things to play
nicely with each other. There's a neat trick of keeping the depth test on but
not writing to the depth buffer that I used. I'm also proud of how fun it is to play
around with the particles and watch how they react to different modes.


<-------------------------Controls------------------------------------->

Viewing Toggles:

'r': Changes the mode the fire uses, there are 5 modes in total

't and T' : Increments/Decrements the red component of the starting color
'y and Y' : Increments/Decrements the green component of the starting color
'u and U' : Increments/Decrements the blue component of the starting color

'g and G' : Increments/Decrements the red component of the ending color
'h and H' : Increments/Decrements the green component of the ending color
'j and J' : Increments/Decrements the blue component of the ending color


Standard WASD movement keys are used.
The mouse or the arrow keys can be used to direct movement.

Forward movement is the in the direction of looking.
