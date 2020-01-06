#ifndef POINTLIGHT_NODE_HPP
#define POINTLIGHT_NODE_HPP

#include "node.hpp"

class PointLightNode : public Node {

    private:
        float lightIntensity_;
        glm::fvec3 lightColor_;

    public:
        PointLightNode();
        PointLightNode(float lightIntensity, glm::fvec3 const& lightColor, std::string const& name, Node* const& parent, std::string const& path, int depth, Node* const& origin);
        
        ~PointLightNode();

        void setLightIntensity(float lightIntensity);
        float gettLightIntensity() const;

        void setLightColor(glm::fvec3 const& lightColor);
        glm::fvec3 getLightColor() const;
};

#endif      // POINTLIGHT_NODE_HPP