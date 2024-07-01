#include <UnitTest++.h>
#include "TestReporterStdout.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <nlohmann/json-schema.hpp>
#include "json.hpp"
using nlohmann::json_uri;
using nlohmann::json_schema::json_validator;
using json = nlohmann::json;

namespace UnitTest {
    class ListFilter {
        char **list;
        int n;
    public:
        ListFilter(char **list_, int count) {
            list = list_; n = count;
        }
        bool operator()(const Test* const t) const {
            for (int i=0;i<n;++i) {
                std::string dot_cpp_appended = std::string(list[i]) + ".cpp";
                if (!strcasecmp(t->m_details.testName, list[i]) ||
                        !strcasecmp(t->m_details.suiteName, list[i]) ||
                        !strcasecmp(t->m_details.filename, list[i]) ||
                        !strcasecmp(t->m_details.filename, dot_cpp_appended.c_str())) {
                    // erring on the side of matching more tests
                    return true;
                }
            }
            return false;
        }
    };

    int RunTheseTests(char ** list, int n) {
        TestReporterStdout reporter;
        TestRunner runner(reporter);
        return runner.RunTestsIf(Test::GetTestList(), NULL, ListFilter(list,n), 0);
    }
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        UnitTest::RunAllTests();
    } else {
        UnitTest::RunTheseTests(argv+1, argc-1); // skip the program's name itself.
    }
}