#version 400 core

in vec2 uv;

uniform sampler2D tex;

out vec4 fragColor;

void main(){
    vec2 texelSize = 1.0 / textureSize(tex, 0).xy;

    const int supportWidth = 20;

    fragColor = vec4(0.0);
    float weights = 0.0;
    for (int i = -supportWidth; i <= supportWidth; i++) {
        // currently doing horizontal blur
        float weight = (supportWidth + 1) - abs(i);
        weights += weight;
        fragColor += weight * texture(tex, uv + vec2(i * texelSize.x, 0));
    }
    fragColor /= weights;

    // comment out this line to filter!
    //fragColor = texture(tex, uv);
}
