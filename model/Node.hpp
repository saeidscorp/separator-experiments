//
// Created by saeid on 5/12/20.
//

#ifndef SEPARATOR_NODE_HPP
#define SEPARATOR_NODE_HPP

#include <vector>
#include <string>
#include <map>

#include "modeltypes.hpp"

namespace model {

    class Edge;

    class Node {
        friend Edge;

    private:
        int _id;
        double _lon, _lat;

        std::string *_name;
        std::vector<Edge *> *_edges;
        std::map<int, Node *> *_neighs;
        std::map<Node *, Edge *> *_edge_map;

        void addNeigh(Node *other);

        void remove_neigh(Node *other);

        void addEdge(Edge *edge);

        void remove_edge(Edge *edge);

    public:

        explicit Node(int id);

        Node(int id, double lon, double lat);

        Node(int id, double lon, double lat, std::string name);

        Node(const Node& other);

        [[nodiscard]] int getId() const;

        void setId(int id);

        [[nodiscard]] double getLon() const;

        void setLon(double lon);

        [[nodiscard]] double getLat() const;

        void setLat(double lat);

        [[nodiscard]] std::string * getName() const;

        void setName(std::string name);

        [[nodiscard]] std::vector<Edge *> getEdges() const;

        [[nodiscard]] std::vector<Node *> getNeighs() const;

        Edge *connect(Node *other);

        Node *connect(Edge *existingEdge);

        void disconnect(Node *other);

        void disconnect(Edge *edge);

        std::optional<Edge *> getEdgeOf(Node *node);

        static double distance(const Node *one, const Node *another);

    };

}

#endif //SEPARATOR_NODE_HPP
