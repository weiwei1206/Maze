#include <iostream>
#include <queue>
#include <vector>
#include <limits>
#include <deque>
#define MAX_ROW 10
#define MAX_COL 10
#define MAX_WALL_NUM 81

std::deque<int> findWay(int row, int col, int wall[MAX_WALL_NUM], int start, int end);
void printMaze(int row, int col, int wall[MAX_WALL_NUM], int start, int end);
void printMazeAndWay(int row, int col, int wall[MAX_WALL_NUM], int start, int end, std::deque<int> path);

int main()
{
	//先用一个4*4的迷宫练练手
	int wall[] = {0,0,1,1,1,0,0,1,0,0,0,1,0,1,0,0,0,1,0,1,0,0,0,1};//设计好的迷宫(0表示无墙，1表示有墙)
	int row = 4;
	int col = 4;
	int start = 1;
	int end = 13;

	std::deque<int> path = findWay(row, col, wall, start, end);
	printMazeAndWay(row, col, wall, start, end ,path);
}

std::deque<int> findWay(int row, int col, int wall[MAX_WALL_NUM], int start, int end)
{
	int distance[MAX_ROW * MAX_COL] = { 0 };
	int former[MAX_ROW * MAX_COL] = { 0 };
	int visited[MAX_ROW * MAX_COL] = { 0 };
	int wallNum = row * (col - 1) + col * (row - 1);

	//init
	for (int i = 0; i < row * col; ++i)
	{
		visited[i] = 0;
		distance[i] = INT_MAX;
		former[i] = -1;
	}

	//BFS
	visited[start] = 1;
	distance[start] = 0;
	former[start] = -1;

	std::queue<int> q;
	q.push(start);

	//计算路径
	while (!q.empty())
	{
		bool over = false;

		int now = q.front();
		q.pop();

		int x = now / col;
		int y = now % col;
		int cc = (col - 1) + col;//cc是墙数组中一行元素的个数
		int upWall = (x - 1) * cc + y + (col - 1);//在墙数组中,先索引到upWall所在行，再加上【横墙】与【竖墙】的偏移，再加上正常的列偏移
		int downWall = upWall + cc;
		int leftWall = x * cc + y - 1;
		int rightWall = x * cc + y;//注意，这四扇墙可能会不是墙、或者横竖错误，下面要进行判断

		int upNode = now - col;
		int downNode = now + col;
		int leftNode = now - 1;
		int rightNode = now + 1;////注意，这四个node可能会越界，或者不在它应该在的那一行，下面要进行判断

		std::vector<int> gogo;

		//up
		if ((upWall >= 0) && (wall[upWall] == 0) && (visited[upNode] == 0))
		{
			gogo.push_back(upNode);
		}

		//down
		if ((downWall < wallNum) && (wall[downWall] == 0) && (visited[downNode] == 0))
		{
			gogo.push_back(downNode);
		}

		//left
		if (((leftWall % cc) < col - 1) && (wall[leftWall] == 0) && (visited[leftNode] == 0))
		{
			gogo.push_back(leftNode);
		}

		//right
		if (((rightWall % cc) < col - 1) && (rightWall < wallNum) && (wall[rightWall] == 0) && (visited[rightNode] == 0))
		{
			gogo.push_back(rightNode);
		}

		for (auto i = gogo.begin(); i < gogo.end(); ++i)
		{
			visited[*i] = 1;
			distance[*i] = distance[now] + 1;
			former[*i] = now;
			q.push(*i);
			if (*i == end)
			{
				over = true;
				break;
			}
		}
		if (over) break;
	}
	//存储路径
	std::deque<int> path;
	int tempNode = end;
	do
	{
		path.push_front(tempNode);
		tempNode = former[tempNode];
	} while (tempNode != -1);

	return path;


}

void printMaze(int row, int col, int wall[MAX_WALL_NUM], int start, int end)
{

}

void printMazeAndWay(int row, int col, int wall[MAX_WALL_NUM], int start, int end, std::deque<int> path)
{
	int d = 1;//记录路径长度

	//打印路径
	for (auto p = path.begin(); p < path.end(); ++p)
	{
		std::cout << "[" << *p / col << "," << *p % col << "]" << std::endl;
	}

	//打印迷宫与答案
	int mazerow = 1 + 2 * row;
	int mazecol = 1 + 4 * col;
	for (int i = 0; i < mazerow; ++i)
	{
		for (int j = 0; j < mazecol; ++j)
		{
			int nodePos = col * ((i - 1) / 2) + ((j - 1) / 4);//当光标落在横边框上的时候，该公式把nodePos放在它上侧的节点，当光标落在竖边框的时候，该公式把nodePos放在它左侧的节点。以便于一会判断墙是否绘制

			int x = nodePos / col;
			int y = nodePos % col;
			int cc = (col - 1) + col;//cc是墙数组中一行元素的个数
			int upWall = (x - 1) * cc + y + (col - 1);//在墙数组中,先索引到upWall所在行，再加上【横墙】与【竖墙】的偏移，再加上正常的列偏移
			int downWall = upWall + cc;
			int leftWall = x * cc + y - 1;
			int rightWall = x * cc + y;//注意，这四扇墙可能会不是墙、或者横竖错误，下面要进行判断

			bool isCross = (i % 2 == 0) && (j % 4 == 0);//判断是否放“+”
			bool isUpORdown = (i == 0 || i == mazerow - 1) && !isCross;//在图的第一行、最后一行和没有“+”的地方放“-”
			bool isLeftORright = (j == 0 || j == mazecol - 1) && !isCross;//在图的最左边、最右边和没有“+”的地方放“-”
			bool isHorWall = !isUpORdown && (i % 2 == 0 && j % 4 != 0) && wall[downWall] == 1;//扫描到水平墙的时候，先排除最上面和最下面（同时防止wall[]访问越界）和“+”处，然后再把坐标转换成nodePos，并判断该nodePos处的下墙
			bool isVerWall = !isLeftORright && (i % 2 == 1 && j % 4 == 0) && wall[rightWall] == 1;//原理同上，不过是判断nodePos出的右墙
			bool onPath = (i % 2 == 1 && j % 4 == 2) && (std::find(path.begin(), path.end(), nodePos) != path.end()) && start != nodePos && end != nodePos;//在寻找到的路径上放上“*”
			bool isStart = start == nodePos && (i % 2 == 1 && j % 4 == 2);//在起点处放“S”
			bool isEnd = end == nodePos && (i % 2 == 1 && j % 4 == 2);//在终点处放“E”

			bool nonePrinted = true;
			if (isCross)
			{
				std::cout << "+";
				nonePrinted = false;
			}
			if (isUpORdown)
			{
				std::cout << "-";
				nonePrinted = false;
			}
			if (isLeftORright)
			{
				std::cout << "|";
				nonePrinted = false;
			}

			if (isStart)
			{
				std::cout << "S";
				nonePrinted = false;
			}
			if (isEnd)
			{
				std::cout << "E";
				nonePrinted = false;
			}
			if (onPath)
			{
				std::cout << "*";
				++d;
				nonePrinted = false;
			}
			if (isHorWall)
			{
				std::cout << "-";
				nonePrinted = false;
			}
			if (isVerWall)
			{
				std::cout << "|";
				nonePrinted = false;
			}
			if (nonePrinted)
			{
				std::cout << " ";
			}
		}
		std::cout << std::endl;
	}
	std::cout << "路径长度：" << d << std::endl;
}