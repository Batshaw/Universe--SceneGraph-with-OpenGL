#ifndef CAMERA_NODE_HPP
#define CAMERA_NODE_HPP

#include <node.hpp>
#include <glm/glm.hpp>

class CameraNode : public Node {

    public:
        //Constructors
        CameraNode();
        CameraNode(bool isPerspective, bool isEnabled, glm::mat4 const& projectionMatrix);
        
        ~CameraNode();

    private:
        bool insPerspective_;
        bool isEnabled_;
        glm::mat4 projectionMatrix_;

};



#endif      //CAMERA_NODE_HPP