#pragma once

#include <iostream>

namespace SparCraft
{
namespace GraphViz
{
	class Property
	{
	public:
		std::string prop;
		std::string value;
		Property(std::string p, std::string val) : prop(p), value(val) { }	
		void print(std::ofstream & out) { out << prop << "=\"" << value << "\", "; }
	};

	class Node
	{	
	public:
		std::vector<Property> props;
		std::string name;
		Node(std::string n) : name(n) {}
		Node() {}
	
		void set(std::string p, std::string v) { props.push_back(Property(p,v)); }
		void print(std::ofstream & out)
		{
			out << name << " [";
			for (size_t p(0); p<props.size(); ++p) { props[p].print(out); }
			out << "]\n";
		}
	};

	class Edge
	{
	public:
		std::pair<std::string, std::string> nodes;
		std::vector<Property> props;

		Edge(std::string n1, std::string n2) : nodes(n1, n2) {}
		Edge(Node & n1, Node & n2) : nodes(n1.name, n2.name) {}
		void set(std::string p, std::string v) { props.push_back(Property(p,v)); }
		void print(std::ofstream & out)
		{
			out << nodes.first << " -> " << nodes.second << " [";
			for (size_t p(0); p<props.size(); ++p) { props[p].print(out); }
			out << "]\n";
		}
	};

	class Graph
	{
		std::vector<Node> nodes;
		std::vector<Edge> edges;
		std::vector<Property> props;
		std::string name;

	public:

		Graph(std::string n) : name(n) {}
		void set(std::string p, std::string v) { props.push_back(Property(p,v)); }
		void addNode(Node & n) { nodes.push_back(n); }
		void addEdge(Edge & e) { edges.push_back(e); }

        void print(std::ofstream & out)
		{
			out << "digraph " << name << " {\n";
			for (size_t p(0); p<props.size(); ++p) { out << props[p].prop << "=\"" << props[p].value << "\"\n"; }
			for (size_t n(0); n<nodes.size(); ++n) { nodes[n].print(out); }
			for (size_t e(0); e<edges.size(); ++e) { edges[e].print(out); }
		    out << "}\n";
		}

	};

	/*
	void example()
	{
		// construct a graph, give it a name and some properties
		GraphViz::Graph G("g");
		                G.set("bgcolor", "#000000");

		// construct some nodes with unique identifiers then give them some properties
		GraphViz::Node  n1("n1");
		                n1.set("fontcolor", "#ffffff");
						n1.set("style", "filled");
						n1.set("fillcolor", "#ff0000");
						n1.set("label", "Node 1");
						n1.set("shape", "box");
						n1.set("color", "white");

		GraphViz::Node  n2("n2");
		                n2.set("fontcolor", "#ff00ff");
						n2.set("style", "filled");
						n2.set("fillcolor", "#ffffff");
						n2.set("label", "Node 2");

		// construct an edge, either on unique identifier or node object
		GraphViz::Edge  e1(n1, n2);
                        e1.set("label", "Edge 1");
						e1.set("color", "white");

		// add the elements to the graph, this can happen at any time in any order
		G.addNode(n1);
		G.addNode(n2);
		G.addEdge(e1);

		// print out the graph
		G.print();

		// to use this output you need to have GraphViz installed
		// to run the above example, pipe the output to test.txt then run
		//     dot < test.txt -Tpng > test.png
	}*/
}
}