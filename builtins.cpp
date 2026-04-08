#include "builtins.h"
#include <iostream>
#include <unistd.h>
using namespace std;

bool is_builtin(const string& cmd) {
    return cmd == "cd" || cmd == "pwd" ||
           cmd == "exit" || cmd == "help";
}

int run_builtin(const Command& cmd) {
    const string& command = cmd.args[0];

    if (command == "exit") {
        exit(0);
    }
    else if (command == "pwd") {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)))
            cout << cwd << "\n";
        else
            perror("pwd");
    }
    else if (command == "cd") {
        const char* path = (cmd.args.size() > 1)
                            ? cmd.args[1].c_str()
                            : getenv("HOME");
        if (chdir(path) != 0)
            perror("cd");
    }
    else if (command == "help") {
        cout << "\n";
        cout << "  Custom Shell\n";
        cout << "\n";
        cout << "  BUILT-IN COMMANDS:\n";
        cout << "    cd [path]     change directory\n";
        cout << "    pwd           print working directory\n";
        cout << "    help          show this help message\n";
        cout << "    exit          exit the shell\n";
        cout << "\n";
        cout << "  SUPPORTED FEATURES:\n";
        cout << "    cmd > file    redirect output to file\n";
        cout << "    cmd >> file   append output to file\n";
        cout << "    cmd < file    redirect input from file\n";
        cout << "    cmd | cmd     pipe output to next command\n";
        cout << "\n";
    }
    return 0;
}
