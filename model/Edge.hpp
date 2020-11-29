//
// Created by saeid on 5/12/20.
//

#ifndef SEPARATOR_EDGE_HPP
#define SEPARATOR_EDGE_HPP

#include <vector>
#include <string>

namespace model {

    class Node;

    class Edge {
    private:
        int _id;
        double _max_speed;
        double _eta; /* the goal of queries */
        bool _directed;

        std::string *_name;
        Node *_from;
        Node *_to;

        void connectFromTo();

    public:
        Edge(Node *from, Node *to, bool directed = true);

        Edge(Node *from, Node *to, int id, bool directed = true);

        Edge(Node *from, Node *to, double maxSpeed, double eta, bool directed = true);

        Edge(Node *from, Node *to, int id, double maxSpeed, double eta, bool directed = true);

        Edge(Node *from, Node *to, int id, double maxSpeed, double eta, std::string *name, bool directed = true);

        Edge(const Edge& other);

        [[nodiscard]] int getId() const;

        void setId(int id);

        [[nodiscard]] double getMaxSpeed() const;

        void setMaxSpeed(double maxSpeed);

        [[nodiscard]] double getEta() const;

        void setEta(double eta);

        [[nodiscard]] std::string *getName() const;

        void setName(std::string *name);

        [[nodiscard]] Node *getFrom() const;

        void setFrom(Node *from);

        [[nodiscard]] Node *getTo() const;

        void setTo(Node *to);

        [[nodiscard]] Node *getOther(Node *thisOne);
    };

}

#endif //SEPARATOR_EDGE_HPP
