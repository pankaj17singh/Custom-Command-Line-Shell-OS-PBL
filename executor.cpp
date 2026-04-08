#include "executor.h"
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
using namespace std;

static int execute_single(const Command& cmd, int in_fd, int out_fd) {
    if (cmd.args.empty()) return 0;

    vector<char*> argv;
    for (const auto& arg : cmd.args)
        argv.push_back(const_cast<char*>(arg.c_str()));
    argv.push_back(nullptr);

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return -1; }

    if (pid == 0) {
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);

        if (in_fd != STDIN_FILENO) {
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }
        if (out_fd != STDOUT_FILENO) {
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }
        if (!cmd.input_file.empty()) {
            int fd = open(cmd.input_file.c_str(), O_RDONLY);
            if (fd < 0) { perror("open"); exit(1); }
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        if (!cmd.output_file.empty()) {
            int flags = O_WRONLY | O_CREAT | (cmd.append ? O_APPEND : O_TRUNC);
            int fd = open(cmd.output_file.c_str(), flags, 0644);
            if (fd < 0) { perror("open"); exit(1); }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        execvp(argv[0], argv.data());
        cerr << cmd.args[0] << ": command not found\n";
        exit(1);
    }

    return pid;
}

int execute(const vector<Command>& commands) {
    if (commands.empty()) return 0;

    if (commands.size() == 1) {
        int pid = execute_single(commands[0], STDIN_FILENO, STDOUT_FILENO);
        int status;
        waitpid(pid, &status, 0);
        int exit_code = WEXITSTATUS(status);
        if (exit_code != 0)
            cerr << "[exit status: " << exit_code << "]\n";
        return exit_code;
    }

    int num_commands = commands.size();
    int num_pipes = num_commands - 1;

    vector<array<int,2>> pipes(num_pipes);
    for (int i = 0; i < num_pipes; i++) {
        if (pipe(pipes[i].data()) < 0) {
            perror("pipe");
            return -1;
        }
    }

    vector<pid_t> pids;
    for (int i = 0; i < num_commands; i++) {
        int in_fd  = (i == 0) ? STDIN_FILENO  : pipes[i-1][0];
        int out_fd = (i == num_commands - 1) ? STDOUT_FILENO : pipes[i][1];
        pids.push_back(execute_single(commands[i], in_fd, out_fd));
    }

    for (int i = 0; i < num_pipes; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    int last_status = 0;
    for (pid_t pid : pids) {
        int status;
        waitpid(pid, &status, 0);
        last_status = WEXITSTATUS(status);
    }
    if (last_status != 0)
        cerr << "[exit status: " << last_status << "]\n";
    return last_status;
}
