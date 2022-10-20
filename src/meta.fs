#version 330

in vec2 fragTexCoord;                    // screen position <-1,+1>
in vec4 fragColor;

uniform sampler2D texture0;          // texture to blur
uniform vec4 colDiffuse;

out vec4 finalColor;          // fragment output color
vec2 iResolution = vec2(1200);

const float Pi = 6.28318530718; // Pi*2

// GAUSSIAN BLUR SETTINGS {{{
const float Directions = 16.0; // BLUR DIRECTIONS (Default 16.0 - More is better but slower)
const float Quality = 4.0; // BLUR QUALITY (Default 4.0 - More is better but slower)
const float Size = 8.0; // BLUR SIZE (Radius)
const vec4 Col = vec4(0.0, 0.0, 0.1, 1.0);
// GAUSSIAN BLUR SETTINGS }}}

//---------------------------------------------------------------------------
void main()
{
    vec2 Radius = Size/iResolution.xy;

    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragTexCoord;
    // Pixel colour
    vec4 currentColor = texture(texture0, uv);

    // Blur calculations
    for( float d=0.0; d<Pi; d+=Pi/Directions)
    {
        for(float i=1.0/Quality; i<=1.0; i+=1.0/Quality)
        {
            currentColor += texture( texture0, uv+vec2(cos(d),sin(d))*Radius*i);
        }
    }

    // Output to screen
    currentColor /= Quality * Directions - 15.0;

    if (currentColor.r < 0.5) {
        finalColor = Col;
    } else {
        finalColor = vec4(0.0);
    }
    // finalColor = vec4(currentColor.rgb, 1.0);
}
