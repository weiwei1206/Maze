#include <iostream>
#include <queue>
#include <vector>
#include <limits>
#include <deque>
#include <time.h>
#define MAX_ROW 10
#define MAX_COL 10
#define MAX_NODE_NUM (MAX_ROW*MAX_COL)
#define MAX_WALL_NUM ((MAX_ROW)*(MAX_COL-1)+(MAX_COL)*(MAX_ROW-1))

std::deque<int> findWay(int row, int col, int wall[], int start, int end);
void printMaze(int row, int col, int wall[], int start, int end);
void printMazeAndWay(int row, int col, int wall[], int start, int end, std::deque<int> path);
void createMaze(int& row, int& col, int wall[], int& start, int& end);
//支持对并查集的操作
int find(int x, int pre[]);
void join(int x, int y, int pre[]);

int main()
{
	int wall[MAX_WALL_NUM];
	int row;
	int col;
	int start;
	int end;
	while (1)
	{
		createMaze(row, col, wall, start, end);
		printMaze(row, col, wall, start, end);
		system("pause");
		system("cls");
		std::deque<int> path = findWay(row, col, wall, start, end);
		printMazeAndWay(row, col, wall, start, end, path);
		system("pause");
		system("cls");
	}
}

std::deque<int> findWay(int row, int col, int wall[], int start, int end)
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

void printMaze(int row, int col, int wall[], int start, int end)
{
	//打印迷宫
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
}

void printMazeAndWay(int row, int col, int wall[], int start, int end, std::deque<int> path)
{
	int d = 1;//记录路径长度

/*	//打印路径
	for (auto p = path.begin(); p < path.end(); ++p)
	{
		std::cout << "->[" << *p / col << "," << *p % col << "]" ;
	}
	std::cout << std::endl;
*/
	//打印迷宫与答案
	int mazerow = 1 + 2 * row;
	int mazecol = 1 + 4 * col;
	for (int i = 0; i < mazerow; ++i)
	{
		for (int j = 0; j < mazecol; ++j)
		{
			//节点坐标
			int nodePos = col * ((i - 1) / 2) + ((j - 1) / 4);//当光标落在横边框上的时候，该公式把nodePos放在它上侧的节点，当光标落在竖边框的时候，该公式把nodePos放在它左侧的节点。以便于一会判断墙是否绘制

			int x = nodePos / col;
			int y = nodePos % col;
			int cc = (col - 1) + col;//cc是墙数组中一行元素的个数

			//这是nodePos节点对应的四个墙在墙数组中的pos，请注意,他们有可能越界（指向地图边界）
			int upWall = (x - 1) * cc + y + (col - 1);
			int downWall = upWall + cc;
			int leftWall = x * cc + y - 1;
			int rightWall = x * cc + y;

			//画墙
			bool isCross = (i % 2 == 0) && (j % 4 == 0);//判断是否放“+”
			bool isUpORdown = (i == 0 || i == mazerow - 1) && !isCross;//在图的第一行、最后一行和没有“+”的地方放“-”
			bool isLeftORright = (j == 0 || j == mazecol - 1) && !isCross;//在图的最左边、最右边和没有“+”的地方放“-”
			bool isHorWall = !isUpORdown && (i % 2 == 0 && j % 4 != 0) && wall[downWall] == 1;//扫描到水平墙的时候，先排除最上面和最下面（同时防止wall[]访问越界）和“+”处，然后再把坐标转换成nodePos，并判断该nodePos处的下墙
			bool isVerWall = !isLeftORright && (i % 2 == 1 && j % 4 == 0) && wall[rightWall] == 1;//原理同上，不过是判断nodePos出的右墙

			//画路、起点、终点
			bool onPath = (i % 2 == 1 && j % 4 == 2) && (std::find(path.begin(), path.end(), nodePos) != path.end()) && start != nodePos && end != nodePos;//在寻找到的路径上放上“*”
			bool isStart = start == nodePos && (i % 2 == 1 && j % 4 == 2);//在起点处放“S”
			bool isEnd = end == nodePos && (i % 2 == 1 && j % 4 == 2);//在终点处放“E”
			
			//画箭头
			auto pUp = std::find(path.begin(), path.end(), nodePos);
			auto pDown = std::find(path.begin(), path.end(), nodePos+col);
			bool found = pUp != path.end() && pDown != path.end();
			bool isGoUp = !isUpORdown && (i % 2 == 0 && j % 4 == 2) && (pDown < pUp) && found && !isHorWall;
			bool isGoDown = !isUpORdown && (i % 2 == 0 && j % 4 == 2) && (pUp < pDown) && found && !isHorWall;

			auto pLeft = std::find(path.begin(), path.end(), nodePos);
			auto pRight = std::find(path.begin(), path.end(), nodePos + 1);
			found = pLeft != path.end() && pRight != path.end();
			bool isGoLeft = !isLeftORright && (i % 2 == 1 && j % 4 == 0) && (pRight < pLeft) && found && !isVerWall;
			bool isGoRight = !isLeftORright && (i % 2 == 1 && j % 4 == 0) && (pLeft < pRight) && found && !isVerWall;



			//开画
			bool nonePrinted = true;
			//画箭头
			if (isGoUp)
			{
				std::cout << "|";
				nonePrinted = false;
			}
			if (isGoDown)
			{
				std::cout << "|";
				nonePrinted = false;
			}
			if (isGoLeft)
			{
				std::cout << "-";
				nonePrinted = false;
			}
			if (isGoRight)
			{
				std::cout << "-";
				nonePrinted = false;
			}
			//画墙
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
			//画起点终点、路径
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
			//画墙
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
			//补空格
			if (nonePrinted)
			{
				std::cout << " ";
			}
		}
		std::cout << std::endl;
	}
	std::cout << "路径长度：" << d << std::endl;
}

void createMaze(int& row, int& col, int wall[], int& start, int& end)
{
	srand((unsigned)time(NULL));

	row = rand() % (MAX_ROW - 3) + 3;
	col = rand() % (MAX_COL - 3) + 3;//这里注意要先设置好row和col的值
	int cc = col - 1 + col;
	int nodeNum = row * col;
	int wallNum = row * (col - 1) + col * (row - 1);
	int pre[MAX_NODE_NUM];

	//init
	for (int i = 0; i < wallNum; ++i)
	{
		wall[i] = 1;
	}
	for (int i = 0; i < nodeNum; ++i)
	{
		pre[i] = i;
	}

	//随机Prim算法生成迷宫
	while (1)
	{
		//判断是否结束算法
		bool endFlag = true;
		for (int i = 0; i < nodeNum; ++i)
		{
			if (pre[i] != pre[0])
			{
				endFlag = false;
				break;
			}
		}

		//随机删除一个墙壁
		int ran = rand() % wallNum;
		if (ran % (col - 1 + col) < (col - 1))//如果是竖直墙壁
		{
			int leftNode = (ran / cc) * col + ran % cc;
			int rightNode = leftNode + 1;
			if (find(leftNode, pre) != find(rightNode, pre))
			{
				join(leftNode, rightNode, pre);
				wall[ran] = 0;
			}
		}
		else//如果是水平墙壁
		{
			int upNode = (ran / cc) * col + (ran - (col - 1)) % cc;
			int downNode = upNode + col;
			if (find(upNode, pre) != find(downNode, pre))
			{
				join(upNode, downNode, pre);
				wall[ran] = 0;
			}
		}
		do
		{
			start = rand() % nodeNum;
			end = rand() % nodeNum;
		} while (start == end);

		if (endFlag) break;
	}
}

int find(int x, int pre[])
{
	{
		int r = x;
		while (pre[r] != r)                                                                                              //返回根节点 r
			r = pre[r];

		int i = x, j;
		while (i != r)                                                                                                        //路径压缩
		{
			j = pre[i]; // 在改变上级之前用临时变量  j 记录下他的值 
			pre[i] = r; //把上级改为根节点
			i = j;
		}
		return r;
	}
}

void join(int x, int y, int pre[])
{
	int fx = find(x, pre), fy = find(y, pre);
	if (fx != fy)
		pre[fx] = fy;
}