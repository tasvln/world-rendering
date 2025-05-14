#version 330 core

// Main grid and sub-grid rendering shader

// screen-space line thickness calculations
// `grid` represents the main grid spacing, while `subGrid` represents the smaller sub-grid inside each square
// The `fade` effect is applied to lines based on their distance from the camera

// Uniforms
uniform vec3 cameraPosition; // Camera position for fade effect
uniform float spacing; // Main grid spacing

// Inputs/Outputs
in vec3 worldPos;
out vec4 FragColor;

void main()
{
  // screen-space line thickness
  vec2 dx = vec2(dFdx(worldPos.x), dFdy(worldPos.x));
  vec2 dy = vec2(dFdx(worldPos.z), dFdy(worldPos.z));
  vec2 dudv = vec2(length(dx), length(dy));

  // Main grid position
  float scale = 2.0; // Scaling factor for line thickness
  vec2 grid = abs(fract(worldPos.xz / spacing * scale));

  // Sub-grid position (inside each square)
  // Smaller grid inside main grid
  float subGridSpacing = spacing * 0.25;
  vec2 subGrid = abs(fract(worldPos.xz / subGridSpacing * scale));

  // Calculate minimum distance to both grid lines (main and sub-grid)

  // grid line distance
  float line = min(grid.x, grid.y);
  // Sub-grid line distance
  float subLine = min(subGrid.x, subGrid.y);

  // Line thickness and anti-aliasing
  float thickness = 0.008;
  float aa = max(fwidth(line), 0.005);

  float axisThickness = 0.004;
  // Axis lines (X in red, Z in blue)
  bool isXAxis = abs(worldPos.z) < spacing * axisThickness;
  bool isZAxis = abs(worldPos.x) < spacing * axisThickness;

  // Distance from the camera for fade calculation
  float dist = length(worldPos.xz - cameraPosition.xz);

  // Fade effect based on distance from the camera
  float fade = 1.0 - smoothstep(80.0, 100.0, dist);

  // Axis lines (Red for X, Blue for Z)
  // Green line for X-axis
  if (isXAxis) {
    FragColor = vec4(vec3(0.0, 0.8, 0.0), fade);
    return;
  }
  // Red line for Z-axis
  if (isZAxis) {
    FragColor = vec4(vec3(0.8, 0.0, 0.0), fade);
    return;
  }

  // Fade for both main grid and sub-grid lines
  float gridFade = smoothstep(thickness + aa, thickness - aa, line);
  float subGridFade = smoothstep(thickness + aa, thickness - aa, subLine);

  // Colors for grid lines and background
  // Main grid lines color (darker)
  vec3 gridColor = vec3(0.1);
  // Sub-grid lines color (lighter)
  vec3 subGridColor = vec3(0.2);
  // Background color between grid lines
  vec3 backgroundColor = vec3(0.4);

  // Combine both grid and sub-grid lines
  vec3 finalColor = mix(gridColor, subGridColor, subGridFade);
  finalColor = mix(finalColor, backgroundColor, gridFade);

  // Apply fade effect to grid lines
  FragColor = vec4(finalColor, fade);
}
