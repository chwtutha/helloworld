// Hunt the Wumpus - simple text version
// Minimal standalone implementation in a single file.

#include <bits/stdc++.h>
using namespace std;

static const vector<array<int,3>> ADJ = {
	{0,0,0}, // dummy for 0-index
	{2,5,8},
	{1,3,10},
	{2,4,12},
	{3,5,14},
	{1,4,6},
	{5,7,15},
	{6,8,17},
	{1,7,9},
	{8,10,18},
	{2,9,11},
	{10,12,19},
	{3,11,13},
	{12,14,20},
	{4,13,15},
	{6,14,16},
	{15,17,20},
	{7,16,18},
	{9,17,19},
	{11,18,20},
	{13,16,19}
};

struct Game {
	int player;
	int wumpus;
	vector<int> pits;
	vector<int> bats;
	int arrows = 5;
	mt19937 rng;

	Game() {
		rng.seed((unsigned)chrono::system_clock::now().time_since_epoch().count());
		reset();
	}

	int rand_room() { return uniform_int_distribution<int>(1,20)(rng); }

	void reset() {
		vector<int> pool(20);
		iota(pool.begin(), pool.end(), 1);
		shuffle(pool.begin(), pool.end(), rng);
		int idx = 0;
		player = pool[idx++];
		wumpus = pool[idx++];
		pits.clear(); bats.clear();
		pits.push_back(pool[idx++]);
		pits.push_back(pool[idx++]);
		bats.push_back(pool[idx++]);
		bats.push_back(pool[idx++]);
		arrows = 5;
	}

	bool adjacent(int a, int b) const {
		for (int x : ADJ[a]) if (x==b) return true;
		return false;
	}

	void describe_location() const {
		cout << "You are in room " << player << ".\n";
		cout << "Tunnels lead to rooms " << ADJ[player][0] << ", " << ADJ[player][1] << ", " << ADJ[player][2] << ".\n";
	}

	void smell_check() const {
		// Warnings for nearby hazards
		for (int r : ADJ[player]) {
			if (r==wumpus) cout << "You smell a terrible stench. (Wumpus nearby)\n";
			if (find(pits.begin(), pits.end(), r)!=pits.end()) cout << "You feel a cold wind. (Draft nearby)\n";
			if (find(bats.begin(), bats.end(), r)!=bats.end()) cout << "You hear faint wingbeats. (Bats nearby)\n";
		}
	}

	bool enter_room(int r) {
		player = r;
		if (player == wumpus) {
			cout << "You entered the Wumpus' room! It eats you.\n";
			return false; // player dead
		}
		if (find(pits.begin(), pits.end(), player) != pits.end()) {
			cout << "You fell into a bottomless pit! You die.\n";
			return false;
		}
		auto it = find(bats.begin(), bats.end(), player);
		if (it != bats.end()) {
			cout << "Giant bat snatches you and drops you in a random room...\n";
			int dest;
			do { dest = rand_room(); } while(dest==player);
			cout << "You are dropped in room " << dest << ".\n";
			return enter_room(dest);
		}
		return true;
	}

	bool shoot(int target) {
		if (!adjacent(player, target)) {
			cout << "You can only shoot into adjacent rooms.\n";
			return true; // game continues
		}
		arrows--;
		cout << "You shoot an arrow into room " << target << "...\n";
		if (target == wumpus) {
			cout << "You hear a terrible scream! You killed the Wumpus. You win!\n";
			return false; // game over (win)
		} else if (target == player) {
			cout << "The arrow circles and returns to you! You shot yourself.\n";
			cout << "You die.\n";
			return false;
		} else {
			cout << "Nothing. The arrow clatters away.\n";
			// Wumpus may move
			if (uniform_int_distribution<int>(1,4)(rng) == 1) {
				int neww;
				do { neww = rand_room(); } while(neww==wumpus);
				wumpus = neww;
				cout << "You hear a rumbling noise in the distance. The Wumpus has moved.\n";
			}
			if (arrows <= 0) {
				cout << "You have no arrows left. You are defenseless.\n";
				cout << "The Wumpus comes for you and eats you.\n";
				return false;
			}
			return true;
		}
	}

	void print_help() const {
		cout << "Commands:\n";
		cout << "  m <n> - move to room n\n";
		cout << "  s <n> - shoot an arrow into room n\n";
		cout << "  r     - restart game\n";
		cout << "  q     - quit\n";
		cout << "  h     - help\n";
	}
};

int main(){
	ios::sync_with_stdio(false);
	cin.tie(nullptr);

	Game g;
	cout << "Welcome to Hunt the Wumpus! (simple CLI version)\n";
	cout << "Type 'h' for help.\n\n";

	bool running = true;
	while (running) {
		g.describe_location();
		g.smell_check();
		cout << "Arrows left: " << g.arrows << "\n";
		cout << "> ";
		string cmd;
		if (!(cin >> cmd)) break;
		if (cmd == "m" || cmd == "M") {
			int room; cin >> room;
			if (room < 1 || room > 20) {
				cout << "Invalid room number.\n";
				continue;
			}
			if (!g.adjacent(g.player, room)) {
				cout << "You can't move there directly. Choose an adjacent room.\n";
				continue;
			}
			if (!g.enter_room(room)) {
				// player died or game ended
				cout << "Game over. Type 'r' to restart or 'q' to quit.\n";
				string a; while (cin >> a) {
					if (a=="r") { g.reset(); break; }
					if (a=="q") { running = false; break; }
					cout << "Type 'r' to restart or 'q' to quit.\n";
				}
			}
		} else if (cmd == "s" || cmd == "S") {
			int room; cin >> room;
			if (room < 1 || room > 20) {
				cout << "Invalid room number.\n";
				continue;
			}
			bool cont = g.shoot(room);
			if (!cont) {
				// game over (win or lose)
				cout << "Game over. Type 'r' to restart or 'q' to quit.\n";
				string a; while (cin >> a) {
					if (a=="r") { g.reset(); break; }
					if (a=="q") { running = false; break; }
					cout << "Type 'r' to restart or 'q' to quit.\n";
				}
			}
		} else if (cmd == "r") {
			g.reset();
			cout << "Game restarted.\n";
		} else if (cmd == "q") {
			cout << "Goodbye!\n";
			break;
		} else if (cmd == "h") {
			g.print_help();
		} else {
			cout << "Unknown command. Type 'h' for help.\n";
		}
		cout << "\n";
	}

	return 0;
}
