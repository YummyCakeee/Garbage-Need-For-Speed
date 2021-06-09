#version 330 core

in vec2 TexCoords;
out vec4 FragColor;
uniform sampler2D screenTexture;
uniform vec3 playerSpeed;

void main()
{	
	vec2 dir = normalize(vec2(0.5) - TexCoords);
    float samples[10];
    samples[0] = -0.008;
    samples[1] = -0.005;
    samples[2] = -0.003;
    samples[3] = -0.002;
    samples[4] = -0.001;
    samples[5] = 0.001;
    samples[6] = 0.002;
    samples[7] = 0.003;
    samples[8] = 0.005;
    samples[9] = 0.008;
  
	FragColor = texture(screenTexture,TexCoords); 

    if (length(playerSpeed) > 0.01)
    {
        vec4 sum = FragColor;

        for (int i = 0; i < 10; i++)
           sum += texture2D(screenTexture, TexCoords + dir * samples[i] * 1.0 );
        sum *= 1.0/11.0;
        float t = distance(vec2(0.5), TexCoords) * clamp(length(playerSpeed)/10.0, 0.0, 3.5);
        
        t = clamp( t ,0.0,1.0);
        FragColor = mix(FragColor, sum, t );
    }

    float gamma = 2.2f;
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0f/gamma));
}