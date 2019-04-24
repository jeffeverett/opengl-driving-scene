#version 400 core

// Note: the following based off of https://github.com/McNopper/OpenGL/blob/master/Example42/shader/fxaa.frag.glsl

// Inputs
in vec2 vTexCoords;

// Outputs
out vec4 fFragColor;

// Uniforms
uniform sampler2D colorTexture;

uniform vec2 texelStep;
uniform bool showEdges;

uniform float lumaThreshold;
uniform float mulReduce;
uniform float minReduce;
uniform float maxSpan;

// see FXAA
// http://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf
// http://iryoku.com/aacourse/downloads/09-FXAA-3.11-in-15-Slides.pdf
// http://horde3d.org/wiki/index.php5?title=Shading_Technique_-_FXAA

void main(void)
{
  vec3 rgbM = texture(colorTexture, vTexCoords).rgb;

  // Sampling neighbour texels. Offsets are adapted to OpenGL texture coordinates. 
  vec3 rgbNW = textureOffset(colorTexture, vTexCoords, ivec2(-1, 1)).rgb;
  vec3 rgbNE = textureOffset(colorTexture, vTexCoords, ivec2(1, 1)).rgb;
  vec3 rgbSW = textureOffset(colorTexture, vTexCoords, ivec2(-1, -1)).rgb;
  vec3 rgbSE = textureOffset(colorTexture, vTexCoords, ivec2(1, -1)).rgb;

  // see http://en.wikipedia.org/wiki/Grayscale
  const vec3 toLuma = vec3(0.299, 0.587, 0.114);

  // Convert from RGB to luma.
  float lumaNW = dot(rgbNW, toLuma);
  float lumaNE = dot(rgbNE, toLuma);
  float lumaSW = dot(rgbSW, toLuma);
  float lumaSE = dot(rgbSE, toLuma);
  float lumaM = dot(rgbM, toLuma);

  // Gather minimum and maximum luma.
  float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
  float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

  // If contrast is lower than a maximum threshold ...
  if (lumaMax - lumaMin <= lumaMax * lumaThreshold) {
    // ... do no AA and return.
    fFragColor = vec4(rgbM, 1.0);

    return;
  }

  // Sampling is done along the gradient.
  vec2 samplingDirection;	
  samplingDirection.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
  samplingDirection.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

  // Sampling step distance depends on the luma: The brighter the sampled texels, the smaller the final sampling step direction.
  // This results, that brighter areas are less blurred/more sharper than dark areas.  
  float samplingDirectionReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.25 * mulReduce, minReduce);

  // Factor for norming the sampling direction plus adding the brightness influence. 
  float minSamplingDirectionFactor = 1.0 / (min(abs(samplingDirection.x), abs(samplingDirection.y)) + samplingDirectionReduce);

  // Calculate final sampling direction vector by reducing, clamping to a range and finally adapting to the texture size. 
  samplingDirection = clamp(samplingDirection * minSamplingDirectionFactor, vec2(-maxSpan), vec2(maxSpan)) * texelStep;

  // Inner samples on the tab.
  vec3 rgbSampleNeg = texture(colorTexture, vTexCoords + samplingDirection * (1.0/3.0 - 0.5)).rgb;
  vec3 rgbSamplePos = texture(colorTexture, vTexCoords + samplingDirection * (2.0/3.0 - 0.5)).rgb;

  vec3 rgbTwoTab = (rgbSamplePos + rgbSampleNeg) * 0.5;  

  // Outer samples on the tab.
  vec3 rgbSampleNegOuter = texture(colorTexture, vTexCoords + samplingDirection * (0.0/3.0 - 0.5)).rgb;
  vec3 rgbSamplePosOuter = texture(colorTexture, vTexCoords + samplingDirection * (3.0/3.0 - 0.5)).rgb;

  vec3 rgbFourTab = (rgbSamplePosOuter + rgbSampleNegOuter) * 0.25 + rgbTwoTab * 0.5;   

  // Calculate luma for checking against the minimum and maximum value.
  float lumaFourTab = dot(rgbFourTab, toLuma);

  // Are outer samples of the tab beyond the edge ... 
  if (lumaFourTab < lumaMin || lumaFourTab > lumaMax) {
    // ... yes, so use only two samples.
    fFragColor = vec4(rgbTwoTab, 1.0); 
  }
  else {
    // ... no, so use four samples. 
    fFragColor = vec4(rgbFourTab, 1.0);
  }

  // Show edges for debug purposes.
  if (showEdges) {
    fFragColor = vec4(1.0, 0.0, 1.0, 1.0);
  }
}