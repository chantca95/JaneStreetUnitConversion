//
//  main.cpp
//  JaneStreetUnits
//
//  Created by Alex Chan on 5/11/23.
//

#include <iostream>
#include <tuple>
#include <string>
#include <vector>
#include <sstream>
#include <queue>
#include <set>

using namespace std;

class Node {
public:
    string unit;
    unordered_map<string, float> edges;
    Node(string unit) : unit(unit){}
    void addEdge(string toUnit, float multiplier) {
        edges[toUnit] = multiplier;
    }
    void printNode() {
        cout << "This Node is of unit: " << unit << ", and it has the following edges: " << endl;
        for (auto it = edges.begin(); it != edges.end(); it++) {
            cout << "To get to unit: " << it->first << ", you multiply by " << it->second << endl;
        }
    }
};

void parse_facts(vector<tuple<string, float, string>> &facts, unordered_map<string, Node*> &mp) {
    for (auto fact : facts) {
        string from = get<0>(fact);
        float multiplier = get<1>(fact);
        string to = get<2>(fact);
        
        Node* fromNode = mp.count(from) ? mp[from] : new Node(from);
        Node* toNode = mp.count(to) ? mp[to] : new Node(to);
        fromNode->addEdge(to, multiplier);
        toNode->addEdge(from, 1/multiplier);
                
        if (!mp.count(from)) {
            mp[from] = fromNode;
        }
        if (!mp.count(to)) {
            mp[to] = toNode;
        }
    }
}

float answerQuery(string query, unordered_map<string, Node*> &mp) {
    vector<string> tokens;
    stringstream ss(query);
    string token;
    while (getline(ss, token, ' ')) {
        tokens.push_back(token);
    }
    
    float qty = stof(tokens[0]);
    string fromUnit = tokens[1];
    string toUnit = tokens[2];
    
    queue<pair<string, float>> q;
    q.push(pair<string,float>(fromUnit, 1.0));
    set<string> visited;
    visited.insert(fromUnit);
    
    while (!q.empty()) {
        auto size = q.size();
        for (int i=0; i<size; i++) {
            string currUnit = q.front().first;
            float currentMultiplier = q.front().second;
            q.pop();
            
            Node* currNode = mp[currUnit];
            if (currNode->edges.count(toUnit)) {
                cout << "Existing entry!" << endl;
                currentMultiplier *= currNode->edges[toUnit];
                mp[fromUnit]->addEdge(toUnit, currentMultiplier);
                mp[toUnit]->addEdge(fromUnit, 1/currentMultiplier);
                return qty * currentMultiplier;
            } else {
                cout << "Not an existing entry!" << endl;
                for (auto edge : currNode->edges) {
                    string nextUnit = edge.first;
                    if (visited.count(nextUnit)) {
                        continue;
                    }
                    float nextMultiplier = edge.second * currentMultiplier;
                    q.push(pair<string,float>(nextUnit, nextMultiplier));
                    visited.insert(nextUnit);
                }
            }
        }
    }
    // if you reached here, it means this query is impossible.
    // we should add an Edge with value 0, so that future attempts will shortcircuit
    mp[fromUnit]->addEdge(toUnit, 0);
    return 0;
}

int main(int argc, const char * argv[]) {
    vector<tuple<string, float, string>> facts;
    facts.push_back(tuple<string, float, string>("m", 3.28, "ft"));
    facts.push_back(tuple<string, float, string>("ft", 12, "in"));
    facts.push_back(tuple<string, float, string>("in", 2.5, "cm"));

    facts.push_back(tuple<string, float, string>("days", 24, "hr"));
    facts.push_back(tuple<string, float, string>("hr", 60, "min"));
    facts.push_back(tuple<string, float, string>("min", 60, "sec"));
    
    unordered_map<string, Node*> mp;
    
    parse_facts(facts, mp);
    
    string query1 = "12 ft cm";
    cout << answerQuery(query1, mp) << endl;
    string query2 = "31 ft cm";
    cout << answerQuery(query2, mp) << endl;
    string query3 = "4143 cm ft";
    cout << answerQuery(query3, mp) << endl;
    string query4 = "1 days sec";
    cout << answerQuery(query4, mp) << endl;
    string query5 = "2 hr sec";
    cout << answerQuery(query5, mp) << endl;
    string query6 = "2 m hr";
    cout << answerQuery(query6, mp) << endl;
    string query7 = "0.2 m hr";
    cout << answerQuery(query7, mp) << endl;
    
    return 0;
}

