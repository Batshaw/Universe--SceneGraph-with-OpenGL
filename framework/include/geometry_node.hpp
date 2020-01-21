#ifndef GEOMETRY_NODE_HPP
#define GEOMETRY_NODE_HPP


#include <node.hpp>
#include <model.hpp>
#include <structs.hpp>      // for texture_object
#include <pixel_data.hpp>   // for pixel_data
#include <texture_loader.hpp>

class GeometryNode : public Node {

        public:
            // Constructors
            GeometryNode();
            GeometryNode(std::string const& name, Node* const& parent, std::string const& path, int depth, Node* const& origin);
            ~GeometryNode();

            void setGeometry(model const& geometry);
            void setTexturePath(std::string const& texture_path) override;
            void setTextureObject(texture_object const& texture_object) override;
            void setTexture() override;

            model getGeometry() const;
            std::string getTexturePath() const override;
            texture_object getTextureObject() const override;
            pixel_data getTexture() const override;
        
        private:
            model geometry_;
            pixel_data texture_;
            std::string texture_path_;
            texture_object texture_object_;

};

#endif      //GEOMETRY_NODE_HPP