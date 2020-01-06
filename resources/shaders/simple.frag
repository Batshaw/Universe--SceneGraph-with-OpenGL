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

// Shades of light
vec3 ambient_color = vec3(0.5f);
vec3 specular_color = vec3(0.5f);
vec3 diffuse_color = vec3(0.5f);
vec3 point_light_pos = vec3(0.0f);

// Reflectivity
float reflec = 15.0f;
float shine = 20.0f;      // Intensity of the hightlight

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

  // Blinn-Phong: ambient + LightIntensity * diffuse * reflec + specular(Blinn)

  float diffuse_light = max(dot(h, normal_vector), 0) * LightIntensity;
  vec3 diffuse = diffuse_color * diffuse_light * LightColor;

  float specular_light = pow(max(dot(h, normal_vector), 0), 4) * LightIntensity;
  vec3 specular = specular_color * specular_light;
  
  out_Color = vec4((ambient_color + diffuse) * PlanetColor + specular * LightColor, 1.0f);
}
