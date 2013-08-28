/****************************************************************************
 * Copyright (c) 2012, voidnull@github
 * License : BSD Clause 2 : http://bit.ly/bsd-clause-2
 ****************************************************************************/
#include <string.h>
#include <iostream>
#include "cmdconsole.h"
#include <algorithm>

using namespace std;

CmdConsole::CmdConsole() {
    registerCommand("help", &CmdConsole::cmdHelp);
    setPrompt("console:> ");
}

CmdConsole* CmdConsole::CompletionCB::console = NULL;

void CmdConsole::setPrompt(const std::string& prompt) {
    this->prompt = prompt;
}

void CmdConsole::setHistoryFile(const std::string& filename) {
    historyFile = filename;
}

void CmdConsole::setHistoryLength(size_t length) {
    linenoiseHistorySetMaxLen(length);
}

void CmdConsole::clearScreen() {
    linenoiseClearScreen();
}

void CmdConsole::complete(const std::string& line, Completions& completions) {
    std::map <std::string, CmdCallBack>::const_iterator cmdIter;
    for ( cmdIter = mapCallBacks.begin() ; cmdIter != mapCallBacks.end() ; ++ cmdIter) {
        if (0 == strncmp(line.c_str(),cmdIter->first.c_str(), line.length())){
            completions.add(cmdIter->first);
        }
    }
}

void CmdConsole::cmdUnknown(const std::string& line) {
    cout << "  " << Color::Red << "unable to process line: " << Color::End << line << "\n";
}

void CmdConsole::cmdHelp(std::vector<std::string>& args) {
    cout << Color::BoldMagenta << "Available Commands: "<< Color::End << "\n";
    std::map <std::string, CmdCallBack>::const_iterator cmdIter;
    for ( cmdIter = mapCallBacks.begin() ; cmdIter != mapCallBacks.end() ; ++cmdIter) {
        cout << "  " << cmdIter->first << "\n";
    }
    if (!args.empty()) {
        cout << "\n";
        cout << Color::BoldWhite << "Arguments:" << Color::End << "\n";
        for (uint i=0 ; i< args.size(); i++) {
            cout << "  " << args[i] << "\n";
        }
    }
}

void CmdConsole::registerCommand(const std::string& cmd, CmdCallBack cb) {
    std::string lowercmd=cmd;
    std::transform(lowercmd.begin(), lowercmd.end(), lowercmd.begin(), ::tolower);
    mapCallBacks[lowercmd] = cb;
}

void CmdConsole::run() {
    char *line,*next;
    CompletionCB::console = this;
    linenoiseSetCompletionCallback(CompletionCB::completion);

    if (!historyFile.empty()) {
        linenoiseHistoryLoad(historyFile.c_str());
    }

    std::string cmd;
    std::map <std::string, CmdCallBack>::const_iterator cmdIter;
    std::vector<std::string> args;
    if (!historyFile.empty()) {
        linenoiseHistoryLoad(historyFile.c_str());
    }
    while((line = linenoise(prompt.c_str())) != NULL) {
        if (line[0] != '\0') {
            next = line;
            while(next[0]!='\0' && !isspace(next[0])) next++;
                        
            cmd = std::string(line,next-line);
            // cout << line << ":" << cmd << "\n";
            args.clear();
            
            // find first non white space
            while(next[0]!='\0' && isspace(next[0])) next++;
            // cout << "next=" << next << "\n";

            std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

            cmdIter = mapCallBacks.find(cmd);
            if (cmdIter != mapCallBacks.end()) {
                // cout << "known command: " << cmdIter->first << "\n";
                if (next[0]!='\0') {
                    if (!splitArgs(next,args)) {
                        cout << Color::Yellow << "[WARN] error parsing line .. " << Color::End <<"\n";
                    }
                }
                (this->*(cmdIter->second))(args);
            } else {
                cmdUnknown(line);
            }

            linenoiseHistoryAdd(line);
            if (!historyFile.empty()) {
                if (-1 == linenoiseHistorySave(historyFile.c_str())) {
                    cout <<"error saving history\n";
                }
            }
            
        }
        free(line);
    }
}

/**
 * adapted from redis.sds.sdssplitargs
 * https://github.com/antirez/redis/blob/unstable/src/sds.c
 */
bool CmdConsole::splitArgs(const std::string& line, std::vector<std::string>& args) {
    const char *p = line.c_str();
    std::string current;
    while(true) {
        while(*p && isspace(*p)) p++;
        if (*p) {
            int inq=0;  /* set to 1 if we are in "quotes" */
            int insq=0; /* set to 1 if we are in 'single quotes' */
            int done=0;
            while(!done) {
                if (inq) {
                    if (*p == '\\' && *(p+1) == '"') { 
                        current.append(1,'"');
                        p++;
                    } else if (*p == '"') {
                        /* closing quote must be followed by a space or
                         * nothing at all. */
                        if (*(p+1) && !isspace(*(p+1))) return false;
                        done = 1;
                    } else if (!*p) {
                        /* unterminated quotes */
                        return false;
                    } else {
                        current = current.append(1,p[0]);
                    }                    
                } else if (insq) {
                    if (*p == '\\' && *(p+1) == '\'') {
                        p++;
                        current.append(1,'\'');
                    } else if (*p == '\'') {
                        /* closing quote must be followed by a space or
                         * nothing at all. */
                        if (*(p+1) && !isspace(*(p+1))) return false;
                        done=1;
                    } else if (!*p) {
                        /* unterminated quotes */
                        return false;
                    } else {
                        current = current.append(1,p[0]);
                    }
                } else {
                    switch(*p) {
                        case ' ':
                        case '\n':
                        case '\r':
                        case '\t':
                        case '\0':
                            done=1;
                            break;
                        case '"':
                            inq=1;
                            break;
                        case '\'':
                            insq=1;
                            break;
                        default:
                            current.append(1,p[0]);
                            // cout << "append:" << p[0] << ":" << current <<"\n";
                            break;
                    }
                }
                if (*p) p++;
            } // while (!done)
            args.push_back(current);
            current.clear();
        } else { // if p
            return true;
        }
    } // while true
    return false;
}


std::string Color::End = "\x1b[0m";

std::string Color::Black = "\x1b[30m";
std::string Color::Red= "\x1b[31m";
std::string Color::Green = "\x1b[32m";
std::string Color::Yellow = "\x1b[33m";
std::string Color::Blue =  "\x1b[34m";
std::string Color::Magenta = "\x1b[35m";
std::string Color::Cyan =  "\x1b[36m";
std::string Color::White = "\x1b[37m";

std::string Color::BoldBlack = "\x1b[1;30m";
std::string Color::BoldRed= "\x1b[1;31m";
std::string Color::BoldGreen = "\x1b[1;32m";
std::string Color::BoldYellow = "\x1b[1;33m";
std::string Color::BoldBlue =  "\x1b[1;34m";
std::string Color::BoldMagenta = "\x1b[1;35m";
std::string Color::BoldCyan =  "\x1b[1;36m";
std::string Color::BoldWhite = "\x1b[1;37m";
