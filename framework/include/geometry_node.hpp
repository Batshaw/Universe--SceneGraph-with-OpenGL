#ifndef GEOMETRY_NODE_HPP
#define GEOMETRY_NODE_HPP


#include <node.hpp>
#include <model.hpp>

class GeometryNode : public Node {

        public:
            // Constructors
            GeometryNode();
            GeometryNode(std::string const& name, model const& geometry);
            ~GeometryNode();

            void setGeometry(model const& geometry);
            model getGeometry();
        
        private:
            model geometry_;

};

#endif      //GEOMETRY_NODE_HPP