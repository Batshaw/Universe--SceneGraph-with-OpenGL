#include "scene_graph.hpp"

//constructor for using without Singleton Pattern
SceneGraph::SceneGraph(std::string const& name, Node* const& root) :
    name_{name},
    root_{root}     {}

SceneGraph::SceneGraph() :
    name_{"Scene Graph"},
    root_{}                 {}

SceneGraph::~SceneGraph() {
  root_ = nullptr;
}

void SceneGraph::setName(std::string const& name) {
    name_ = name;
}
void SceneGraph::setRoot(Node* const& root) {
    root_ = root;
}

std::string SceneGraph::getName() const{
    return name_;
}
Node* SceneGraph::getRoot() const{
    return root_;
}

std::string SceneGraph::printGraph() {
    // change later
    return name_;
}
