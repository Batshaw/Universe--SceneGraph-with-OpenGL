#include <geometry_node.hpp>

GeometryNode::GeometryNode() : Node{}, geometry_{}  {}

GeometryNode::GeometryNode(std::string const& name, Node* const& parent, std::string const& path, int depth, Node* const& origin) :
    Node{name, parent, path, depth, origin},
    geometry_{} {}

void GeometryNode::setGeometry(model const& geometry) {
    geometry_ = geometry;
}
model GeometryNode::getGeometry() const {
    return geometry_;
}