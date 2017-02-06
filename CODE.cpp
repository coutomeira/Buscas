#include <cstdio>
#include <iostream>
#include <queue>
#include <string>
#include <stack>
#include <stdlib.h>
#include <set>
#include <chrono>
#include <cmath>

static const int SIZE = 3;

struct Node {
	int matrix; 
	int zero_x;
	int zero_y;
	const Node* prev;
	int depth;
	int cost;
};

// matriz codificada em inteiro usando setMatrixEntry e getMatrixEntry

/*
0 1 2
3 4 5  ->  876543210
6 7 8
*/

int objective;
int objective_table[9][2];

int num_visited_nodes = 0;
int num_generated_nodes = 1;
int max_depth = 0;

std::chrono::time_point<std::chrono::system_clock> start;

std::vector<Node*> children;

static const int LOOKUP_TABLE[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000 };

// matrix[i][j]
int getMatrixEntry(int matrix, int i, int j)
{
	int index = i*SIZE + j;

	int divisor = LOOKUP_TABLE[index]; //10^index

	return (matrix / divisor) % 10;
}

// matrix[i][j] = k
void setMatrixEntry(int& matrix, int i, int j, int k)
{
	int index = i*SIZE + j;

	int multiplier = LOOKUP_TABLE[index]; //10^index

	matrix = (matrix - getMatrixEntry(matrix, i, j) * multiplier + k * multiplier);
}

void print(const Node* p)
{
	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
			std::cout << getMatrixEntry(p->matrix, i, j) << " ";

		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void print_solution(const Node* final_node)
{
	std::chrono::time_point<std::chrono::system_clock> end;

	end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed = end - start;

	int solution_depth = final_node->depth;

	std::stack<const Node*> movements;

	const Node* p = final_node;

	while(p != NULL)
	{
		movements.push(p);
		p = p->prev;
	}

	int c = 0;

	while(!movements.empty())
	{
		print(movements.top());

		std::cout << std::endl;

		movements.pop();

		c++;
	}

	std::cout << std::endl << "   " << "Movimentos: " << c - 1 << std::endl << std::endl;
	std::cout << "   " << "Segundos: " << elapsed.count() << std::endl << std::endl;
	std::cout << "   " << "Nos Visitados: " << num_visited_nodes << std::endl << std::endl;
	std::cout << "   " << "Nos Gerados: " << num_generated_nodes << std::endl << std::endl;
	std::cout << "   " << "Profundidade da Solucao: " << solution_depth << std::endl << std::endl;
	std::cout << "   " << "Profundidade Maxima: " << max_depth << std::endl << std::endl;

}

bool possible(int i, int j)
{
	return (i >= 0 && i < SIZE && j >= 0 && j < SIZE);
}

int calculate_cost(int matrix)
{
	int counter = 0;

	for(int i = 0; i < SIZE; i++)
	{
		for(int j = 0; j < SIZE; j++)
		{
			counter += std::abs(i - objective_table[getMatrixEntry(matrix, i, j)][0]) + std::abs(j - objective_table[getMatrixEntry(matrix, i, j)][1]);
		}
	}

	return counter;
}

void generate_children(const Node* node)
{
	int is[4] = { 0,0,1,-1 };
	int js[4] = { 1,-1,0,0 };

	children.clear();

	for (int a = 0; a < 4; a++)
	{
		if (possible(node->zero_x + is[a], node->zero_y + js[a]))
		{
			Node* child = (Node*)malloc(sizeof(Node));
			num_generated_nodes++;

			child->matrix = node->matrix;

			setMatrixEntry(child->matrix, node->zero_x, node->zero_y, getMatrixEntry(child->matrix, node->zero_x + is[a], node->zero_y + js[a]));
			setMatrixEntry(child->matrix, node->zero_x + is[a], node->zero_y + js[a], 0);

			child->zero_x = node->zero_x + is[a];
			child->zero_y = node->zero_y + js[a];
			child->prev = node;
			child->depth = node->depth + 1;

			if(child->depth > max_depth)
			{
				max_depth = child->depth;
			}

			children.push_back(child);
		}
	}
}


void bfs(Node* initial)
{
	std::queue<Node*> node_queue;
	node_queue.push(initial);

	while(!node_queue.empty())
	{
		Node* current = node_queue.front();
		num_visited_nodes++;

		if(current->matrix == objective)
		{
			print_solution(current);
			return;
		}
		else
		{
			node_queue.pop();

			generate_children(current);

			for(unsigned int i = 0; i < children.size(); i++)
			{
				const Node* parent = current->prev;

				while(parent != NULL)
				{
					if(parent->matrix == children[i]->matrix)
					{
						break;
					}

					parent = parent->prev;
				}

				if(parent == NULL)
					node_queue.push(children[i]);
			}
		}
	}
}

void dfs(Node* initial)
{
	std::stack<Node*> node_stack;
	node_stack.push(initial);

	while(!node_stack.empty())
	{
		Node* current = node_stack.top();
		num_visited_nodes++;

		if(current->matrix == objective)
		{
			print_solution(current);
			return;
		}
		else
		{
			node_stack.pop();

			generate_children(current);

			for(unsigned int i = 0; i < children.size(); i++)
			{
				const Node* parent = current->prev;

				while(parent != NULL)
				{
					if(parent->matrix == children[i]->matrix)
					{
						break;
					}

					parent = parent->prev;
				}

				if(parent == NULL)
					node_stack.push(children[i]);
			}
		}
	}
}

void iddfs(Node* initial)
{
	std::stack<Node*> node_stack;

	int limit = 0;
	while (true)
	{
		num_visited_nodes = 0;
		node_stack.push(initial);

		while (!node_stack.empty())
		{
			Node* current = node_stack.top();
			num_visited_nodes++;

			if (current->matrix == objective)
			{
				print_solution(current);
				return;
			}
			else
			{
				node_stack.pop();

				if(current->depth == limit)
					continue;

				generate_children(current);

				for(unsigned int i = 0; i < children.size(); i++)
				{
					const Node* parent = current->prev;

					while(parent != NULL)
					{
						if(parent->matrix == children[i]->matrix)
						{
							break;
						}

						parent = parent->prev;
					}

					if(parent == NULL)
						node_stack.push(children[i]);
				}
			}
		}

		limit++;
	}
}

auto cmp = [](Node* left, Node* right) { return (left->cost) > (right->cost); }; //funçao para inserir o node com menor custo no topo da pq

void greedy(Node* initial)
{
	initial->cost = calculate_cost(initial->matrix);

	std::priority_queue<Node*, std::vector<Node*>, decltype(cmp)> node_pq(cmp);
	node_pq.push(initial);

	while(!node_pq.empty())
	{
		Node* current = node_pq.top();
		num_visited_nodes++;

		if(current->matrix == objective)
		{
			print_solution(current);
			return;
		}
		else
		{
			node_pq.pop();

			generate_children(current);

			for(unsigned int i = 0; i < children.size(); i++)
			{
				const Node* parent = current->prev;

				while(parent != NULL)
				{
					if(parent->matrix == children[i]->matrix)
					{
						break;
					}

					parent = parent->prev;
				}

				if(parent == NULL)
				{
					children[i]->cost = calculate_cost(children[i]->matrix);
					node_pq.push(children[i]);
				}
			}
		}
	}
}

void astar(Node* initial)
{
	initial->cost = calculate_cost(initial->matrix);

	std::priority_queue<Node*, std::vector<Node*>, decltype(cmp)> node_pq(cmp);
	node_pq.push(initial);

	while(!node_pq.empty())
	{
		Node* current = node_pq.top();
		num_visited_nodes++;

		if(current->matrix == objective)
		{
			print_solution(current);
			return;
		}
		else
		{
			node_pq.pop();

			generate_children(current);

			for(unsigned int i = 0; i < children.size(); i++)
			{
				const Node* parent = current->prev;

				while(parent != NULL)
				{
					if(parent->matrix == children[i]->matrix)
					{
						break;
					}

					parent = parent->prev;
				}

				if(parent == NULL)
				{
					children[i]->cost = children[i]->depth + calculate_cost(children[i]->matrix);
					node_pq.push(children[i]);
				}
			}
		}
	}
}

int parity(int arr)
{
	int count = 0;

	for(int i = 0; i < SIZE*SIZE - 1; i++)
	{
		for(int j = i + 1; j < SIZE*SIZE; j++)
		{
			if(getMatrixEntry(arr, j / SIZE, j % SIZE) &&
				getMatrixEntry(arr, i / SIZE, i % SIZE) &&
				getMatrixEntry(arr, i / SIZE, i % SIZE) > getMatrixEntry(arr, j / SIZE, j % SIZE))
			{
				count++;
			}
		}
	}
		
	return count;
}

bool is_possible(int inicial, int objetivo)
{
	int countInicial = parity(inicial);
	int countObjetivo = parity(objetivo);

	return (countInicial % 2 == countObjetivo % 2);
}

int main()
{
	for(int i = 0; i < SIZE; i++)
	{
		for(int j = 0; j < SIZE; j++)
		{
			int k;
			scanf("%d", &k);

			objective_table[k][0] = i;			//guarda as coordenadas de cada numero do objetivo para depois utilizar na funcao calcular_custo
			objective_table[k][1] = j;

			setMatrixEntry(objective, i, j, k);
		}
	}

	Node initial;

	for(int i = 0; i < SIZE; i++)
	{
		for(int j = 0; j < SIZE; j++)
		{
			int k;
			scanf("%d", &k);

			if(k == 0)
			{
				initial.zero_x = i;
				initial.zero_y = j;
			}

			setMatrixEntry(initial.matrix, i, j, k);
		}	
	}

	initial.depth = 0;
	initial.prev = NULL;
	initial.cost = 0;

	std::cout << "Metodo de busca (0 - BFS, 1 - DFS, 2 - IDDFS, 3 - GREEDY, 4 - A*): ";

	int search;
	scanf("%d", &search);

	if(is_possible(initial.matrix, objective))
	{

		start = std::chrono::system_clock::now();
		if(search == 0)
		{
			bfs(&initial);
		}
		else if(search == 1)
		{
			dfs(&initial);
		}
		else if(search == 2)
		{
			iddfs(&initial);
		}
		else if(search == 3)
		{
			greedy(&initial);
		}
		else if(search == 4)
		{
			astar(&initial);
		}
		else
		{
			std::cout << "Metodo de busca invalido." << std::endl;
		}
	}
	else
		std::cout << "Nao ha soluçao!" << std::endl;

	return 0;
}
