#include<bits/stdc++.h>

const int NUMBER_OF_FLASK = 11;
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

bool isSameColor(Flask::const_iterator begin, Flask::const_iterator end){
    return std::adjacent_find(begin, end, std::not_equal_to<>() ) == end;
}

int numSameColorFromTop(const Flask& flask){
    int count = 0;
    for(auto it = flask.rbegin(); it < flask.rend() && *it == *flask.rbegin(); it++){
        count++;
    }

    return count;
}

class GameState{
    public:
    GameState(const std::array<Flask, NUMBER_OF_FLASK>& flasks) : flasks(flasks){}

    GameState newState(Move step) const{
        GameState ret(flasks);
        ret.flasks[step.end].emplace_back(flasks[step.start].back());
        ret.flasks[step.start].pop_back();
        return ret;
    }

    bool isValidMove(Move step) const{
        if(step.start == step.end || step.start < 0 || step.start >= NUMBER_OF_FLASK || step.end < 0 || step.start >= NUMBER_OF_FLASK){
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

            if(!isFull(flask) || !isSameColor(flask.begin(), flask.end())){
                return false;
            }
        }

        return true;
    }

    int numExposed(Color color) const{
        int num = 0;
        for(auto flask : flasks){                
            if(!flask.empty() && flask.back() == color){
                num += numSameColorFromTop(flask);
            }
        }

        return num;
    }

    int score(Move move) const{
        const Flask startFlask = flasks[move.start];
        const Flask endFlask = flasks[move.end];
        int reward = 0;

        if(endFlask.empty()){
            if(isSameColor(startFlask.begin(), startFlask.end())){
                return -1000;
            }

            reward += numExposed(startFlask.back()) * 20;
        }
        else if(isSameColor(endFlask.begin(), endFlask.end())){
            reward += endFlask.size() * 100;
        }
        
        reward += isSameColor(startFlask.begin(), std::prev(startFlask.end())) * 10 - startFlask.size();

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
        if(NUMBER_OF_FLASK >= 10 && i < 10){
            std::cout << i << "  | ";
        }
        else{
            std::cout << i << " | ";
        }
        
        for(auto color : state.flasks[i]){
            std::cout << COLOR_TO_STRING[color] << " ";
        }
        std::cout << std::endl;
    }

    return os;
}

bool hasVisited(const GameState& state){
    static std::vector<GameState> visited;
    for(auto visitedState : visited){
        if(state == visitedState){
            return true;
        }
    }

    visited.emplace_back(state);
    return false;
}

bool dfs(const GameState& state, std::vector<Move>& moves){
    if(hasVisited(state)){
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
                candidates.emplace_back(move);
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
        Flask{BLUE, VIOLET, LIME, MAROON},
        Flask{BLUE, BLUE, YELLOW, MAROON},
        Flask{ORANGE, YELLOW},
        Flask{},
        Flask{RED, ORANGE, LIME},
        Flask{RED, VIOLET, GREY, BLUE},
        Flask{GREY, GREEN, ORANGE, LIME},
        Flask{MAROON, MAROON, GREEN, ORANGE},
        Flask{YELLOW, YELLOW, LIME},
        Flask{VIOLET, VIOLET, RED, GREEN},
        Flask{GREEN, GREY, RED, GREY}
    });

    std::vector<Move> sol = {};

    dfs(state, sol);
    using namespace std;
    cout << state << endl;
    for(int i = 0; i < sol.size(); i++){
        state = state.newState(sol[i]);
        std::cout << sol[i] << std::endl;
        cout << state << endl;
    }
}