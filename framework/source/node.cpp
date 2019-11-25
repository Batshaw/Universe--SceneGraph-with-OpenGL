#include "node.hpp"

// typedef std::shared_ptr<Node> node_ptr;

Node::Node(std::string const& name) : 
                parent_(nullptr),
                children_(),
                name_("root"),
                path_("//" + name_),
                depth_(0),
                localTransform_(glm::fmat4{}),      
                worldTransform_(glm::fmat4{}),
                origin_(nullptr)    {}

Node::Node() :  parent_(nullptr),
                children_(),
                name_("root"),
                path_("//" + name_),
                depth_(0),
                localTransform_(glm::fmat4{}),      
                worldTransform_(glm::fmat4{}),
                origin_(nullptr)       {}

Node::Node(std::string const& name, Node* const& parent,
            std::string const& path, int depth, Node* const& origin) :
        name_(name),
        parent_(parent),
        path_(path),
        depth_(depth),
        origin_(origin)     {}

Node::~Node() {}

// Setter
void Node::setParent(Node* const& parent) {
    parent_ = parent;
}
void Node::setLocalTransform(glm::fmat4 const& localTransform) {
    localTransform_ = localTransform;
}
void Node::setWorldTransform(glm::fmat4 const& worldTransform) {
    worldTransform_ = worldTransform;
}
void Node::setDistanceToOrigin(glm::fvec3 const& distance_to_origin) {
    distance_to_origin_ = distance_to_origin;
}
void Node::setSpeed(float const& speed) {
    speed_ = speed;
}
void Node::setRadius(float const& radius) {
    radius_ = radius;
}

// Getter
Node* Node::getParent() const {
    return parent_;
}
Node* Node::getChildren(std::string const& child_name) const {
    for(auto const& child : children_) {
        if(child->name_ == child_name)
            return child;
    }
    return nullptr;
}
std::list<Node*> Node::getChildrenList() const {
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
glm::fmat4 Node::getLocalTransform() const {
    return localTransform_;
}
glm::fmat4 Node::getWorldTransform() const {
    return worldTransform_;
}
glm::fvec3 Node::getDistanceToOrigin() const {
    return distance_to_origin_;
}
Node* Node::getOrigin() const {
    return origin_;
}
float Node::getSpeed() const {
    return speed_;
}
float Node::getRadius() const {
    return radius_;
}

//lakjsdhaslkhdasd
void Node::addChildren(Node* const& children) {
    children_.emplace_back(children);
}
Node* Node::removeChildren(std::string const& children_name) {
    Node* removed_child = getChildren(children_name);
    children_.remove(removed_child);
    return removed_child;
}
