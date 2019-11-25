#include "application_solar.hpp"
#include "window_handler.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding 
using namespace gl;
// typedef std::shared_ptr<Node> node_ptr;

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
 ,scene_graph{}
 ,m_view_transform{glm::translate(glm::fmat4{}, glm::fvec3{0.0f, 0.0f, 4.0f})}
 ,m_view_projection{utils::calculate_projection_matrix(initial_aspect_ratio)}
{
  initializeGeometry();
  init_planets();
  initializeShaderPrograms();
}

ApplicationSolar::~ApplicationSolar() {
  glDeleteBuffers(1, &planet_object.vertex_BO);
  glDeleteBuffers(1, &planet_object.element_BO);
  glDeleteVertexArrays(1, &planet_object.vertex_AO);
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

  std::list<Node*> const our_solar_system = scene_graph.getRoot()->getChildrenList();
  render_planets(our_solar_system);

}

void ApplicationSolar::render_planets(std::list<Node*> const& scene_children_list) const {
  // go through children list
  for(auto const& planet_ptr : scene_children_list) {
    auto children_of_planet = planet_ptr->getChildrenList();
    if(!children_of_planet.empty()) {
      render_planets(children_of_planet);   // recursive
    }
    // ignore the holder
    if(planet_ptr->getDepth() % 2 == 0) {
      //transform of the planet
      glm::fmat4 transform_matrix = compute_transform_matrix(planet_ptr);
      // extra matrix for normal transformation to keep them orthogonal to surface
      glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * transform_matrix);
      
      // specify the sun ignore for now
      glUseProgram(m_shaders.at("planet").handle);
      glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                     1, GL_FALSE, glm::value_ptr(transform_matrix));
      glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                     1, GL_FALSE, glm::value_ptr(normal_matrix));
      // bind the VAO to draw
      glBindVertexArray(planet_object.vertex_AO);

      // draw bound vertex array using bound shader
      glDrawElements(planet_object.draw_mode, planet_object.num_elements, model::INDEX.type, NULL);      
   }
  }
}
glm::fmat4 ApplicationSolar::compute_transform_matrix(Node* const& planet_ptr) const {
  glm::fmat4 transform_matrix = planet_ptr->getWorldTransform();

/*  if(planet_ptr->getDepth() == 4) {

  }
*/
  // rotation around parent_planet
  transform_matrix = glm::rotate(transform_matrix, float(glfwGetTime()), glm::fvec3{0.0f, 1.0f, 0.0f});
  transform_matrix = glm::translate(transform_matrix, -1.0f * planet_ptr->getDistanceToOrigin());
  // rotation around itself
  //transform_matrix = glm::rotate(transform_matrix, float(glfwGetTime()), glm::fvec3{0.0f, 1.0f, 0.0f});

  return transform_matrix;
}


void ApplicationSolar::uploadView() {
  // vertices are transformed in camera space, so camera transform must be inverted
  glm::fmat4 view_matrix = glm::inverse(m_view_transform);
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));
}

void ApplicationSolar::uploadProjection() {
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));
}

// update uniform locations
void ApplicationSolar::uploadUniforms() { 
  // bind shader to which to upload unforms
  glUseProgram(m_shaders.at("planet").handle);
  // upload uniform values to new locations
  uploadView();
  uploadProjection();
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
}

// load models
void ApplicationSolar::initializeGeometry() {
  model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

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

//--- init_functions for creating things
void ApplicationSolar::init_planets() {
  model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

  // init root: default Node creats root Node
  Node* root_ptr = new Node();

  // init sun:
  Node* sun_1_ptr = new Node("sun_holder", root_ptr, root_ptr->getPath() + "/sun", 1, nullptr);
  GeometryNode* sun_1_geo_ptr = new GeometryNode("sun", root_ptr, "//root/sun_1", 2, nullptr);
  sun_1_geo_ptr->setGeometry(planet_model);
  sun_1_geo_ptr->setDistanceToOrigin(glm::fvec3{0.0f, 0.0f, 0.0f});
  // add sun to root
  root_ptr->addChildren(sun_1_ptr);
  sun_1_ptr->addChildren(sun_1_geo_ptr);

  //init earth TEST:
  Node* earth_ptr = new Node("earth_holder", root_ptr, root_ptr->getPath() + "/earth", 1, sun_1_geo_ptr);
  GeometryNode* earth_geo_ptr = new GeometryNode("earth", earth_ptr, "//root/earth", 2, sun_1_geo_ptr);
  earth_geo_ptr->setGeometry(planet_model);
  earth_geo_ptr->setDistanceToOrigin(glm::fvec3{20.0f, 0.0f, 0.0f});
  root_ptr->addChildren(earth_ptr);
  earth_ptr->addChildren(earth_geo_ptr);
  // init Scene Graph
  scene_graph.setName("solar_system");
  scene_graph.setRoot(root_ptr);
}

void ApplicationSolar::set_m_ViewTransform(glm::fmat4 const& camera_matrix) {
  m_view_transform = camera_matrix;
}

///////////////////////////// callback functions for window events ////////////
// handle key input
void ApplicationSolar::keyCallback(int key, int action, int mods) {
  if (key == GLFW_KEY_W  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.1f});
    uploadView();
  }
  else if (key == GLFW_KEY_S  && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.1f});
    uploadView();
  }
}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
  // mouse handling
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