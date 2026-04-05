#include "builtins.h"
#include <iostream>
#include <unistd.h>
using namespace std;

bool is_builtin(const string& cmd) {
    return cmd == "cd" || cmd == "pwd" || 
           cmd == "exit" || cmd == "help";
}

int run_builtin(const vector<string>& args) {
    const string& cmd = args[0];

    if (cmd == "exit") {
        exit(0);
    }
    else if (cmd == "pwd") {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)))
            cout << cwd << "\n";
        else
            perror("pwd");
    }
    else if (cmd == "cd") {
        const char* path = (args.size() > 1) 
                            ? args[1].c_str() 
                            : getenv("HOME");
        if (chdir(path) != 0)
            perror("cd");
    }
    else if (cmd == "help") {
        cout << "Built-in commands: cd, pwd, exit, help\n";
    }
    return 0;
}
