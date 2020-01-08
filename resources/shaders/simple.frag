#version 150

// input from vertex
in  vec3 pass_Normal;
in vec3 fragment_pos;
in vec3 camera_pos;

// output color
out vec4 out_Color;

uniform vec3 PlanetColor;
uniform float LightIntensity;
uniform vec3 LightColor;
uniform bool CellShadingMode;

// Shades of light
vec3 ambient_color = vec3(1.0f, 0.97f, 0.91f);    // Cosmic latte
vec3 point_light_pos = vec3(0.0f);                // position of the sun holder

// Reflectivity
float shine = 15.0f;      // Intensity of the hightlight

float ambient_intensity = 0.8f;
float specular_intensity = 0.5f;
float diffuse_intensity = 0.6f;
float outline = 1.0f;

void main() {
  //out_Color = vec4(abs(normalize(pass_Normal)), 1.0);
  // out_Color = vec4(PlanetColor, 1.0f);

  vec3 normal_vector = normalize(pass_Normal);

  // Vector from pixel to PointLight
  vec3 incoming_ray = normalize(point_light_pos - fragment_pos);

  // Vector from pixel to camera
  vec3 camera_ray = normalize(camera_pos - fragment_pos);
  // direction of the reflection
  vec3 h = normalize(incoming_ray + camera_ray);

  // Blinn-Phong: (ambient + diffuse) * PlanetColor + specular(Blinn)

  float diffuse_light = max(dot(normal_vector, incoming_ray), 0) * LightIntensity;
  vec3 diffuse = diffuse_intensity * diffuse_light * ambient_color;               //diffuse color = ambient color

  float specular_light = pow(max(dot(h, normal_vector), 0), shine) * LightIntensity;
  vec3 specular = specular_intensity * LightColor * specular_light;               // specular color is white = light color

  vec3 ambient = ambient_intensity * ambient_color;
  
  if(!CellShadingMode) {
    out_Color = vec4((ambient + diffuse) * PlanetColor + specular, 1.0f);
  }
  else {
    vec3 CellColor = floor(PlanetColor * 3);   // 3 levels of brightness
    CellColor = CellColor / 3;
    /*
    if(max(dot(normal_vector, incoming_ray), 0.0f) < 0.05f) {    // outline the vertex if the normal vector and the vector from vertex to camera is almost perpendicular
      outline = 0.0f;
    }
    */
    out_Color = vec4((ambient + diffuse) * CellColor + specular, 1.0f) * outline;
  }
}
