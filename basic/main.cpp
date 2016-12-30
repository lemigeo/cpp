#include <iostream>
#include <stdlib.h>
#include <list>     //when you many insertion or erasure element
#include <vector>   //random access
#include <map>
#include <string>
#include <sstream>
#include <iterator>

using namespace std;

int main(int argc, char *argv[])
{
    cout << "Hello World" << endl;
    list<int> items;

    //loop
    for(auto i=0; i<3; i++)
    {
        items.push_back(i);
    }

    //find position and insert
    auto it = find(items.begin(), items.end(), 1);
    if (it != items.end()) {
        items.insert(it, 10);
    }

    //foreach loop
    map<char,int> map;
    for(auto item : items)
    {        
        map.insert(pair<char,int>(item, 'a'));
    }

    //map loop
    for (std::map<char,int>::iterator it = map.begin(); it != map.end(); it++)
    {        
        printf("result %c : %d\n", it->second, it->first);
    } 
    
    string csv = "world,peace";    
    //sub string    
    string sub = csv.substr(0, 4);    
    
    //string to character array    
    printf("%s\n", sub.c_str());        
    
    //split
    stringstream ss(csv);
    vector<string> vstrings;
    string buf;
    while(getline(ss, buf, ','))
    {
        printf("%s\n", buf.c_str());
    }
       
    auto line = NULL;
    cin >> line; 
    return 0;
}
