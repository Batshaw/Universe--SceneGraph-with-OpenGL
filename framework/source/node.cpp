#include "node.hpp"

typedef std::shared_ptr<Node> node_ptr;

Node::Node() :  parent_(nullptr),
                children_(),
                name_("default_node"),
                path_("\\" + name_),
                depth_(0),
                localTransform_(1.0f),      // identity matrix for non_transformation
                worldTransform_(1.0f)       {}

Node::Node(std::string const& name) :   parent_(nullptr),
                                        name_(name),
                                        path_("..." + name),
                                        depth_(0)   {}

Node::Node(std::string const& name, node_ptr const& parent,
            std::string const& path, int depth) :   parent_(parent),
                                                    name_(name),
                                                    path_(path),
                                                    depth_(depth)     {}

Node::~Node() {}

void Node::setParent(node_ptr const& parent) {
    parent_ = parent;
}
void Node::setLocalTransform(glm::mat4 const& localTransform) {
    localTransform_ = localTransform;
}
void Node::setWorldTransform(glm::mat4 const& worldTransform) {
    worldTransform_ = worldTransform;
}

node_ptr Node::getParent() const {
    return parent_;
}
node_ptr Node::getChildren(std::string const& child_name) const {
    for(auto const& child : children_) {
        if(child->name_ == child_name)
            return child;
    }
    return nullptr;
}
std::list<node_ptr> Node::getChildrenList() const {
    return children_;
}
std::string Node::getName() const {
    return name_;
}
std::string Node::getPath() const {
    return path_;
}
int Node::getDepth() const {
    return depth_;
}
glm::mat4 Node::getLocalTransform() const {
    return localTransform_;
}
glm::mat4 Node::getWorldTransform() const {
    return worldTransform_;
}

void Node::addChildren(node_ptr const& children) {
    children_.push_back(children);
}
node_ptr Node::removeChildren(std::string const& children_name) {
    node_ptr removed_child = getChildren(children_name);
    children_.remove(removed_child);
    return removed_child;
}
