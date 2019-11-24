#ifndef SCENE_GRAPH_HPP
#define SCENE_GRAPH_HPP

#include "camera_node.hpp"
#include "geometry_node.hpp"

#include <string>
#include <node.hpp>

class SceneGraph {

    public:
        // static method for Singleton Pattern
        static SceneGraph *getInstance() {
            if(!scenegraph_instance)
                scenegraph_instance = new SceneGraph;
            return scenegraph_instance;
        }
        // Setter:
        void setName(std::string const& name);
        void setRoot(Node const& root);

        // Getter:
        std::string getName();
        Node getRoot();

        std::string printGraph();

    private:
        // private instance for SceneGraph --> no objects can be created
        static SceneGraph *scenegraph_instance;
        // member variables
        std::string name_;
        Node root_;
};



#endif      // SCENE_GRAPH_HPP