#include "cmdconsole.h"

int main() {
    CmdConsole console;
    console.setHistoryFile("/tmp/console.hist");
    console.setPrompt("consoletest:> ");
    console.run();
}
