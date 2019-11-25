#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"
#include "scene_graph.hpp"

// typedef std::shared_ptr<Node> node_ptr;

// gpu representation of model
class ApplicationSolar : public Application {
 public:
  // allocate and initialize objects
  ApplicationSolar(std::string const& resource_path);
  // free allocated objects
  ~ApplicationSolar();

  // react to key input
  void keyCallback(int key, int action, int mods);
  //handle delta mouse movement input
  void mouseCallback(double pos_x, double pos_y);
  //handle resizing
  void resizeCallback(unsigned width, unsigned height);

  // draw all objects
  void render() const;

 protected:
  void initializeShaderPrograms();
  void initializeGeometry();
  void init_planets();
  // update uniform values
  void uploadUniforms();
  // upload projection matrix
  void uploadProjection();
  // upload view matrix
  void uploadView();

  void initializeSceneGraph();
  void set_m_ViewTransform(glm::fmat4 const& camera_matrix);

  // render methods
  void render_planets(std::list<Node*> const& planets_list) const;

  // compute transform matrix
  glm::fmat4 compute_transform_matrix(Node* const& planet_ptr) const;


  SceneGraph scene_graph;

  // cpu representation of model
  model_object planet_object;
  
  // camera transform matrix
  glm::fmat4 m_view_transform;
  // camera projection matrix
  glm::fmat4 m_view_projection;
};

#endif