#include <geometry_node.hpp>

GeometryNode::GeometryNode(std::string const& name, model const& geometry) :
    Node(),
    geometry_(geometry) {}

void GeometryNode::setGeometry(model const& geometry) {
    geometry_ = geometry;
}
model GeometryNode::getGeometry() const {
    return geometry_;
}