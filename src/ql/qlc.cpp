
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "linenoise.hpp"
#include "queryc.hpp"
#include "graph_db.hpp"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

/**
 * Remove leading and trailing whitespaces from the given string.
 */
static void trim(std::string &s) {
  s.erase(s.begin(), std::find_if_not(s.begin(), s.end(),
                                      [](char c) { return std::isspace(c); }));
  s.erase(std::find_if_not(s.rbegin(), s.rend(),
                           [](char c) { return std::isspace(c); })
              .base(),
          s.end());
}

/**
 * Run an interactive shell for entering and executing queries.
 */
void run_shell(queryc& qlc) {
  const auto path = "history.txt";
  // Enable the multi-line mode
  linenoise::SetMultiLine(true);

  // Set max length of the history
  linenoise::SetHistoryMaxLen(4);
  // Load history
  linenoise::LoadHistory(path);

  while (true) {
    std::string line;
    auto quit = linenoise::Readline("poseidon> ", line);

    if (quit) {
      std::cout << "Bye!" << std::endl;
      break;
    }

    trim(line);
    if (line.length() == 0)
      continue;

    qlc.compile(line);

    // Add line to history
    linenoise::AddHistory(line.c_str());

    // Save history
    linenoise::SaveHistory(path);
  }

}

int main(int argc, char* argv[]) {
    spdlog::info("Starting poseidon_cli, Version {}", POSEIDON_VERSION);

    auto dct = p_make_ptr<dict>();
    queryc qlc(dct);
   
    qlc.compile("Project([$1.Id:int, $1.Age:int, $2.Name:string], Join($1.Id == $2.Id, NodeScan('Movie'), Filter($1.Age >= 42, NodeScan('Person'))))");

    run_shell(qlc);
}
 