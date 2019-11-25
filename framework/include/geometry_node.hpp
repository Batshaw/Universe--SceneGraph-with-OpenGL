#ifndef GEOMETRY_NODE_HPP
#define GEOMETRY_NODE_HPP


#include <node.hpp>
#include <model.hpp>

class GeometryNode : public Node {

        public:
            // Constructors
            GeometryNode();
            GeometryNode(std::string const& name, Node* const& parent, std::string const& path, int depth, Node* const& origin);
            ~GeometryNode();

            void setGeometry(model const& geometry);
            model getGeometry() const;
        
        private:
            model geometry_;

};

#endif      //GEOMETRY_NODE_HPP