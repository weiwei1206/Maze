#include <iostream>
#include <queue>
#include <vector>
#include <limits>
#include <deque>
#include <time.h>
#include <Windows.h>
#include <conio.h>
#define MAX_ROW 10
#define MAX_COL 10
#define MAX_NODE_NUM (MAX_ROW*MAX_COL)
#define MAX_WALL_NUM ((MAX_ROW)*(MAX_COL-1)+(MAX_COL)*(MAX_ROW-1))

std::deque<int> findWayBFS(int row, int col, int wall[], int start, int end, bool proc);//proc：是否用动画展示过程
std::deque<int> findWayDFS(int row, int col, int wall[], int start, int end, bool proc);
void DFS_VISIT(int row, int col, int wall[], int start, int end, int distance[], int former[], int visited[], int now, bool& allOver, bool proc);
void printMaze(int row, int col, int wall[], int start, int end);
void printMazeAndWay(int row, int col, int wall[], int start, int end, std::deque<int> path);
void createMaze(int& row, int& col, int wall[], int& start, int& end);//本函数用到随机数，由于本函数可能被短时间多次调用，故不在函数内srand，请在别处自行srand
//支持对并查集的操作
int find(int x, int pre[]);
void join(int x, int y, int pre[]);
void printState(int row, int col, int wall[], int start, int end, int distance[]);

int main()
{
	//定义一些变量供函数使用
	int wall[MAX_WALL_NUM];
	int row;
	int col;
	int start;
	int end;
	char ch;

	//为createMaze撒随机数种子
	srand((unsigned)time(NULL));

	createMaze(row, col, wall, start, end);
	printMaze(row, col, wall, start, end);
	std::deque<int> path = findWayBFS(row, col, wall, start, end, false);
	std::cout << "输入1重新生成地图，输入2使用BFS寻路，输入3使用DFS寻路，输入4显示BFS寻找到的路径, 输入5显示DFS寻找到的路径，输入其他退出程序：";
	while (1)
	{
		ch = _getch();
		if (ch == '1')
		{
			system("cls");
			createMaze(row, col, wall, start, end);
			printMaze(row, col, wall, start, end);
		}
		else if (ch == '2')
		{
			system("cls");
			path = findWayBFS(row, col, wall, start, end, true);
			printMazeAndWay(row, col, wall, start, end, path);
		}
		else if (ch == '3')
		{
			system("cls");
			path = findWayDFS(row, col, wall, start, end, true);
			printMazeAndWay(row, col, wall, start, end, path);
		}
		else if (ch == '4')
		{
			system("cls");
			path = findWayBFS(row, col, wall, start, end, false);
			printMazeAndWay(row, col, wall, start, end, path);
		}
		else if (ch == '5')
		{
			system("cls");
			path = findWayDFS(row, col, wall, start, end, false);
			printMazeAndWay(row, col, wall, start, end, path);
		}
		else
		{
			return 0;
		}
		std::cout << "输入1重新生成地图，输入2使用BFS寻路，输入3使用DFS寻路，输入4显示BFS寻找到的路径, 输入5显示DFS寻找到的路径，输入其他退出程序：";
	}
}

std::deque<int> findWayBFS(int row, int col, int wall[], int start, int end, bool proc)
{
	int distance[MAX_ROW * MAX_COL] = { 0 };//各顶点到起点距离，初始化为无穷
	int former[MAX_ROW * MAX_COL] = { 0 };//存储第i个节点的前驱节点，初始化为-1
	int visited[MAX_ROW * MAX_COL] = { 0 };//标记第i个节点是否被访问过，0表示未被访问，1表示已被访问
	int wallNum = row * (col - 1) + col * (row - 1);//计算墙壁数量上限，方便后面计算

	//init
	for (int i = 0; i < row * col; ++i)
	{
		visited[i] = 0;
		distance[i] = INT_MAX;
		former[i] = -1;
	}
	visited[start] = 1;//初始化起始节点
	distance[start] = 0;
	former[start] = -1;

	std::queue<int> q;
	q.push(start);//把起始节点扔进队列

	//计算路径
	while (!q.empty())
	{
		bool over = false;

		int now = q.front();
		q.pop();

		//一大堆方便计算用的变量
		int x = now / col;//节点横坐标
		int y = now % col;//节点纵坐标
		int cc = (col - 1) + col;//cc是墙数组中一行元素的个数（https://blog.csdn.net/qq_39480548/article/details/107055411）<-这里有解释这个变量的意义
		//各个墙在墙数组里的坐标
		int upWall = (x - 1) * cc + y + (col - 1);//在墙数组中,先索引到upWall所在行，再加上【横墙】与【竖墙】的偏移，再加上正常的列偏移
		int downWall = upWall + cc;
		int leftWall = x * cc + y - 1;
		int rightWall = x * cc + y;//注意，这四扇墙可能会不是墙、或者横竖错误，下面要进行判断
		//各个节点在节点数组里的坐标
		int upNode = now - col;
		int downNode = now + col;
		int leftNode = now - 1;
		int rightNode = now + 1;////注意，这四个node可能会越界，或者不在它应该在的那一行，下面要进行判断

		std::vector<int> gogo;

		//判断当前节点（int now）能否上下左右移动（撞墙和走回头路都算不能移动），如果能，加入到gogo向量中，方便之后的进队操作和判断操作
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

		//把gogo中的节点放到队列中
		for (auto i = gogo.begin(); i < gogo.end(); ++i)
		{
			visited[*i] = 1;
			distance[*i] = distance[now] + 1;
			former[*i] = now;
			q.push(*i);
			if (*i == end)//判断是否结束
			{
				over = true;
				break;
			}
			//打印寻路过程功能是否开启
			if (proc)
			{
				printState(row, col, wall, start, end, distance);
				Sleep(200);
				system("cls");
			}
		}
		if (over) break;
	}
	//存储路径：从终点开始不断地找前驱，直到找到某个节点的前驱是-1 。如果无解，那么终点的前驱是-1，path中将只有终点一个点
	std::deque<int> path;
	int tempNode = end;
	do
	{
		path.push_front(tempNode);
		tempNode = former[tempNode];
	} while (tempNode != -1);

	return path;


}

std::deque<int> findWayDFS(int row, int col, int wall[], int start, int end, bool proc)
{
	int distance[MAX_ROW * MAX_COL] = { 0 };
	int former[MAX_ROW * MAX_COL] = { 0 };
	int visited[MAX_ROW * MAX_COL] = { 0 };

	//init
	for (int i = 0; i < row * col; ++i)
	{
		visited[i] = 0;
		distance[i] = INT_MAX;
		former[i] = -1;
	}
	visited[start] = 1;
	distance[start] = 0;
	former[start] = -1;//以上和BFS基本相同

	std::deque<int> path;

	bool allOver = false;

	DFS_VISIT(row, col, wall, start, end, distance, former, visited, start, allOver, proc);//这是一个递归函数

	//存储路径，和BFS相同
	int tempNode = end;
	do
	{
		path.push_front(tempNode);
		tempNode = former[tempNode];
	} while (tempNode != -1);

	return path;
}

void DFS_VISIT(int row, int col, int wall[], int start, int end, int distance[], int former[], int visited[], int now, bool& allOver, bool proc)
{
	if (allOver) return;//如果子函数找到了终点，allOver就会被置为true，已经入栈的父函数就会直接出栈，加快运行速度

	int wallNum = row * (col - 1) + col * (row - 1);

	//计算路径
	while (1)
	{
		//一些方便计算的数，和BFS基本一样
		int x = now / col;
		int y = now % col;
		int cc = (col - 1) + col;
		int upWall = (x - 1) * cc + y + (col - 1);
		int downWall = upWall + cc;
		int leftWall = x * cc + y - 1;
		int rightWall = x * cc + y;

		int upNode = now - col;
		int downNode = now + col;
		int leftNode = now - 1;
		int rightNode = now + 1;

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

		//DFS：递归的方式一条路走到底，如果当前这条分支无路可走了就结束当前函数，继续走最近的其他分支
		if (gogo.empty())
		{
			return;
		}

		for (auto i = gogo.begin(); i < gogo.end(); ++i)
		{
			visited[*i] = 1;
			distance[*i] = distance[now] + 1;
			former[*i] = now;
			if (*i == end)//如果找到终点就结束当前函数，并且做一下标记
			{
				allOver = true;
				return;
			}
			if (!allOver && proc)//在从起点迈出第一步时，如果有多个可走的地方，而找到终点时这些地方没走遍，就会先把这几个地方走了在进入终点。这是因为第一步会避开本函数开头的allOver判定，从而运行到printState这里。因此在printState处加入allOver判定，解决问题。
			{
				printState(row, col, wall, start, end, distance);
				Sleep(200);
				system("cls");
			}
			DFS_VISIT(row, col, wall, start, end, distance, former, visited, *i, allOver, proc);
		}
	}
}

//没啥好看的，也就是在纸上写写画画整了好几个小时qaq
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

//没啥好看的，也就是在纸上写写画画整了好几个小时qaq
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

//用了一手Prim算法加并查集，请参看以下两篇blog【https://www.jianshu.com/p/f643b0a0b887】【https://www.cnblogs.com/-new/p/6662301.html】
void createMaze(int& row, int& col, int wall[], int& start, int& end)
{
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

//一个用来打印寻路算法运行中状态的函数
//没啥好看的，也就是在纸上写写画画整了好几个小时qaq
void printState(int row, int col, int wall[], int start, int end, int distance[])
{
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

			//画已被扫描的点、起点、终点
			bool onScan = (i % 2 == 1 && j % 4 == 2) && distance[nodePos] < INT_MAX && start != nodePos && end != nodePos;//在寻找到的路径上放上“*”
			bool isStart = start == nodePos && (i % 2 == 1 && j % 4 == 2);//在起点处放“S”
			bool isEnd = end == nodePos && (i % 2 == 1 && j % 4 == 2);//在终点处放“E”



			//开画
			bool nonePrinted = true;
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
			//画起点终点、已被扫描的点
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
			if (onScan)
			{
				std::cout << distance[nodePos];
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
}