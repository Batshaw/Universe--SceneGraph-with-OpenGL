#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"
#include "scene_graph.hpp"
#include "PointLight_node.hpp"
#include "texture_loader.hpp"

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
  // init orbits
  void init_orbits();
  // init Texture for planets
  void init_textures();
  // init  FrameBuffer
  void init_FrameBuffer(unsigned int width = 850u, unsigned int height = 650u);
  // init ScreenQuad
  void init_ScreenQuad();
  // update uniform values
  void uploadUniforms();
  // upload projection matrix
  void uploadProjection();
  // upload view matrix
  void uploadView();
  // upload ApperanceMode
  void uploadMode();

  // void initializeSceneGraph();

  // help function to render method
  void render_planets(std::list<Node*> const& planets_list) const;
  void render_stars() const;
  void render_orbits(Node* const& child_planet) const;
  void render_ScreenQuad() const;

  // compute transform matrix (help function)
  glm::fmat4 compute_transform_matrix(Node* const& planet_ptr) const;

  // Texture loader
  void planet_texture_loader(std::list<Node*> const& planets_list);

  // create the first and only instance for SceneGraph (Singleton Pattern)
  SceneGraph* scene_graph = SceneGraph::getInstance();
  PointLightNode* sun_1_light = new PointLightNode(3.5f, {0.98f, 0.96f, 0.95f}, "sun1_light", nullptr, "//root/sun_1_light", 1, nullptr);
  //star vector
  std::vector<GLfloat> star_container;
  std::vector<GLfloat> orbit_container;

  // cpu representation of model
  model_object planet_object;
  model_object star_object;
  model_object orbit_object;
  model_object screen_quad_object;
  
  // camera transform matrix
  glm::fmat4 m_view_transform;
  // camera projection matrix
  glm::fmat4 m_view_projection;

  // Frame Buffer Object with a texture as Color Attachment and a Renderbuffer as Depth Attachment
  texture_object frameBuffer_color_texture_;
  texture_object renderBuffer_depth_texture_;
  texture_object frame_buffer_;

  // MODE
  bool CellShadingMode;
  int model_type;
};

#endif