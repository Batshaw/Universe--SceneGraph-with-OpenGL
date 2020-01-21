#ifndef GEOMETRY_NODE_HPP
#define GEOMETRY_NODE_HPP


#include <node.hpp>
#include <model.hpp>
#include <structs.hpp>      // for texture_object

class GeometryNode : public Node {

        public:
            // Constructors
            GeometryNode();
            GeometryNode(std::string const& name, Node* const& parent, std::string const& path, int depth, Node* const& origin);
            ~GeometryNode();

            void setGeometry(model const& geometry);
            void setTexturePath(std::string const& texture_path);
            void setTextureObject(texture_object const& texture_object);

            model getGeometry() const;
            std::string getTexturePath() const;
            texture_object getTextureObject() const;
        
        private:
            model geometry_;
            std::string texture_path_;
            texture_object texture_object_;

};

#endif      //GEOMETRY_NODE_HPP