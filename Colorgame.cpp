#include<bits/stdc++.h>

const int NUMBER_OF_FLASK = 15;
const int BALLS_PER_FLASK = 4;

enum Color{
    YELLOW = 1,
    GREEN = 2,
    BLUE = 3,
    RED = 4,
    GREY = 5,
    ORANGE = 6,
    VIOLET = 7,
    LIME = 8,
    MAROON = 9,
    NAVY = 10,
    CYAN = 11,
    BLACK = 12,
    OLIVE = 13
};

std::map<Color, std::string> COLOR_TO_STRING{
    {YELLOW, "01"},
    {GREEN, "02"},
    {BLUE, "03"},
    {RED, "04"},
    {GREY, "05"},
    {ORANGE, "06"},
    {VIOLET, "07"},
    {LIME, "08"},
    {MAROON, "09"},
    {NAVY, "10"},
    {CYAN, "11"},
    {BLACK, "12"},
    {OLIVE, "13"}
};


struct Move{
    int start, end;
};

std::ostream& operator<<(std::ostream& os, const Move& move){
    std::cout << move.start << " -> " << move.end;
    return os;
}


using Flask = std::vector<Color>;

bool isFull(const Flask& flask){
    return flask.size() == BALLS_PER_FLASK;
}

bool allSameColor(const Flask& flask){
    if(flask.empty()){
        return true;
    }

    return std::adjacent_find(flask.begin(), flask.end(), std::not_equal_to<>() ) == flask.end();
}

bool canReceiveColor(const Flask& flask, Color color){
    return !isFull(flask) && flask.back() == color;
}

int numSameColorAtTop(const Flask& flask){
    int cnt = 0;
    for(auto it = flask.rbegin(); it < flask.rend(); it++){
        if(*it == *flask.rbegin()){
            cnt++;
        }
        else{
            break;
        }
    }

    return cnt;
}

bool sameColorExceptTop(const Flask& flask){
    if(flask.empty()){
        return true;
    }

    return std::adjacent_find(flask.begin(), flask.end()-1, std::not_equal_to<>() ) == flask.end()-1;
}


struct GameState{
    public:
    GameState(const std::array<Flask, NUMBER_OF_FLASK>& flasks) : flasks(flasks){}

    GameState newState(Move step) const{
        GameState ret(flasks);
        ret.flasks[step.end].push_back(flasks[step.start].back());
        ret.flasks[step.start].pop_back();
        return ret;
    }

    bool isValidMove(Move step) const{
        if(step.start == step.end || step.start < 0 || step.start >= NUMBER_OF_FLASK || step.end < 0 || step.start >= NUMBER_OF_FLASK){
            return false;
        }
        
        if(isFull(flasks[step.end])){
            return false;
        }

        if(flasks[step.end].empty() && flasks[step.start].size() == 1){
            return false;
        }

        if(flasks[step.start].empty()){
            return false;
        }

        if(flasks[step.end].empty()){
            return true;
        }

        return !isFull(flasks[step.end]) && flasks[step.end].back() == flasks[step.start].back();
    }

    bool isSolved() const{
        for(auto flask : flasks){
            if(flask.empty()){
                continue;
            }

            if(!isFull(flask) || !allSameColor(flask)){
                return false;
            }
        }

        return true;
    }

    int score(Move move) const{
        int reward = 0;

        if(flasks[move.end].empty()){
            if(allSameColor(flasks[move.start])){
                return -1000;
            }
            const Color startColor = flasks[move.start].back();
            int immediatelyExposed = 0;
            for(auto flask : flasks){
                if(flask.empty()){
                    continue;
                }

                if(flask.back() == startColor){
                    immediatelyExposed += numSameColorAtTop(flask);
                }
            }

            reward += immediatelyExposed * 20;
        }
        else if(allSameColor(flasks[move.end])){
            reward += flasks[move.end].size() * 100;
        }
        
        reward += sameColorExceptTop(flasks[move.start]) * 10 - flasks[move.start].size();

        return reward;
    }

    bool operator==(const GameState& rhs) const{
        for(int i = 0; i < flasks.size(); i++){
            if(flasks[i] != rhs.flasks[i]){
                return false;
            }
        }

        return true;
    }

    private:
    friend std::ostream& operator<<(std::ostream& os, const GameState& state);
    std::array<Flask, NUMBER_OF_FLASK> flasks;
};

std::ostream& operator<<(std::ostream& os, const GameState& state){
    for(int i = 0; i < NUMBER_OF_FLASK; i++){
        std::cout << i << " | ";
        for(auto color : state.flasks[i]){
            std::cout << COLOR_TO_STRING[color] << " ";
        }
        std::cout << std::endl;
    }

    return os;
}

bool isVisited(const GameState& state){
    static std::vector<GameState> visited;
    for(auto visitedState : visited){
        if(state == visitedState){
            return true;
        }
    }

    visited.push_back(state);
    return false;
}

bool dfs(const GameState& state, std::vector<Move>& moves){
    if(isVisited(state)){
        return false;
    }

    if(state.isSolved()){
        return true;
    }

    std::vector<Move> candidates;

    for(int i = 0; i < NUMBER_OF_FLASK; i++){
        for(int j = 0; j < NUMBER_OF_FLASK; j++){
            const Move move{i, j};
            if(state.isValidMove(move)){
                candidates.push_back(move);
            }
        }
    }

    sort(candidates.begin(), candidates.end(), [state](Move a, Move b){
        return state.score(a) > state.score(b);
    });

    for(auto move : candidates){
        if(dfs(state.newState(move), moves)){
            moves.insert(moves.begin(), move);
            return true;
        }
    }
    
    return false;
}

int main(){
    GameState state({
        Flask{},
        Flask{YELLOW, YELLOW, VIOLET, GREY},
        Flask{OLIVE, OLIVE, ORANGE, YELLOW},
        Flask{VIOLET, MAROON, GREEN, CYAN},
        Flask{MAROON, OLIVE, BLUE, NAVY},
        Flask{BLACK, OLIVE, YELLOW, ORANGE},
        Flask{ORANGE, ORANGE, NAVY, BLUE},
        Flask{LIME, RED},
        Flask{NAVY, NAVY, GREY, VIOLET},
        Flask{CYAN, GREEN, RED, LIME},
        Flask{BLUE, BLUE, GREY, BLACK},
        Flask{BLACK, VIOLET, GREEN, MAROON},
        Flask{GREY, BLACK, CYAN, GREEN},
        Flask{RED, LIME},
        Flask{MAROON, CYAN, RED, LIME}
    });

    std::vector<Move> sol = {};

    auto start = std::chrono::high_resolution_clock::now();
    dfs(state, sol);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    //std::cout << duration.count() << std::endl;

    // std::cout << state << std::endl;
    // for(int i = 0; i < sol.size(); i++){
    //     state = state.newState(sol[i]);
    //     std::cout << sol[i] << std::endl;
    //     std::cout << state << std::endl;;
    // }
    std::cout << sol.size() << std::endl;
}
