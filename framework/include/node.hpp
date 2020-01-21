#ifndef NODE_HPP
#define NODE_HPP

#include <string>
#include <memory>       // for shared_ptr
#include <list>
#include <glm/glm.hpp>
#include <structs.hpp>
#include <pixel_data.hpp>

// typedef std::shared_ptr<Node> node_ptr;

class Node {
    
    public:
        // Constructors
        Node();     // for root_
        Node(std::string const& name);
        Node(std::string const& name, Node* const& parent, std::string const& path, int depth, Node* const& origin);

        ~Node();

        // Setter
        void setParent(Node* const& parent);
        void setLocalTransform(glm::fmat4 const& localTransform);
        void setWorldTransform(glm::fmat4 const& worldTransform);
        void setDistanceToOrigin(glm::fvec3 const& distance_to_origin);
        void setSpeed(float const& speed);
        void setRadius(float const& radius);
        void setColor(glm::fvec3 const& color);


        // Getter
        Node* getParent() const;
        Node* getChildren(std::string const& child_name) const;
        std::list<Node*> getChildrenList() const;
        std::string getName() const;
        std::string getPath() const;
        int getDepth() const;
        glm::fmat4 getLocalTransform() const;
        glm::fmat4 getWorldTransform() const;
        glm::fvec3 getDistanceToOrigin() const;
        Node* getOrigin() const;
        float getSpeed() const;
        float getRadius() const;
        glm::fvec3 getColor() const;

        void addChildren(Node* const& children);
        Node* removeChildren(std::string const& children_name);

        // virtual function for geometry node to override
        virtual void setTexturePath(std::string const& texture_path);
        virtual void setTextureObject(texture_object const& texture_object);
        virtual void setTexture();

        virtual std::string getTexturePath() const;
        virtual texture_object getTextureObject() const;
        virtual pixel_data getTexture() const;

    private:
        Node* parent_;
        Node* origin_;
        std::list<Node*> children_;
        std::string name_;
        std::string path_;
        int depth_;
        glm::fmat4 localTransform_;
        glm::fmat4 worldTransform_;
        glm::fvec3 distance_to_origin_;
        float speed_;
        float radius_;
        glm::fvec3 color_;
        
};

#endif      // NODE_HPP