#include "executor.h"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

int execute(const vector<string>& args) {
    vector<char*> argv;
    for (const auto& arg : args)
        argv.push_back(const_cast<char*>(arg.c_str()));
    argv.push_back(nullptr); 

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return -1;
    }

    if (pid == 0) {
        execvp(argv[0], argv.data());
        cerr << args[0] << ": command not found\n";
        exit(1);
    }

    int status;
    waitpid(pid, &status, 0);
    return WEXITSTATUS(status);
}