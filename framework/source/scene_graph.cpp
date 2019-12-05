#include "scene_graph.hpp"

// set member (in-class) instance to null_pointer
SceneGraph* SceneGraph::scenegraph_instance = nullptr;

SceneGraph::SceneGraph() :
    name_{"Scene Graph"},
    root_{new Node()}                 {}

SceneGraph::~SceneGraph() {
   root_ = nullptr;
}

// Setter
void SceneGraph::setName(std::string const& name) {
    name_ = name;
}
void SceneGraph::setRoot(Node* const& root) {
    root_ = root;
}

// Getter
std::string SceneGraph::getName() const{
    return name_;
}
Node* SceneGraph::getRoot() const{
    return root_;
}

void SceneGraph::printGraph() {
	Node* root = getRoot();
	int depth = root->getDepth();

	printNode(root, depth);
}

void SceneGraph::printNode(Node* node, int depth) {
	for (int i = 0; i < depth; ++i) {
		std::cout << " ";
	}

	std::cout << node->getName() << std::endl;
	for (Node* const& child : node->getChildrenList()) {
		printNode(child, depth + 1);
	}
}