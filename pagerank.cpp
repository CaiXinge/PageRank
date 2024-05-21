#include<iostream>
#include<fstream>
#include<vector>
#include<map>
#include<algorithm>
using namespace std;
const double STOPLOSS = 1e-10;
typedef unsigned int Node_ID;
typedef vector<Node_ID> Links;
typedef map<Node_ID, Links> Page;
vector<unsigned int> in_degree;
vector<unsigned int> out_degree;
Page in_page;
Page out_page;
typedef vector<double> Rank;
Rank new_rank, old_rank;
unsigned int Node_num = 0;
typedef map<Node_ID, double> Node_Score;
Node_Score Node_score;
typedef pair<Node_ID, double> PAIR;
unsigned int max(unsigned int a, unsigned int b)
{
	return a > b ? a : b;
}
int Init(const char* filename)
{
	fstream input(filename);
	Node_ID from = 0;
	Node_ID to = 0;
	while (!input.eof() && input >> from >> to)
	{
		in_page[to].push_back(from);
		out_page[from].push_back(to);
		Node_num = max(Node_num, max(from, to));
	}
	input.close();
	in_degree.resize(Node_num + 1);
	out_degree.resize(Node_num + 1);
	for (unsigned int i = 0; i <= Node_num; i++)
	{
		in_degree[i] = in_page[i].size();
		out_degree[i] = out_page[i].size();
	}
	unsigned int count = Node_num;
	for (unsigned int i = 0; i <= Node_num; i++)
	{
		if (in_degree[i] == 0 && out_degree[i] == 0)
		{
			count--;
		}
	}
	return count + 1;
}
void get_rank(int n, double teleport)
{
	old_rank.resize(Node_num + 1, 0);
	new_rank.resize(Node_num + 1, 1.0 / n);
	double loss = 0.0;
	do
	{
		loss = 0.0;
		for (auto i : in_page)
		{
			new_rank[i.first] = 0.0;
			for (int j = 0; j < i.second.size(); j++)
			{
				if (out_degree[i.second[j]] != 0)
				{
					new_rank[i.first] += teleport * (old_rank[i.second[j]] / out_degree[i.second[j]]);
				}
				else
				{
					new_rank[i.first] += 0;
				}
			}
		}
		double S = 0.0;
		for (auto i : new_rank)
		{
			S += i;
		}
		for (int i = 0; i < new_rank.size(); i++)
		{
			new_rank[i] += (1.0 - S) / n;
		}
		for (int i = 0; i < new_rank.size(); i++)
		{
			double temp = new_rank[i] - old_rank[i];
			if (temp > 0)
			{
				loss += temp;
			}
			else
			{
				loss -= temp;
			}
		}
		for (int i = 0; i < new_rank.size(); i++)
		{
			old_rank[i] = new_rank[i];
		}
		cout << "current loss: " << loss << endl;
	} while (loss > STOPLOSS);
	cout << "final loss: " << loss << endl;
	ofstream result("result.txt");
	for (auto i : in_page)
	{
		result << i.first << "\t" << new_rank[i.first] << endl;
	}
	result.close();
	return;
}
bool cmp_value(const PAIR& lhs, const PAIR& rhs)
{
	return lhs.second > rhs.second;
}
void gettop100()
{
	for (int i = 0; i < new_rank.size(); i++)
	{
		Node_score[i] = new_rank[i];
	}
	vector<PAIR> nodes_top(Node_score.begin(), Node_score.end());
	sort(nodes_top.begin(), nodes_top.end(), cmp_value);
	ofstream topnode("top100.txt");
	for (int i = 0; i < 100; i++)
	{
		topnode << nodes_top[i].first << '\t' << nodes_top[i].second << endl;
	}
	topnode.close();
	return;
}
int main()
{
	const char* filename = "Data.txt";
	double teleport = 0.85;
	int count = Init(filename);
	ofstream out("degree.txt");
	for (auto i : in_page)
	{
		out << i.first << '\t' << "out_degree: " << out_degree[i.first] << "\t\tin_degree" << in_degree[i.first] << endl;
	}
	out.close();
	get_rank(count, teleport);
	gettop100();
	return 0;
}