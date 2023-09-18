#include "test.hpp"
#include <iostream>
#include <vector>
using namespace std;


int main()
{

    SkipList<unsigned, unsigned> sl;
    
	for(unsigned i=0; i < 10; i++)
	{
		sl.insert(i,i+ 100);
	}
    
    
	std::vector<unsigned> heights;
	for (unsigned i = 0; i < 10; i++) {
		sl.insert(i, i);
		heights.push_back(sl.height(i));
	}

	sl.insert(255, 255);
	heights.push_back(sl.height(255));
    for(unsigned int i = 0; i < heights.size(); i++) { cout << heights[i] << " "; }
    cout << endl <<  sl.numLayers() << endl;

		// The expected height for 255 is 12 because there are fewer than 16 nodes in
		// the skip list when 12 is added.




    
    //sl.see();
    //vector<unsigned> res = sl.allKeysInOrder();
    //for(unsigned int i = 0; i < res.size(); i++) { cout << res[i] << " "; }
    //cout << endl;

    //cout << sl.isSmallestKey(0) << endl;



    return 0;
}

// g++ project.cpp -std=c++11 -o p2
