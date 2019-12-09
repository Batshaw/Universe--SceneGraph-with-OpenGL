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
  // init all planets
  void init_planets();
  // init stars
  void init_stars();
  // update uniform values
  void uploadUniforms();
  // upload projection matrix
  void uploadProjection();
  // upload view matrix
  void uploadView();

  // void initializeSceneGraph();

  // help function to render method
  void render_planets(std::list<Node*> const& planets_list) const;
  void render_stars() const;

  // compute transform matrix (help function)
  glm::fmat4 compute_transform_matrix(Node* const& planet_ptr) const;

  // create the first and only instance for SceneGraph (Singleton Pattern)
  SceneGraph* scene_graph = SceneGraph::getInstance();
  //star vector
  std::vector<GLfloat> star_container;

  // cpu representation of model
  model_object planet_object;
  model_object star_object;
  
  // camera transform matrix
  glm::fmat4 m_view_transform;
  // camera projection matrix
  glm::fmat4 m_view_projection;
};

#endif