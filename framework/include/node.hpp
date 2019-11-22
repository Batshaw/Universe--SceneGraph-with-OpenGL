#ifndef NODE_HPP
#define NODE_HPP

#include <string>
#include <memory>       // for shared_ptr
#include <list>
#include <glm/glm.hpp>

typedef std::shared_ptr<Node> node_ptr;

class Node {
    
    public:
        // Constructors
        Node();
        Node(std::string const& name, node_ptr const& parent, std::string const& path, int depth);

        ~Node();

        // Setter
        void setParent(node_ptr const& parent);
        void setLocalTransform(glm::mat4 const& localTransform);
        void setWorldTransform(glm::mat4 const& worldTransform);


        // Getter
        node_ptr getParent() const;
        node_ptr getChildren(std::string const& child_name) const;
        std::list<node_ptr> getChildrenList() const;
        std::string getName() const;
        std::string getPath() const;
        int getDepth() const;
        glm::mat4 getLocalTransform() const;
        glm::mat4 getWorldTransform() const;

        void addChildren(node_ptr const& children);
        node_ptr removeChildren(std::string const& children_name);

    private:
        node_ptr parent_;
        std::list<node_ptr> children_;
        std::string name_;
        std::string path_;
        int depth_;
        glm::mat4 localTransform_;
        glm::mat4 worldTransform_;
        
};

#endif      // NODE_HPP