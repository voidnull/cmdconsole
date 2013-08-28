/****************************************************************************
 * Copyright (c) 2012, voidnull@github
 * License : BSD Clause 2 : http://bit.ly/bsd-clause-2
 ****************************************************************************/
#ifndef __CPP__CMDCONSOLE__H__
#define __CPP__CMDCONSOLE__H__

#include <string>
#include <map>
#include <vector>

#include "linenoise.h"

class CmdConsole {
public:

    struct Completions {
        linenoiseCompletions *lc;
        void add(const std::string& completion) {
            linenoiseAddCompletion(lc,completion.c_str());
        }
    };

    struct CompletionCB {
        static CmdConsole* console;
        static void completion(const char *buf, linenoiseCompletions *lc) {
            Completions completions;
            completions.lc = lc;
            console->complete(std::string(buf),completions);
        }
    };

    typedef void (CmdConsole::*CmdCallBack)(std::vector<std::string>&) ;
    CmdConsole();
    void setPrompt(const std::string& prompt);
    void setHistoryFile(const std::string& filename);
    void setHistoryLength(size_t length);
    void clearScreen();

    virtual void complete(const std::string& line, Completions& completions);
    virtual void cmdUnknown(const std::string& line);
    virtual void cmdHelp(std::vector<std::string>& args);

    bool splitArgs(const std::string& line, std::vector<std::string>& args);

    void registerCommand(const std::string& cmd, CmdCallBack cb);
    void run();

protected:
    std::string prompt;
    std::string historyFile;

    std::map <std::string, CmdCallBack> mapCallBacks;

};

struct Color {
    static std::string End;

    static std::string Black;
    static std::string Red;
    static std::string Green;
    static std::string Yellow;
    static std::string Blue;
    static std::string Magenta;
    static std::string Cyan;
    static std::string White;

    static std::string BoldBlack;
    static std::string BoldRed;
    static std::string BoldGreen;
    static std::string BoldYellow;
    static std::string BoldBlue;
    static std::string BoldMagenta;
    static std::string BoldCyan;
    static std::string BoldWhite;
};


#endif // __CPP__CMDCONSOLE__H__
