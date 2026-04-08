#include "parser.h"
#include <sstream>
using namespace std;

Command parse_single(const string& input) {
    Command cmd;
    istringstream stream(input);
    string token;

    while (stream >> token) {
        if (token == ">") {
            stream >> cmd.output_file;
            cmd.append = false;
        }
        else if (token == ">>") {
            stream >> cmd.output_file;
            cmd.append = true;
        }
        else if (token == "<") {
            stream >> cmd.input_file;
        }
        else {
            cmd.args.push_back(token);
        }
    }
    return cmd;
}

vector<Command> parse(const string& input) {
    vector<Command> commands;
    string current;

    for (size_t i = 0; i <= input.size(); i++) {
        if (i == input.size() || input[i] == '|') {
            if (!current.empty()) {
                commands.push_back(parse_single(current));
                current.clear();
            }
        } else {
            current += input[i];
        }
    }
    return commands;
}
