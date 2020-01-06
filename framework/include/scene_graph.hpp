#ifndef SCENE_GRAPH_HPP
#define SCENE_GRAPH_HPP

#include "camera_node.hpp"
#include "geometry_node.hpp"
#include "PointLight_node.hpp"

#include <string>
#include <iostream>
#include <node.hpp>
#include <memory>

class SceneGraph {

    public:
   
        // static method for Singleton Pattern took from the Internet
        static SceneGraph* getInstance() {
            if(scenegraph_instance == nullptr)
                scenegraph_instance = new SceneGraph();
            return scenegraph_instance;
        }

        // Setter:
        void setName(std::string const& name);

        // Getter:
        std::string getName() const;
        Node* getRoot() const;

        void printGraph();
		void printNode(Node* node, int depth);

    private:
        // private constructors
        SceneGraph();
        ~SceneGraph();

        // private Setter
        void setRoot(Node* const& root);

        // private instance for SceneGraph --> no objects can be created
        static SceneGraph* scenegraph_instance;

        // member variables
        std::string name_;
        Node* root_;
};



#endif      // SCENE_GRAPH_HPP