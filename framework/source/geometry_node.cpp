#include <geometry_node.hpp>

GeometryNode::GeometryNode() : Node{}, geometry_{}  {}

GeometryNode::GeometryNode(std::string const& name, Node* const& parent, std::string const& path, int depth, Node* const& origin) :
    Node{name, parent, path, depth, origin},
    geometry_{},
    texture_path_{" "} {}

void GeometryNode::setGeometry(model const& geometry) {
    geometry_ = geometry;
}
void GeometryNode::setTexturePath(std::string const& texture_path) {
    texture_path_ = texture_path;
}
void GeometryNode::setTextureObject(texture_object const& texture_object) {
    texture_object_ = texture_object;
}

model GeometryNode::getGeometry() const {
    return geometry_;
}
std::string GeometryNode::getTexturePath() const {
    return texture_path_;
}
texture_object GeometryNode::getTextureObject() const {
    return texture_object_;
}