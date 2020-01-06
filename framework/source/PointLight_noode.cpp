#include "PointLight_node.hpp"

PointLightNode::PointLightNode() : 
    Node{},
    lightIntensity_{},
    lightColor_{}       {}

PointLightNode::PointLightNode(float lightIntensity, glm::fvec3 const& lightColor, std::string const& name, Node* const& parent, std::string const& path, int depth, Node* const& origin) :
    lightIntensity_{lightIntensity},
    lightColor_{lightColor},
    Node{name, parent, path, depth, origin}     {}

void PointLightNode::setLightIntensity(float lightIntensity) {
    lightIntensity_ = lightIntensity;
}

float PointLightNode::gettLightIntensity() const {
    return lightIntensity_;
}

void PointLightNode::setLightColor(glm::fvec3 const& lightColor) {
    lightColor_ = lightColor;
}

glm::fvec3 PointLightNode::getLightColor() const {
    return lightColor_;
}