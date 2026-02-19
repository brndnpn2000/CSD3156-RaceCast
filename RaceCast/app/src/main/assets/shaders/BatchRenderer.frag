#version 310 es
precision mediump float;

in vec4 vColor;
in vec2 vTexCoords;
in float vTexIndex;

// Array of texture samplers (matches m_max_texture_slots = 8)
uniform sampler2D uTextures[8];

out vec4 fragColor;

void main() {
    vec4 texColor;
    int index = int(vTexIndex);

    // Manual sampling switch for maximum hardware compatibility
    switch(index) {
        case 0: texColor = texture(uTextures[0], vTexCoords); break;
        case 1: texColor = texture(uTextures[1], vTexCoords); break;
        case 2: texColor = texture(uTextures[2], vTexCoords); break;
        case 3: texColor = texture(uTextures[3], vTexCoords); break;
        case 4: texColor = texture(uTextures[4], vTexCoords); break;
        case 5: texColor = texture(uTextures[5], vTexCoords); break;
        case 6: texColor = texture(uTextures[6], vTexCoords); break;
        case 7: texColor = texture(uTextures[7], vTexCoords); break;
        default: texColor = vec4(1.0, 0.0, 1.0, 1.0); break; // Fallback to Magenta
    }

    // Combine texture color with vertex color
    fragColor = texColor * vColor;
}