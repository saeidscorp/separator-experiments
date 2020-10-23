//
// Created by saeid on 5/12/20.
//

#ifndef SEPARATOR_NODE_HPP
#define SEPARATOR_NODE_HPP

#include <vector>
#include <string>
#include <map>

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

        void addNeigh(Node *other);

        void addEdge(Edge *edge);

    public:

        explicit Node(int id);

        Node(int id, double lon, double lat);

        Node(int id, double lon, double lat, std::string name);

        [[nodiscard]] int getId() const;

        void setId(int id);

        [[nodiscard]] double getLon() const;

        void setLon(double lon);

        [[nodiscard]] double getLat() const;

        void setLat(double lat);

        [[nodiscard]] std::string * getName() const;

        void setName(std::string name);

        [[nodiscard]] const std::vector<Edge *> *getEdges() const;

        [[nodiscard]] std::map<int, Node *> *getNeighs() const;

        void addEdge(Node *other);

    };

}

#endif //SEPARATOR_NODE_HPP
