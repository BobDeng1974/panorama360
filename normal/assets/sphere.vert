/*
 * This proprietary software may be used only as
 * authorised by a licensing agreement from ARM Limited
 * (C) COPYRIGHT 2012 ARM Limited
 * ALL RIGHTS RESERVED
 * The entire notice above must be reproduced on all authorised
 * copies and copies may only be made to the extent permitted
 * by a licensing agreement from ARM Limited.
 */

attribute vec4 av4position;
uniform mat4 mvp;
attribute mediump vec2 myUV;
varying vec2 myTexCoord;
void main()
{
	myTexCoord = myUV;
    gl_Position = mvp * av4position; 
}
