#pragma once
#include <vector>
#include <string>
using namespace std;

struct Command {
    vector<string> args;
    string input_file;
    string output_file;
    bool append = false;
};

Command parse_single(const string& input);
vector<Command> parse(const string& input);
