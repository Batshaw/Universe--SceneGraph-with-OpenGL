#include "camera_node.hpp"

CameraNode::CameraNode() :
    name_("default_camera"),
    isPerspective_(true),
    isEnabled_(true),
    projectionMatrix_(1.0f)    {}

CameraNode::CameraNode(std::string const& name, bool const& isPerspective, bool const& isEnabled,
                        glm::mat4 const& projectionMatrix) :
    name_(name),
    isPerspective_(isPerspective),
    projectionMatrix_(projectionMatrix)  {}

CameraNode::~CameraNode() {}
