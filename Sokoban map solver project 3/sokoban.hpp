#include <vector>
#include <string>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <fstream>
#include <algorithm>
#include <queue>
#include <bitset>

std::vector<char> directionLabels = {'U', 'D', 'L', 'R', 'N'};  // Corresponding labels for each direction


struct State {
    uint64_t data;

    // Constants for bit allocation
    static constexpr int PLAYER_BITS = 8;
    static constexpr int BOX_BITS = 8;
    static constexpr int MAX_BOXES = 7;
    
    static constexpr int PLAYER_OFFSET = 0;
    static constexpr int BOXES_OFFSET = PLAYER_OFFSET + PLAYER_BITS; // 16
    static constexpr int TOTAL_BOX_BITS = BOX_BITS * MAX_BOXES; // 48
    static constexpr int TOTAL_BITS = PLAYER_BITS + TOTAL_BOX_BITS; // 64

    // Masks for encoding and decoding
    static constexpr uint64_t PLAYER_MASK = (static_cast<uint64_t>(1) << PLAYER_BITS) - 1; // 0xFFFF
    static constexpr uint64_t BOX_MASK = (static_cast<uint64_t>(1) << BOX_BITS) - 1; // 0x3F

    // Default constructor
    State() : data(0) {}

    // Constructor with data
    explicit State(uint64_t input) : data(input) {}

    // Overload equality operator
    bool operator==(const State& other) const {
        return data == other.data;
    }

    /**
     * @brief Encodes the player position and boxes positions into the data field.
     * 
     * @param player_pos The player's position as an integer.
     * @param boxes_pos A vector of integers representing boxes' positions.
     * @throws std::invalid_argument if the number of boxes exceeds MAX_BOXES or positions are out of range.
     */
    void encode(uint8_t player_pos, std::vector<uint8_t>& boxes_pos) {
        std::sort(boxes_pos.begin(), boxes_pos.end());

        data = 0;
        data |= (static_cast<uint64_t>(player_pos) & PLAYER_MASK) << PLAYER_OFFSET;

        for (size_t i = 0; i < boxes_pos.size(); ++i) {
            data |= (static_cast<uint64_t>(boxes_pos[i]) & BOX_MASK) << (BOXES_OFFSET + i * BOX_BITS);
        }
    }

    /**
     * @brief Decodes the player position from the data field.
     * 
     * @return int The player's position.
     */
    uint8_t decodePlayerPosition() const {
        return static_cast<uint8_t>((data >> PLAYER_OFFSET) & PLAYER_MASK);
    }

    /**
     * @brief Decodes the boxes' positions from the data field.
     * 
     * @return std::vector<int> A vector of boxes' positions.
     */
    std::vector<uint8_t> decodeBoxesPositions() const {
        std::vector<uint8_t> boxes_pos;
        for (uint8_t i = 0; i < MAX_BOXES; ++i) {
            uint8_t pos = static_cast<uint8_t>((data >> (BOXES_OFFSET + i * BOX_BITS)) & BOX_MASK);
            if (pos != 0) { // Assuming 0 means no box; adjust if 0 is a valid position
                boxes_pos.push_back(pos);
            }
        }
        return boxes_pos;
    }

    /**
     * @brief Decodes the single box positions from the boxes field.
     * 
     * @return std::vector<int> A vector of boxes' positions.
     */
    uint8_t decodeSingelBox(int boxnumber) const {
        return static_cast<uint8_t>((data >> (BOXES_OFFSET + boxnumber * BOX_BITS)) & BOX_MASK); 
    }


};

// Specialize std::hash for State
namespace std {
    template <>
    struct hash<State> {
        std::size_t operator()(const State& s) const noexcept {
            return std::hash<uint64_t>()(s.data);
        }
    };
}

class Game {
public:
    std::vector<std::string> empty_map; //map without any boxes and player
    uint8_t map_height = 0;           // row of the map
    uint8_t map_width = 0;           // column of the map 
    uint8_t box_num = 0;            // number of box in the map
    uint8_t init_player_pos; // player initial position 
    std::vector<std::uint8_t> init_boxes_pos; // box position 
    std::unordered_set<std::uint8_t> target_pos; // box position
    State init_state;

    Game(std::vector<std::string> &grid, uint8_t num_boxes, uint8_t init_pos, uint8_t row, uint8_t col) {
        this->box_num = num_boxes;
        this->init_player_pos = init_pos;
        this->map_height = row;
        this->map_width = col;
        this->empty_map = grid;
        empty_map.resize((size_t)map_height);
        for (size_t i = 0; i < grid.size(); i ++) {
            empty_map[i].resize((size_t)map_width);
            for (size_t j = 0; j < grid[i].size(); j ++) {
                if (grid[i][j] == 'B') {
                    uint8_t box_pos = (uint8_t)( i * (size_t)map_width + j );
                    init_boxes_pos.push_back(box_pos);
                    empty_map[i][j] = '.';
                }
                else if (grid[i][j] == 'R') {
                    uint8_t box_pos = (uint8_t)( i * (size_t)map_width + j );
                    uint8_t target = (uint8_t)( i * (size_t)map_width + j );
                    init_boxes_pos.push_back(box_pos);
                    target_pos.insert(target);
                    empty_map[i][j] = 'T';
                }
                else if (grid[i][j] == 'T') {
                    uint8_t target = (uint8_t)( i * (size_t)map_width + j );
                    target_pos.insert(target);
                    empty_map[i][j] = 'T';
                }
                else if (grid[i][j] == '.') {
                    empty_map[i][j] = '.';
                }
                else if (grid[i][j] == '#') {
                    empty_map[i][j] = '#';
                }
                else if (grid[i][j] == 'S') {
                    empty_map[i][j] = '.';
                }
            }
        }

        // sort(init_boxes_pos.begin(), init_boxes_pos.end());
        init_state.encode(init_player_pos, init_boxes_pos);

    }

    // function to convert the points back to positin row and column
    std::pair<uint8_t, uint8_t> position(uint8_t& point);

    // fucntion to check if the state has all boxes at target position
    bool isGoal(const State& state);

    // function to check if the next move is possible and valid 
    bool validMove(uint8_t& player_pos, std::vector<uint8_t>& boxes_pos,const uint8_t& direction);

    // function to check if the current state is deadlock
    bool isDead(std::vector<uint8_t>& boxes_pos);

    // function to convert all the deadzone to wall
    void fill_deadzones(const std::vector<std::string>& grid);

    // function to find the shortest path to solve the sokoban game
    std::string BFS();

};

std::pair<uint8_t, uint8_t> Game::position(uint8_t& point) {

    uint8_t row = point / map_width;       
    uint8_t col = point % map_width;   

    return std::make_pair(row, col);
}

bool Game::isGoal(const State& state) {
    int count_box_at_target = 0;

    // Extract the box positions (bits 8 to 64) from the bitset
    for (uint8_t i = 0; i < box_num; i ++) {  // Each box is 8 bits long

        uint8_t box_bits = state.decodeSingelBox(i);

        if (box_bits == 0) continue;
        // Check if this box position is in the target positions
        else if (target_pos.find(box_bits) != target_pos.end()) {
            count_box_at_target++;
        }
    }

    if (count_box_at_target == box_num) {
        return true;
    }

    return false;
}

bool Game::validMove(uint8_t& player_pos, std::vector<uint8_t>& boxes_pos, const uint8_t& direction) {

    bool boxMove = false;
    uint8_t pushBox = 0; // 
    uint8_t Box_newpos = 0;

    for (auto& box: boxes_pos) {
        if (player_pos == box) {
            // player try to push box
            pushBox = box;
            switch(direction) {
                case 0: {
                    //up 
                    uint8_t up = pushBox - map_width;
                    if (empty_map[(size_t) (up / map_width)][(size_t) (up % map_width)] == 'D') return false; // position is deadlock
                    if (empty_map[(size_t) (up / map_width)][(size_t) (up % map_width)] != '#') {
                        Box_newpos = up;
                    }
                    else return false;
                    break;

                }
                case 1: {
                    //down
                    uint8_t down = pushBox + map_width;
                    if (empty_map[(size_t)position(down).first][(size_t)position(down).second] == 'D') return false; // position is deadlock
                    if (empty_map[(size_t)position(down).first][(size_t)position(down).second] != '#') {
                        Box_newpos = down;
                    }
                    else return false;
                    break;
                }
                case 2: {
                    //left
                    uint8_t left = pushBox - 1;
                    if (empty_map[(size_t)position(left).first][(size_t)position(left).second] == 'D') return false; // position is deadlock
                    if (empty_map[(size_t)position(left).first][(size_t)position(left).second] != '#') {
                        Box_newpos = left;
                    }
                    else return false;
                    break;
                }
                case 3: {
                    //right
                    uint8_t right = pushBox + 1;
                    if (empty_map[(size_t)position(right).first][(size_t)position(right).second] == 'D') return false; // position is deadlock
                    if (empty_map[(size_t)position(right).first][(size_t)position(right).second] != '#') {
                        Box_newpos = right; 
                    }
                    else return false;
                    break;
                }
            }
            boxMove = true; 
        }
    }
    
    if (boxMove) {
        for (auto& box: boxes_pos) {
            // return false if new box moved position already has box 
            if (box == Box_newpos) {
                return false;
            }
        }
        if (empty_map[(size_t)(Box_newpos / map_width)][(size_t)(Box_newpos % map_width)] == '.' || empty_map[(size_t)(Box_newpos / map_width)][(size_t)(Box_newpos % map_width)] == 'T') {
            for (auto& box: boxes_pos) {
                if (box == pushBox) {
                    box = Box_newpos; // pushed box move to the new position
                }
            }       
        }
        else {
            // new box moved position is not movable, wall
            return false;
        }

    }
    else {
        if (empty_map[(size_t)(player_pos / map_width)][(size_t)(player_pos % map_width)] == '#') {

            return false;
        }

    }
    
    return true; // state is valid
}

bool Game::isDead(std::vector<uint8_t>& boxes_pos) {
    
    bool possible_target = false; // has target along the wall
    bool box_found = false; // box that can move along wall is found
    for (auto& box: boxes_pos) {
        size_t i = (size_t)(box / map_width);
        size_t j = (size_t)(box % map_width);
        if (empty_map[i - 1][j] == '#' && empty_map[i][j - 1] == '#' && empty_map[i][j] != 'T') {
            // visited.insert(state);
            empty_map[i][j] = 'D'; // fill up the map with to indicate it is deadlock position
            return true; // deadlock detected
        }
        if (empty_map[i - 1][j] == '#' && empty_map[i][j + 1] == '#' && empty_map[i][j] != 'T') {
            // visited.insert(state);
            empty_map[i][j] = 'D'; // fill up the map with to indicate it is deadlock position
            return true; // deadlock detected
        }
        if (empty_map[i + 1][j] == '#' && empty_map[i][j - 1] == '#' && empty_map[i][j] != 'T') {
            // visited.insert(state);
            empty_map[i][j] = 'D'; // fill up the map with to indicate it is deadlock position
            return true; // deadlock detected
        }
        if (empty_map[i + 1][j] == '#' && empty_map[i][j + 1] == '#' && empty_map[i][j] != 'T') {
            // visited.insert(state);
            empty_map[i][j] = 'D'; // fill up the map with to indicate it is deadlock position
            return true; // deadlock detected
        }
        else if ((uint8_t)i == 1 || (uint8_t)i == map_height - 2) {
            box_found = true;
            for (size_t x = 0; x < (size_t)map_width; x ++) {
                if (empty_map[i][x] == 'T') possible_target = true;
            }
            if (box_found && !possible_target) {
                // visited.insert(state);
                empty_map[i][j] = 'D'; // fill up the map with to indicate it is deadlock position
                empty_map[i][j] = 'D'; // fill up the map with to indicate it is deadlock position
                return true;
            }
        }
        else if ((int)j == 1 || (int)j == map_width - 2) {
            box_found = true;
            for (size_t x = 0; x < (size_t)map_height; x ++) {
                if (empty_map[x][j] == 'T') possible_target = true;
            }
            if (box_found && !possible_target) {
                // visited.insert(state);
                empty_map[i][j] = 'D'; // fill up the map with to indicate it is deadlock position
                return true;
            }
        }

    }

    // deadlock not detected 
    return false;
}

std::string Game::BFS() { 
    // std::unordered_set<State> visited; // hash set to keep track all the visited state, deadlock state
    std::unordered_map<State, std::pair<State, std::bitset<2>>> parent; // hash map to backtrack

    std::queue<State> q;
    q.push(init_state);
    parent[init_state] = {init_state, 'S'};

    while (!q.empty()) {
        State currentState = q.front();
        q.pop();


        if (isGoal(currentState)) {
            std::string path = "";
            State backtrackState = currentState;

            while (!(backtrackState == init_state)) {
                path.push_back(directionLabels[(size_t)parent[backtrackState].second.to_ulong()]);
                backtrackState = parent[backtrackState].first;
            }
            reverse(path.begin(), path.end());

            // shortest path found
            return path;
        }

        for (uint8_t i = 0; i < 4; i ++) {
            // 1 = up, 2 = down, 3 = left, 4 = right
            State nextMove = currentState;
            uint8_t player_pos = nextMove.decodePlayerPosition(); // decode player position to int type 
            std::vector<uint8_t> boxes_pos = nextMove.decodeBoxesPositions(); // decode boxes position to vector<int> type

            // move player base on the direction 0 = up , 1 = down, 2 = left, 3 = right
            switch(i) {
                case 0: {
                    // up 
                    player_pos -= map_width;  // Move up
                    break;
                }
                case 1: {
                    // down
                    player_pos += map_width;  // Move down
                    break;
                }
                case 2: {
                    // left
                    player_pos -= 1;  // Move left
                    break;
                }
                case 3: {
                    // right
                    player_pos += 1;  // Move right
                    break;
                }
            }

            if (validMove(player_pos, boxes_pos, i) && !isDead(boxes_pos)) {

                // Convert player_pos into the first 8 bits of the bitset
                // encode and store back into state data
                nextMove.encode(player_pos, boxes_pos);

                if (parent.find(nextMove) != parent.end()) continue;

                q.push(nextMove);

                parent[nextMove] = {currentState, i};
            }
        
        }
    }
    // cout << "No solution!" << endl;
    return "No solution!";
}

/**
 * @brief  Read your map from stdin
 * @note   Input format: See project description
 * @param  &grid: You should first resize the grid! Modify it inplace.
 * @retval None
 */
void read_map(std::vector<std::string> &grid) {
    //read from input.txt
    std::ifstream infile("inputMap.txt");

    size_t cols, rows;
    infile >> cols >> rows;  // Read the row and column numbers

    // Clear the grid before using
    grid.clear();
    grid.resize(rows);
    
    // Read the map row by row and store in the grid
    std::string line;
    std::getline(infile, line);  // Skip the newline character after reading integers
    for (size_t i = 0; i < rows; ++i) {
        getline(infile, line);  // Read each row of the map
        grid[i] = line; 
    }

    infile.close();  // Close the file

}
/**
 * @brief  Solve the sokoban described by the grid
 * @note   Output string format: See project description
 * @param  &grid: The grid you read by `read_map'
 * @retval 
 */
std::string solve(std::vector<std::string> &grid) {
    // check if map is valid 

    uint8_t num_start = 0;
    uint8_t num_target_pos = 0;
    uint8_t num_boxes = 0;
    uint8_t init_pos = 0;

    for (size_t i = 0; i < grid.size(); i ++) {
        if (grid[i][0] != '#' || grid[i][grid[i].size() - 1] != '#') {
            return "No solution!";
        }
        for (size_t j = 0; j < grid[i].size(); j ++) {
            if (i == 0 || i == grid.size() - 1) {
                if (grid[i][j] != '#') {
                    return "No solution!";
                }
            } 
            else if (grid[i][j] == 'S') {
                num_start ++;
                init_pos = (uint8_t)(i * grid[i].size() + j);
            }
            else if (grid[i][j] == 'R') {
                num_boxes ++;
                num_target_pos ++;
            }
            else if (grid[i][j] == 'T') {
                num_target_pos ++;
            }
            else if (grid[i][j] == 'B') {
                num_boxes ++;
                if (grid[i - 1][j] == '#' && grid[i][j - 1] == '#') {
                    return "No solution!";
                }
                if (grid[i - 1][j] == '#' && grid[i][j + 1] == '#') {
                    return "No solution!";
                }
                if (grid[i + 1][j] == '#' && grid[i][j - 1] == '#') {
                    return "No solution!";
                }
                if (grid[i + 1][j] == '#' && grid[i][j + 1] == '#') {
                    return "No solution!";
                }
            }
        }
    }


    if (num_start != 1) {
        return "No solution!";
    }
    if (num_boxes > num_target_pos) {
        return "No solution!";
    }
    else if (num_boxes == 0) {
        return "";
    }

    Game game(grid, num_boxes, init_pos, (uint8_t)grid.size(), (uint8_t)grid[0].size());
    std::string result = game.BFS();

    return result;
}

// For big cases, paste your answers here (i.e. replace "ans for big 1" with your answer)
// Do not remove the first element, it should be left blank.
// NOTE: Ensure the order is correct!
// Your answer should look like "UUDDLLRR..."
const std::vector<std::string> answers = {
    "", 
    "UUUULLULLULDRDLLLLLLLLRURULLLLRDRDDLUDLLULDDDDLDRRRRRLDDDRRRDDDRRDRRDRRULLLRDDDLLUUURULLDLUUULURRR",
    "RUULDDLDDLLLUUUUURRDLDDRRDDRRULRULUULLULLDDRURRRULDRDDDDLLLUURRDRUUDDDLL",
    "DRURRLLUUULUURDRRRDDDRRDDLLUURRUURRUULLDDDDLLDDRRURULDDLLULLLUUULUURDRRRRLDDRRDDDLLULLLUUULURRRRDDRRDDDLLDLURRRUUULLDDUUUULLLDDDDRRDRUUURRDDDLRUUUURRUULLDLLLLRRRRDDLLUDDDLDDRUUUURRDLULDDLDDRURRURULULLDDLDRUUURRUULLDDDDLLLUUULUURDRRURDDDDULDDLLUUULURRRURDRRDDDLLULDURRRUULLDDRDL",
    "RRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRRLLLLDDRRRR",
    "RRUUUULURRRRRRRRRURDDDDRDLDLLURDRUUUURULLLDLUUULUURRDLLDDDLLLLLDDDDDLLLUURRDRUUULURRRRRRRRRURDDDDRDLDLLURDRUUUURULLLDLUUULULLULDRRRURRDLLDDDLLLLLDDDDRRRDLLRRDDLLLUUUUUULURRRRRRRRRURDDDDRDLDLLURDRUUUURULLLDLUUULULLDLURRRURRDLLDDDLLLLLDDDDDRRRRRULLLLDLUUUULURRRRRRRRRURDDDDRDLDLLURDRUUUURULLLDLUUULULLLLRRRRURRDLLLLLRUL",
    "URRUULULLRRDRDDLUUDDLDLLURUURUURRDLDDRDDLLRURUULUURDDLLLDDRDRRULUURDULLLDDRDRUURULDDDRULLLLDRRURUURUULLLDDDUUURRRDDLUDDDLDLLURUURURRDLDDRDLLLURDRUUURULDDDRUU",
    "LUURRDRUUULURRRRRRRRRURDDDDRDLDLLURDRUUUURULLLDLUUULUURRDLLDDDLLLLLDDDDDRRULDLUUUULURRRRRRRRRURDDDDRDLDLLURDRUUUURULLLDLUUULULLLLRRRRURRDLLDDDLLLLLDDDRDRRDRRULLLLDLUUUULURRRRRRRRRURDDDDRDLDLLURDRUUUURULLLDLUUULULLLRRRURRDLLDDDLLLLLDDDDDDDRRRUURULLLDLUUUULURRRRRRRRRURDDDDRDLDLLURDRUUUURULLLDLUUULULLDLURRRURRDLLLLULLRDRRRDDDLLLLLDDDLLLDDRRURRRRDDDLLLUUUUUULURRRRRRRRRURDDDDRDLDLLURDRUUUURULLLDLUUULUURRDLLLLDLURUL",
    "ULDDDRDLLLDLLURRRLLUURDUURURRDLDDLLUURURDLLDDRDLDLLULUURRLURRDLLLDDRDRRULLRRRURRDLLLDLLURURRDRRULDLLDLURUUURURRDLDDRDLLLUUURURDDDLDRLLDLLURRURRUULLDULLDRDUURRRDDLLDDLLURURRRUULLLLDLDLDRUURURRDLDRLDDRUULUURRRDDLRDL",
    "RDDLLLDDLLURDRRRDRUUULLLDLDRRLUURRDRDDLULLULLUUUUURRDDDDDLDRRRURDUUUULUURDDDDLDDLLULLUUUURURDDDDDLDRRLUURRDDLLULLUURLUURRDDDDLDRRR",
    "DDRUUUULDDDRDLLLRURULLLULUURDDLDDRUUURR",
    "",
    "",
    "LLLUUURRRUUUUURDRRDDDDLDLULUUURRURDDUUURDDDDDUURRDL",
    "",
    ""
};

// Don't modify this.
std::string print_answer(int index) {
    if (index < 1 || index >= (int)answers.size()) {
        return "invalid";
    }
    return answers[(size_t)index];
}
