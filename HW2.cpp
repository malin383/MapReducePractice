// HW2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include<fstream>
#include "tbb/tbb.h"
#include <string>
#include <chrono>

using namespace tbb;
using namespace std;

/*hashcompare class
*
*HashCompare to define hash() and equal() for MapReduceHash
*
*/
struct HashCompare {
	//Maps Key to hashcode of type size_t
	static size_t hash(const string& x) {
		size_t h = 0;
		for (const char* s = x.c_str(); *s; ++s) {
			h = (h * 17) ^ *s;
		}
		return h;
	}
	static bool equal(const string& x, const string& y) {
		return x == y;
	}
};


//Hash map to store words and their occurences
typedef concurrent_hash_map <string, int, HashCompare> MapReduceHash;

typedef concurrent_vector<string> NameCounts;
typedef concurrent_vector<string> NameDictionary;

//------------------PROBLEM 1---------------
/*
*countWords: reads a text file and returns number of characters
*
*args: const char* fileName
*/
NameCounts countWords(const char* fileName) {
    fstream file;
	file.open(fileName);
	string word;

	//Make hashmaps
	MapReduceHash ch;
	MapReduceHash::accessor a;
	NameCounts nc;
	NameDictionary nd;

	//reading words from file
	while (file >> word) {
		if (ch.find(a, word)) {
			a->second += 1;
			//cout << a->first << ": " << a->second << "\n";
			a.release();
		}
		else {
			ch.insert(a, word);
			a->second += 1;
			//cout << a->first << ": " << a->second << "\n";
			nd.grow_by(1);
			nd[nd.size()-1] = word;
			a.release();
		}
	}
	for (int i = 0; i < nd.size(); i++)
	{
		if (ch.find(a, nd[i])) {
			nc.grow_by(1);
			nc[i] = nd[i] + ": " + to_string(a->second);
		}
	}
	return nc;
}

//------------------PROBLEM 2---------------
//Vector that stores names in queues
typedef concurrent_vector<string> NameStorage;

//HashMap to keep track of names
typedef concurrent_hash_map <string, int, HashCompare> MapReduceHash;

NameCounts countWordsMapReduce(const char* fileName) {
	fstream file;
	file.open(fileName);
	string word;

	//Make hashmaps; Int value will now be index val of string
	MapReduceHash ch;
	MapReduceHash::accessor a;
	
	//Vector storage for math later
	NameStorage ns;
	NameCounts nc;
	NameDictionary nd;

	//reading words from file
	while (file >> word) {
		if (ch.find(a, word)) {
			ns.grow_by(1);
			ns[ns.size()-1] = word;
		}
		else {
			ch.insert(a, word);
			ns.grow_by(1);
			ns[ns.size()-1] = word;
			nd.grow_by(1);
			nd[ns.size() - 1] = word;
		}
	}
	
	parallel_for(blocked_range<int>(0, ns.size()), [&](const blocked_range<int>& r) {
		for (auto i = r.begin(); i < r.end(); i++)
			{
				ch.find(a, ns[i]);
				a->second += 1;
			}
	});
	parallel_for(blocked_range<int>(0, nd.size()), [&](const blocked_range<int>& r) {
		for (auto i = r.begin(); i < r.end(); i++)
		{
			ch.find(a, ns[i]);
			nc.grow_by(1);
			nc[i] = nd[i] + ": " + to_string(a->second);
		}
		});

	return nc;
}

//PROBLEM 3
struct HashCompareWhiteHouse {
	//Maps Key to hashcode of type size_t
	static size_t hash(const string& x) {
		size_t h = 0;
		for (const char* s = x.c_str(); *s; ++s) {
			h = (h * 17) ^ *s;
		}
		return h;
	}
	static bool equal(const string& x, const string& y) {
		return x == y;
	}
};

typedef concurrent_hash_map<string, string, HashCompareWhiteHouse> WhiteHouseVisits;

int whiteHouseData() {
	WhiteHouseVisits whv;
	return 0;
}
//Most frequent visitors


//Main method
int main()
{
	//INPUT FILE
	string fileName;
	cout << "Enter File Name:\n";
	cin >> fileName;
	
	
	//PROBLEM 1
	auto start = std::chrono::high_resolution_clock::now();
	//work here
	//NameCounts wordCount = countWords(fileName);
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	//cout << "Result in " << elapsed.count() << "s\n";
	/*for (int i = 0; i < wordCount.size(); i++)
	{
		cout << wordCount[i] << "\n";
	}*/

	//PROBLEM 2
	/*start = std::chrono::high_resolution_clock::now();
	//work here
	NameCounts wordCountPar = countWords(fileName.c_str());
	finish = std::chrono::high_resolution_clock::now();
	elapsed = finish - start;
	cout << "Result in " << elapsed.count() << "s\n";
	for (int i = 0; i < wordCountPar.size(); i++)
	{
		cout << wordCountPar[i] << "\n";
	}*/

}