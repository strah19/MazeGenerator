#include "Core/Application.h"
#include "Core/RandomNumberGenerator.h"
#include <stack>
#include <thread>
#include <chrono>

class MazeGenerator : public Ember::Application {
public:
	void OnCreate() {
		srand((unsigned int)time(NULL));

		int start_x = rand() % maze_width;
		int start_y = rand() % maze_height;

		maze_stack.push({ start_x, start_y });
		visted++;
		maze = new int[maze_width * maze_height];
		for (int i = 0; i < maze_width * maze_height; i++)
			maze[i] = 0x00;
		maze[two_point_to_one(start_x, start_y)] = VISTED;
		start = { start_x, start_y };
	}

	virtual ~MazeGenerator() {
		delete[] maze;
	}

	void OnUserUpdate() {
		window->Update();

		if (visted < maze_width * maze_height) {
			Ember::IVec2 top = maze_stack.top();

			std::vector<int> neighbors = get_neighbors(top);

			if (!neighbors.empty()) {
				int next_cell = neighbors[rand() % neighbors.size()];

				switch (next_cell) {
				case 0: {
					maze[two_point_to_one(top.x, top.y - 1)] |= SOUTH | VISTED;
					maze[two_point_to_one(top.x, top.y)] |= NORTH;

					maze_stack.push({ top.x, top.y - 1 });
					break;
				}
				case 1: {
					maze[two_point_to_one(top.x, top.y + 1)] |= NORTH | VISTED;
					maze[two_point_to_one(top.x, top.y)] |= SOUTH;

					maze_stack.push({ top.x, top.y + 1 });
					break;
				}
				case 2: {
					maze[two_point_to_one(top.x - 1, top.y)] |= EAST | VISTED;
					maze[two_point_to_one(top.x, top.y)] |= WEST;

					maze_stack.push({ top.x - 1, top.y });
					break;
				}
				case 3: {
					maze[two_point_to_one(top.x + 1, top.y)] |= WEST | VISTED;
					maze[two_point_to_one(top.x, top.y)] |= EAST;

					maze_stack.push({ top.x + 1, top.y });
					break;
				}
				}

				visted++;
			}
			else
				maze_stack.pop();
		}

		renderer->Clear(background_color);

		for (int y = 0; y < maze_height; y++) {
			for (int x = 0; x < maze_width; x++) {
				if (maze[two_point_to_one(x, y)] & EAST) {
					renderer->Line({ x * cell_size, y * cell_size }, { x * cell_size + cell_size, y * cell_size }, { 255, 255, 255, 255 });
				}
				if (maze[two_point_to_one(x, y)] & SOUTH) {
					renderer->Line({ x * cell_size, y * cell_size }, { x * cell_size, y * cell_size + cell_size }, { 255, 255, 255, 255 });
				}

				if (maze[two_point_to_one(x, y)] & VISTED) {
					renderer->Rectangle({ x * cell_size + 1, y * cell_size + 1, cell_size - 1, cell_size - 1 }, background_color);
				}
				else
					renderer->Rectangle({ x * cell_size + 1, y * cell_size + 1, cell_size - 1, cell_size - 1 }, { 199, 101, 74, 255 });

			}
		}

		if (!maze_stack.empty()) {
			Ember::IVec2 top = maze_stack.top();
			renderer->Rectangle({ top.x * cell_size, top.y * cell_size, cell_size, cell_size }, { 112, 161, 80, 255 });
		}

		renderer->Rectangle({ start.x * cell_size, start.y * cell_size, cell_size, cell_size }, { 0, 0, 0, 255 });

		renderer->Show();
	}

	bool Keyboard(Ember::KeyboardEvents& keyboard) {
		if (keyboard.scancode == Ember::EmberKeyCode::Escape) {
			window->Quit();
			return true;
		}
		return false;
	}

	std::vector<int> get_neighbors(const Ember::IVec2& top) {
		std::vector<int> neighbors;
		if (top.y - 1 > -1 && (maze[two_point_to_one(top.x, top.y - 1)] & VISTED) == 0)
			neighbors.push_back(0);
		if (top.y + 1 < maze_height && (maze[two_point_to_one(top.x, top.y + 1)] & VISTED) == 0)
			neighbors.push_back(1);
		if (top.x - 1 > -1 && (maze[two_point_to_one(top.x - 1, top.y)] & VISTED) == 0)
			neighbors.push_back(2);
		if (top.x + 1 < maze_width && (maze[two_point_to_one(top.x + 1, top.y)] & VISTED) == 0)
			neighbors.push_back(3);

		return neighbors;
	}

	void UserDefEvent(Ember::Event& event) {
		Ember::EventDispatcher dispatch(&event);
		dispatch.Dispatch<Ember::KeyboardEvents>(EMBER_BIND_FUNC(Keyboard));
	}

	int two_point_to_one(int x, int y) {
		return (y * maze_width) + x;
	}
private:
	Ember::Color background_color = { 71, 131, 209, 255 };

	int maze_width = 32;
	int maze_height = 16;

	std::stack<Ember::IVec2> maze_stack;
	int visted = 0;
	int* maze = nullptr;

	Ember::IVec2 start;

	int cell_size = 32;

	enum {
		NORTH = 0x01,
		SOUTH = 0x02,
		EAST = 0x04,
		WEST = 0x08,
		VISTED = 0x10
	};
};

int main(int argc, char** argv) {
	MazeGenerator maze;
	maze.Initialize("Maze Generator");

	maze.Run();

	return 0;
}