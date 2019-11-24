#ifndef CAMERA_NODE_HPP
#define CAMERA_NODE_HPP

#include <node.hpp>
#include <glm/glm.hpp>

class CameraNode : public Node {

    public:
        //Constructors
        CameraNode();
        CameraNode(std::string const& name, bool const& isPerspective, bool const& isEnabled, glm::fmat4 const& projectionMatrix);
        
        ~CameraNode();

    private:
        bool isPerspective_;
        bool isEnabled_;
        glm::fmat4 projectionMatrix_;
        std::string name_;

};



#endif      //CAMERA_NODE_HPP