#include "scene_graph.hpp"

void SceneGraph::setName(std::string const& name) {
    name_ = name;
}
void SceneGraph::setRoot(Node const& root) {
    root_ = root;
}

std::string SceneGraph::getName() {
    return name_;
}
Node SceneGraph::getRoot() {
    return root_;
}

std::string SceneGraph::printGraph() {
    // change later
    return name_;
}
