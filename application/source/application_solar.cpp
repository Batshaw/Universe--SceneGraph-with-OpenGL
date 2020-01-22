#include "application_solar.hpp"
#include "window_handler.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding 
using namespace gl;
// typedef std::shared_ptr<Node> node_ptr;
const double PI = 3.14159265358979323846;
//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

ApplicationSolar::ApplicationSolar(std::string const& resource_path)
 :Application{resource_path}
 ,planet_object{}
 ,star_object{}
 ,star_container{}
 ,orbit_object{}
 ,orbit_container{}
 ,m_view_transform{glm::translate(glm::fmat4{}, glm::fvec3{0.0f, 0.0f, 75.0f})}
 ,m_view_projection{utils::calculate_projection_matrix(initial_aspect_ratio)}
 ,CellShadingMode{false}  // default NORMAL MODE
{
  initializeGeometry();
  init_planets();
  init_stars();
  init_orbits();
  init_textures();
  initializeShaderPrograms();
}

ApplicationSolar::~ApplicationSolar() {
  glDeleteBuffers(1, &planet_object.vertex_BO);
  glDeleteBuffers(1, &planet_object.element_BO);
  glDeleteVertexArrays(1, &planet_object.vertex_AO);

  // delete star_object for deconstructors
  glDeleteBuffers(1, &star_object.vertex_BO);
  glDeleteVertexArrays(1, &star_object.vertex_AO);
  // delete orbit_object
  glDeleteBuffers(1, &orbit_object.vertex_BO);
  glDeleteVertexArrays(1, &star_object.vertex_AO);
}

void ApplicationSolar::render() const {
 // default render from framework
  // bind shader to upload uniforms
  // glUseProgram(m_shaders.at("planet").handle);
/*
  // tesst one planet
  Node* planet_ptr = new Node("planet_1");
  glm::fmat4 model_matrix = planet_ptr->getWorldTransform();
  model_matrix = glm::rotate(model_matrix, float(glfwGetTime()), glm::fvec3{0.0f, 1.0f, 0.0f});
  model_matrix = glm::translate(model_matrix, glm::fvec3{0.0f, 0.0f, -1.0f});
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                     1, GL_FALSE, glm::value_ptr(model_matrix));

  // extra matrix for normal transformation to keep them orthogonal to surface
  glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                     1, GL_FALSE, glm::value_ptr(normal_matrix));

  // bind the VAO to draw
  glBindVertexArray(planet_object.vertex_AO);

  // draw bound vertex array using bound shader
  glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
*/
  std::list<Node*> const our_solar_system = scene_graph->getRoot()->getChildrenList();
  render_planets(our_solar_system);  
  render_stars();
}

void ApplicationSolar::render_planets(std::list<Node*> const& scene_children_list) const {
  // go through children list
  for(auto const& planet_ptr : scene_children_list) {
    auto children_of_planet = planet_ptr->getChildrenList();
    if(!children_of_planet.empty()) {
      render_planets(children_of_planet);   // recursive
    }
    // ignore the holder and camera
    if(planet_ptr->getDepth() % 2 == 0 || planet_ptr->getName().find("cam") == std::string::npos) {
      // call render_orbits here so each planet will have its own orbit
      render_orbits(planet_ptr);
      //transform of the planet      
      glm::fmat4 transform_matrix = compute_transform_matrix(planet_ptr);
      // extra matrix for normal transformation to keep them orthogonal to surface
      glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * transform_matrix);
      
      glUseProgram(m_shaders.at("planet").handle);
      glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                     1, GL_FALSE, glm::value_ptr(transform_matrix));
      glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                     1, GL_FALSE, glm::value_ptr(normal_matrix));

      //glUniform3f
      glUniform3f(m_shaders.at("planet").u_locs.at("PlanetColor"),
                  planet_ptr->getColor().x, planet_ptr->getColor().y, planet_ptr->getColor().z);
      // upload Uniform3f with lightIntensity and lightColor
      glUniform1f(m_shaders.at("planet").u_locs.at("LightIntensity"), sun_1_light->gettLightIntensity());
      glUniform1i(m_shaders.at("planet").u_locs.at("CellShadingMode"), CellShadingMode);
      glUniform3f(m_shaders.at("planet").u_locs.at("LightColor"),
                  sun_1_light->getLightColor().x, sun_1_light->getLightColor().y, sun_1_light->getLightColor().z);

      // upload textures to shader
      glActiveTexture(GL_TEXTURE0);   // active texture
      glBindTexture(GL_TEXTURE_2D, planet_ptr->getTextureObject().handle);  // bind texture to shader for accessing
      glUniform1i(glGetUniformLocation(m_shaders.at("planet").handle, "PlanetTexture"), 0);    // 0 is texture slot in active
      // bind the VAO to draw
      glBindVertexArray(planet_object.vertex_AO);
      // draw bound vertex array using bound shader
      glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);      
   }
  }
}
glm::fmat4 ApplicationSolar::compute_transform_matrix(Node* const& planet_ptr) const {
  glm::fmat4 transform_matrix = planet_ptr->getWorldTransform();
  // moon's rotation around its origin
  if(planet_ptr->getDepth() == 4) {
    glm::fmat4 origin_matrix = planet_ptr->getOrigin()->getLocalTransform();
    transform_matrix = glm::rotate(origin_matrix, float(glfwGetTime() * planet_ptr->getOrigin()->getSpeed()), glm::fvec3{0.0f, 1.0f, 0.0f});
    transform_matrix = glm::translate(transform_matrix, -1.0f * planet_ptr->getOrigin()->getDistanceToOrigin());
  }
  // rotation around parent_planet
  transform_matrix = glm::rotate(transform_matrix * planet_ptr->getLocalTransform(), float(glfwGetTime() * planet_ptr->getSpeed()), glm::fvec3{0.0f, 1.0f, 0.0f});
  transform_matrix = glm::translate(transform_matrix, -1.0f * planet_ptr->getDistanceToOrigin());
  // rotation around itself
  transform_matrix = glm::rotate(transform_matrix, float(glfwGetTime()) * 1.5f, glm::fvec3{0.0f, 1.0f, 0.0f});
  // scale to fix the size of planets
  glm::fvec3 scale_vector = {planet_ptr->getRadius(), planet_ptr->getRadius(), planet_ptr->getRadius()};
  transform_matrix = glm::scale(transform_matrix, scale_vector);

  return transform_matrix;
}

void ApplicationSolar::render_stars() const {
  // bind the VAO to draw
  glBindVertexArray(star_object.vertex_AO);
  // bind shader to upload uniforms
  glUseProgram(m_shaders.at("stars").handle);
  // draw bound vertex array using bound shader
  // (DRAW_MODE: prim_type, start_index, count: number of indices to be rendered)
  glDrawArrays(star_object.draw_mode, 0, star_object.num_elements);
}

void ApplicationSolar::render_orbits(Node* const& child_planet) const {
  float radius = child_planet->getDistanceToOrigin().x;
  glm::fvec3 vec_to_scale = {radius, radius, radius};
  glm::fmat4 orbit_matrix = glm::fmat4{};

  //rotation of the orbits of the moon, so it moves like the parent planet
  if(child_planet->getDepth() == 4) {
    glm::fmat4 parent_matrix = child_planet->getOrigin()->getLocalTransform();
    orbit_matrix = glm::rotate(parent_matrix, float(glfwGetTime()) * (child_planet->getOrigin()->getSpeed()), glm::fvec3{0.0f, 1.0f, 0.0f});
    orbit_matrix = glm::translate(orbit_matrix, -1.0f * child_planet->getOrigin()->getDistanceToOrigin());
  } 
  // scale the orbit with the radius
  orbit_matrix = glm::scale(orbit_matrix * child_planet->getLocalTransform(), vec_to_scale);

  // bind shader to upload uniform
  glUseProgram(m_shaders.at("orbits").handle);
  // matrixes to shader
  glUniformMatrix4fv(m_shaders.at("orbits").u_locs.at("OrbitMatrix"),
                      1, GL_FALSE, glm::value_ptr(orbit_matrix));
  // bind VAO to draw
  glBindVertexArray(orbit_object.vertex_AO);
  // draw bound vertex array using bound shader
  glDrawArrays(orbit_object.draw_mode, 0, planet_object.num_elements);
}

void ApplicationSolar::uploadView() {
  // vertices are transformed in camera space, so camera transform must be inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);
  // upload matrix to gpu
  glUseProgram(m_shaders.at("planet").handle);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));

  glUseProgram(m_shaders.at("stars").handle);
  glUniformMatrix4fv(m_shaders.at("stars").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));                     
  
  glUseProgram(m_shaders.at("orbits").handle);
  glUniformMatrix4fv(m_shaders.at("orbits").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));   
}

void ApplicationSolar::uploadProjection() {
  // upload matrix to gpu
  glUseProgram(m_shaders.at("planet").handle);
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));

  glUseProgram(m_shaders.at("stars").handle);
  glUniformMatrix4fv(m_shaders.at("stars").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));         

  glUseProgram(m_shaders.at("orbits").handle);
  glUniformMatrix4fv(m_shaders.at("orbits").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection)); 
}

void ApplicationSolar::uploadMode() {
  glUseProgram(m_shaders.at("planet").handle);
  glUniform1i(m_shaders.at("planet").u_locs.at("CellShadingMode"), CellShadingMode);
}

// update uniform locations
void ApplicationSolar::uploadUniforms() { 
  // bind shader to which to upload uniforms
  glUseProgram(m_shaders.at("planet").handle);
  // upload uniform values to new locations
  uploadView();
  uploadProjection();
  uploadMode();
}

///////////////////////////// intialisation functions /////////////////////////
// load shader sources
void ApplicationSolar::initializeShaderPrograms() {
  // store shader program objects in container
  m_shaders.emplace("planet", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/simple.vert"},
                                           {GL_FRAGMENT_SHADER, m_resource_path + "shaders/simple.frag"}}});
  // request uniform locations for shader program
  m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
  m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
  m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
  m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
  m_shaders.at("planet").u_locs["PlanetColor"] = -1;
  m_shaders.at("planet").u_locs["LightIntensity"] = -1;
  m_shaders.at("planet").u_locs["LightColor"] = -1;
  m_shaders.at("planet").u_locs["CellShadingMode"] = -1;
  m_shaders.at("planet").u_locs["PlanetTexture"] = -1;


  m_shaders.emplace("stars", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/stars.vert"},
                                           {GL_FRAGMENT_SHADER, m_resource_path + "shaders/stars.frag"}}});
  m_shaders.at("stars").u_locs["ViewMatrix"] = -1;
  m_shaders.at("stars").u_locs["ProjectionMatrix"] = -1;  

  m_shaders.emplace("orbits", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/orbits.vert"},
                                           {GL_FRAGMENT_SHADER, m_resource_path + "shaders/orbits.frag"}}});
  m_shaders.at("orbits").u_locs["OrbitMatrix"] = -1;
  m_shaders.at("orbits").u_locs["ViewMatrix"] = -1;
  m_shaders.at("orbits").u_locs["ProjectionMatrix"] = -1;  
}

// load models
void ApplicationSolar::initializeGeometry() {
  model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL | model::TEXCOORD);

  // generate vertex array object
  glGenVertexArrays(1, &planet_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(planet_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &planet_object.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planet_model.data.size(), planet_model.data.data(), GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::POSITION]);
  // activate second attribute on gpu
  glEnableVertexAttribArray(1);
  // second attribute is 3 floats with no offset & stride
  glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::NORMAL]);
  // activate third attribute on gpu
  glEnableVertexAttribArray(2);
  // second attribute is 2 floats with no offset & stride
  glVertexAttribPointer(2, model::TEXCOORD.components, model::TEXCOORD.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::TEXCOORD]);


   // generate generic buffer
  glGenBuffers(1, &planet_object.element_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet_object.element_BO);
  // configure currently bound array buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * planet_model.indices.size(), planet_model.indices.data(), GL_STATIC_DRAW);

  // store type of primitive to draw
  planet_object.draw_mode = GL_TRIANGLES;
  // transfer number of indices to model object 
  planet_object.num_elements = GLsizei(planet_model.indices.size());
}

//--- init_functions for creating planets
void ApplicationSolar::init_planets() {
  model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);
    // init Scene Graph
  scene_graph->setName("solar_system");
  Node* root_ptr = scene_graph->getRoot();
/*
  // init cam:
  CameraNode* cam_1 = new CameraNode();
  cam_1->setDistanceToOrigin(glm::fvec3{0.0f, 0.0f, 50.0f});
  root_ptr->addChildren(cam_1);
*/
  // init sun:
  // Node* sun_1_ptr = new Node("sun_holder", root_ptr, root_ptr->getPath() + "/sun", 1, nullptr);
  // PointLightNode* sun_1_light = new PointLightNode(1.5f, {0.3f, 0.5f, 0.1f}, "sun1_light", root_ptr, "//root/sun_1_light", 1, nullptr);
  GeometryNode* sun_1_geo_ptr = new GeometryNode("sun", sun_1_light, "//root/sun_1", 2, nullptr);
  sun_1_geo_ptr->setGeometry(planet_model);
  sun_1_geo_ptr->setDistanceToOrigin(glm::fvec3{0.0f, 0.0f, 0.0f});
  sun_1_geo_ptr->setSpeed(0.0f);
  sun_1_geo_ptr->setRadius(5.0f);
  sun_1_geo_ptr->setColor(glm::fvec3{1.0f, 1.0f, 0.7f});
  sun_1_geo_ptr->setTexturePath(m_resource_path + "textures/sunmap.png");
  // add sun to root
  root_ptr->addChildren(sun_1_light);
  sun_1_light->addChildren(sun_1_geo_ptr);
  // init PointLight
  // PointLightNode* sun_1_light = new PointLightNode(1.5f, {0.3f, 0.5f, 0.1f}, "sun1_light", nullptr, "//root/sun_1_light", 1, nullptr);
  // root_ptr->addChildren(sun_1_light);


  // init 1.mercury
  Node* mecury_ptr = new Node("mecury_holder", root_ptr, root_ptr->getPath() + "/mecury", 1, sun_1_geo_ptr);
  GeometryNode* mecury_geo_ptr = new GeometryNode("mecury", mecury_ptr, "//root/mecury", 2, sun_1_geo_ptr);
  mecury_geo_ptr->setGeometry(planet_model);
  mecury_geo_ptr->setDistanceToOrigin(glm::fvec3{8.0f, 0.0f, 0.0f});
  mecury_geo_ptr->setSpeed(0.2f);
  mecury_geo_ptr->setRadius(0.385f);
  mecury_geo_ptr->setColor(glm::fvec3{0.42f, 0.26f, 0.1f});
  mecury_geo_ptr->setTexturePath(m_resource_path + "textures/mercurymap.png");
  // add mecury to root
  root_ptr->addChildren(mecury_ptr);
  mecury_ptr->addChildren(mecury_geo_ptr);

  // init 2.venus
  Node* venus_ptr = new Node("venus_holder", root_ptr, root_ptr->getPath() + "/venus", 1, sun_1_geo_ptr);
  GeometryNode* venus_geo_ptr = new GeometryNode("venus", venus_ptr, "//root/venus", 2, sun_1_geo_ptr);
  venus_geo_ptr->setGeometry(planet_model);
  venus_geo_ptr->setDistanceToOrigin(glm::fvec3{14.0f, 0.0f, 0.0f});
  venus_geo_ptr->setSpeed(0.15f);
  venus_geo_ptr->setRadius(0.95f);
  venus_geo_ptr->setColor(glm::fvec3{0.6f, 0.4f, 0.2f});
  venus_geo_ptr->setTexturePath(m_resource_path + "textures/venusmap.png");
  // add venus to root
  root_ptr->addChildren(venus_ptr);
  venus_ptr->addChildren(venus_geo_ptr);

  // init 3.earth TEST:
  Node* earth_ptr = new Node("earth_holder", root_ptr, root_ptr->getPath() + "/earth", 1, sun_1_geo_ptr);
  GeometryNode* earth_geo_ptr = new GeometryNode("earth", earth_ptr, "//root/earth", 2, sun_1_geo_ptr);
  earth_geo_ptr->setGeometry(planet_model);
  earth_geo_ptr->setDistanceToOrigin(glm::fvec3{20.0f, 0.0f, 0.0f});
  earth_geo_ptr->setSpeed(0.1f);
  earth_geo_ptr->setRadius(1.0f);
  earth_geo_ptr->setColor(glm::fvec3{0.2f, 0.6f, 1.0f});
  earth_geo_ptr->setTexturePath(m_resource_path + "textures/earthmap1k.png");
  // add earth to root
  root_ptr->addChildren(earth_ptr);
  earth_ptr->addChildren(earth_geo_ptr);

  // init moon TEST:
  Node* moon_ptr = new Node("moon_holder", earth_ptr, "/moon", 3, earth_geo_ptr);
  GeometryNode* moon_geo_ptr = new GeometryNode("moon", earth_ptr, "//root/earth/moon", 4, earth_geo_ptr);
  moon_geo_ptr->setGeometry(planet_model);
  moon_geo_ptr->setDistanceToOrigin(glm::fvec3{1.5f, 0.0f, 0.0f});
  moon_geo_ptr->setSpeed(0.5f);
  moon_geo_ptr->setRadius(0.272f);
  moon_geo_ptr->setColor(glm::fvec3{0.5f, 0.5f, 0.5f});
  moon_geo_ptr->setTexturePath(m_resource_path + "textures/moonmap1k.png");
  // add moon to earth
  earth_ptr->addChildren(moon_ptr);
  moon_ptr->addChildren(moon_geo_ptr);

  // init 4.mars
  Node* mars_ptr = new Node("mars_holder", root_ptr, root_ptr->getPath() + "/mars", 1, sun_1_geo_ptr);
  GeometryNode* mars_geo_ptr = new GeometryNode("mars", mars_ptr, "//root/mars", 2, sun_1_geo_ptr);
  mars_geo_ptr->setGeometry(planet_model);
  mars_geo_ptr->setDistanceToOrigin(glm::fvec3{30.0f, 0.0f, 0.0f});
  mars_geo_ptr->setSpeed(0.2f);
  mars_geo_ptr->setRadius(0.53f);
  mars_geo_ptr->setColor(glm::fvec3{0.55f, 0.35f, 0.3f});
  mars_geo_ptr->setTexturePath(m_resource_path + "textures/mars_1k_color.png");
  // add mars to root
  root_ptr->addChildren(mars_ptr);
  mars_ptr->addChildren(mars_geo_ptr);
  // init moon of mars: phobos
  Node* phobos_ptr = new Node("phobos_holder", mars_ptr, "/phobos", 3, mars_geo_ptr);
  GeometryNode* phobos_geo_ptr = new GeometryNode("phobos", mars_ptr, "//root/mars/phobos", 4, mars_geo_ptr);
  phobos_geo_ptr->setGeometry(planet_model);
  phobos_geo_ptr->setDistanceToOrigin(glm::fvec3{1.5f, 0.0f, 0.0f});
  phobos_geo_ptr->setSpeed(1.2f);
  phobos_geo_ptr->setRadius(0.27f);
  phobos_geo_ptr->setColor(glm::fvec3{0.4f, 0.4f, 0.4f});
  phobos_geo_ptr->setTexturePath(m_resource_path + "textures/phobosbump.png");
  // add phobos to mars
  mars_ptr->addChildren(phobos_ptr);
  phobos_ptr->addChildren(phobos_geo_ptr);
  // init moon of mars: deimos
  Node* deimos_ptr = new Node("deimos_holder", mars_ptr, "/deimos", 3, mars_geo_ptr);
  GeometryNode* deimos_geo_ptr = new GeometryNode("deimos", mars_ptr, "//root/mars/deimos", 4, mars_geo_ptr);
  deimos_geo_ptr->setGeometry(planet_model);
  deimos_geo_ptr->setDistanceToOrigin(glm::fvec3{1.0f, 0.0f, 0.0f});
  deimos_geo_ptr->setSpeed(1.7f);
  deimos_geo_ptr->setRadius(0.19f);
  deimos_geo_ptr->setColor(glm::fvec3{0.3f, 0.3f, 0.3f});
  deimos_geo_ptr->setTexturePath(m_resource_path + "textures/deimosbump.png");
  // add deimos to mars
  mars_ptr->addChildren(deimos_ptr);
  deimos_ptr->addChildren(deimos_geo_ptr);

  // init 5.jupiter
  Node* jupiter_ptr = new Node("jupiter_holder", root_ptr, root_ptr->getPath() + "/jupiter", 1, sun_1_geo_ptr);
  GeometryNode* jupiter_geo_ptr = new GeometryNode("jupiter", jupiter_ptr, "//root/jupiter", 2, sun_1_geo_ptr);
  jupiter_geo_ptr->setGeometry(planet_model);
  jupiter_geo_ptr->setDistanceToOrigin(glm::fvec3{40.0f, 0.0f, 0.0f});
  jupiter_geo_ptr->setSpeed(0.15f);
  jupiter_geo_ptr->setRadius(1.98f);
  jupiter_geo_ptr->setColor(glm::fvec3{1.0f, 0.75f, 0.6f});
  jupiter_geo_ptr->setTexturePath(m_resource_path + "textures/jupitermap.png");
  // add jupiter to root
  root_ptr->addChildren(jupiter_ptr);
  jupiter_ptr->addChildren(jupiter_geo_ptr);

  // init 6.saturn
  Node* saturn_ptr = new Node("saturn_holder", root_ptr, root_ptr->getPath() + "/saturn", 1, sun_1_geo_ptr);
  GeometryNode* saturn_geo_ptr = new GeometryNode("saturn", saturn_ptr, "//root/saturn", 2, sun_1_geo_ptr);
  saturn_geo_ptr->setGeometry(planet_model);
  saturn_geo_ptr->setDistanceToOrigin(glm::fvec3{50.0f, 0.0f, 0.0f});
  saturn_geo_ptr->setSpeed(0.13f);
  saturn_geo_ptr->setRadius(1.315f);
  saturn_geo_ptr->setColor(glm::fvec3{0.9f, 0.75f, 0.6f});
  saturn_geo_ptr->setTexturePath(m_resource_path + "textures/saturnmap.png");
  // add satur to root
  root_ptr->addChildren(saturn_ptr);
  saturn_ptr->addChildren(saturn_geo_ptr);

  // init 7.uranus
  Node* uranus_ptr = new Node("uranus_holder", root_ptr, root_ptr->getPath() + "/uranus", 1, sun_1_geo_ptr);
  GeometryNode* uranus_geo_ptr = new GeometryNode("uranus", uranus_ptr, "//root/uranus", 2, sun_1_geo_ptr);
  uranus_geo_ptr->setGeometry(planet_model);
  uranus_geo_ptr->setDistanceToOrigin(glm::fvec3{60.0f, 0.0f, 0.0f});
  uranus_geo_ptr->setSpeed(0.22f);
  uranus_geo_ptr->setRadius(0.94f);
  uranus_geo_ptr->setColor(glm::fvec3{0.69f, 0.83f, 0.93f});
  uranus_geo_ptr->setTexturePath(m_resource_path + "textures/uranusmap.png");
  // add uranus to root
  root_ptr->addChildren(uranus_ptr);
  uranus_ptr->addChildren(uranus_geo_ptr);

  // init 8.neptune
  Node* neptune_ptr = new Node("neptune_holder", root_ptr, root_ptr->getPath() + "/neptune", 1, sun_1_geo_ptr);
  GeometryNode* neptune_geo_ptr = new GeometryNode("neptune", neptune_ptr, "//root/neptune", 2, sun_1_geo_ptr);
  neptune_geo_ptr->setGeometry(planet_model);
  neptune_geo_ptr->setDistanceToOrigin(glm::fvec3{85.0f, 0.0f, 0.0f});
  neptune_geo_ptr->setSpeed(0.3f);
  neptune_geo_ptr->setRadius(1.4f);
  neptune_geo_ptr->setColor(glm::fvec3{0.4f, 0.4f, 0.93f});
  neptune_geo_ptr->setTexturePath(m_resource_path + "textures/neptunemap.png");
  // add neptune to root
  root_ptr->addChildren(neptune_ptr);
  neptune_ptr->addChildren(neptune_geo_ptr);

  scene_graph->printGraph();
}

// creating stars
void ApplicationSolar::init_stars() {
  // for loop to create 2500 stars
  for(int i = 0; i < 2500; ++i) {
    // random XYZ-value for position of the star
    GLfloat x = (rand() % 150) - 75.0f;     // random in INterval [-75; 75]
    GLfloat y = (rand() % 150) - 75.0f;
    GLfloat z = (rand() % 150) - 75.0f;
    star_container.emplace_back(y);
    star_container.emplace_back(x);
    star_container.emplace_back(z); 
    // random RGB-value for color of the star
    GLfloat r = ((rand() % 51) + 153)/ 255.0f;      // [51-204]
    GLfloat g = ((rand() % 80) + 110) / 255.0f;     // [80-190]
    GLfloat b = ((rand() % 64) + 140) / 255.0f;     // [64-204]
    star_container.emplace_back(r);
    star_container.emplace_back(g);
    star_container.emplace_back(b);
  }
  // generation of the vertex array object
  glGenVertexArrays(1, &star_object.vertex_AO);   // (number of VAO, array which VAOs are stored)
  // bind the array for attaching buffers
  glBindVertexArray(star_object.vertex_AO);       // (name of VA to bind)
  // generate generic buffer
  glGenBuffers(1, &star_object.vertex_BO);        // (number of VBO, array)
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, star_object.vertex_BO);     // (target(purpose: Vertex attributes), buffer)
  // creates and initializes a buffer object's data store
  glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(sizeof(float) * star_container.size()), star_container.data(), GL_STATIC_DRAW);
  //Specify the attributes
  // atrribute 0 on GPU
  glEnableVertexAttribArray(0);
  // (index of attribute, number of components, type, normalized, stride = components*num_attributes, pointer to first component of the first attribute)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
  // attribute 1 on GPU
  glEnableVertexAttribArray(1);
  // (....,start of the 2nd attribute is at index 3, type of this is void pointer)
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));

  // define draw mode to use when render
  star_object.draw_mode = GL_POINTS;
  // define number of elements: = size of the array devided by the number of componentsof attributes
  star_object.num_elements = GLsizei(star_container.size() / 6); 
}

void ApplicationSolar::init_orbits() {
  // drawing circle with vertex
  for(int i = 0; i < 361; ++i) {
    //current angle (first angle is 0)
    float angle = float(i) * float(PI) / 180.0f;
    GLfloat x = cos(angle);
    GLfloat y = 0.0f;
    GLfloat z = sin(angle);
    // add index to container
    if(x == 0 && z == 0) {
      continue;
    }
    else {
      orbit_container.emplace_back(x);
      orbit_container.emplace_back(y);
      orbit_container.emplace_back(z);
    }
  }
  // generate vertex array object
  glGenVertexArrays(1, &orbit_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(orbit_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &orbit_object.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, orbit_object.vertex_BO);
  // creates and initializes a buffer object's data store
  glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(sizeof(float) * orbit_container.size()), orbit_container.data(), GL_STATIC_DRAW);

  // activate attribute 0 on GPU
  glEnableVertexAttribArray(0);
  // pass 3 floats for each orbit
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

  // primitive type to draw
  orbit_object.draw_mode = GL_LINE_LOOP;
  // number of indexs to model object
  orbit_object.num_elements = GLsizei(orbit_container.size() / 3);
}
// set Texture for each Planet Object
void ApplicationSolar::init_textures() {
  // planet textures
  auto planets_list = scene_graph->getRoot()->getChildrenList();
  planet_texture_loader(planets_list);
}
void ApplicationSolar::planet_texture_loader(std::list<Node*> const& planets_list) {
  for (auto const& planet_ptr : planets_list) {
    auto children_list = planet_ptr->getChildrenList();

    if (children_list.size() > 0) {
      planet_texture_loader(children_list);
    }

    if (planet_ptr->getDepth() % 2 == 0) {
      auto texture = planet_ptr->getTexture();
      auto texture_object = planet_ptr->getTextureObject();

      glActiveTexture(GL_TEXTURE0);             // the first and the only Texture to be actived
      glGenTextures(1, &texture_object.handle);
      glBindTexture(GL_TEXTURE_2D, texture_object.handle);        // target: texture is bound to 2D
      
      // Define Texture Sampling Parameters
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);   // LINEAR: weighted average of the four neighbouring texels
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glTexImage2D(GL_TEXTURE_2D, 0, texture.channels, texture.width, texture.height,
                                  0, texture.channels, texture.channel_type, texture.ptr());

      planet_ptr->setTextureObject(texture_object);
    }
  }
}

///////////////////////////// callback functions for window events ////////////
// handle key input
void ApplicationSolar::keyCallback(int key, int action, int mods) {
/*  float PI = glm::pi<float>();
  glm::fmat4 local_matrix_origin;
  for(auto const& planet_ptr : scene_graph.getRoot()->getChildrenList()) {
    if(planet_ptr->getName().find("sun") != std::string::npos) {
      local_matrix_origin = planet_ptr->getLocalTransform();
      break;
    }
  }
*/

  if (key == GLFW_KEY_Z  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.5f});
    uploadView();
  }
  else if (key == GLFW_KEY_X  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.5f});
    uploadView();
  }
  else if (key == GLFW_KEY_A  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{-0.5f, 0.0f, 0.0f});
    uploadView();
  }
  else if (key == GLFW_KEY_D  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.5f, 0.0f, 0.0f});
    uploadView();
  }
  else if (key == GLFW_KEY_S  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, -0.5f, 0.0f});
    uploadView();
  }     
  else if (key == GLFW_KEY_W  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.5f, 0.0f});
    uploadView();
  }   
  else if (key == GLFW_KEY_1 && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    if(CellShadingMode == true) {
      CellShadingMode = false;
      uploadMode();
    }
  }
  else if (key == GLFW_KEY_2 && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    if(CellShadingMode == false) {
      CellShadingMode = true;
      uploadMode();
    }
  }

}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {

  float PI = glm::pi<float>();
/*
  glm::fmat4 local_matrix_origin;
  for(auto const& planet_ptr : scene_graph.getRoot()->getChildrenList()) {
    if(planet_ptr->getName().find("sun") != std::string::npos) {
      local_matrix_origin = planet_ptr->getLocalTransform();
      break;
    }
  }
*/
  // mouse handling
//  m_view_transform = glm::rotate(m_view_transform, 0.05f * PI, glm::fvec3{float(pos_y), float(pos_x), 0.0f});
/*
  if(pos_x > 0) {
    m_view_transform = glm::rotate(m_view_transform, 0.05f * PI, glm::fvec3{0.0f, 1.0f, 0.0f});
    m_view_transform = glm::rotate(m_view_transform, glm::radians(float(pos_x/10.0f)), glm::fvec3{0.0f, 1.0f, 0.0f});
  }
  else if(pos_x < 0) {
    m_view_transform = glm::rotate(m_view_transform, -0.05f * PI, glm::fvec3{0.0f, -1.0f, 0.0f});
    m_view_transform = glm::rotate(m_view_transform, glm::radians(float(pos_x/10.0f)), glm::fvec3{0.0f, -1.0f, 0.0f});

  }
*/
/*  if(pos_y > 0) {
    m_view_transform = glm::rotate(m_view_transform, 0.05f * PI, glm::fvec3{1.0f, 0.0f, 0.0f});
  }
  else if(pos_y < 0) {
    m_view_transform = glm::rotate(m_view_transform, -0.05f * PI, glm::fvec3{1.0f, 0.0f, 0.0f});
  }
*/
}

//handle resizing
void ApplicationSolar::resizeCallback(unsigned width, unsigned height) {
  // recalculate projection matrix for new aspect ration
  m_view_projection = utils::calculate_projection_matrix(float(width) / float(height));
  // upload new projection matrix
  uploadProjection();
}


// exe entry point
int main(int argc, char* argv[]) {
  Application::run<ApplicationSolar>(argc, argv, 3, 2);
}