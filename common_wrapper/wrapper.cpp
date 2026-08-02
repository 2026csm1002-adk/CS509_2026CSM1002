#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
namespace fs = filesystem;

struct AssignmentEntry {
    string label;          // shown in the menu
    string driverSrc;      // path to the driver .cpp
    string extraSrc;       // path to the matching src/*.cpp implementation
    string exeName;        // output executable name
    string testsDir;       // where the test files live
    vector<string> extraArgs; // extra args appended after the test file (GEMM -> block size(in blocking method))
};

static vector<AssignmentEntry> buildCatalog() {
    return {
        {"Assignment 1 - GEMM (Simple + Blocking)",
         "assignment_01/driver/driver_gemm.cpp",
         "assignment_01/src/gemm.cpp",
         "assignment_01/driver/driver_gemm",
         "assignment_01/tests/gemm",
         {"32"}},
        {"Assignment 1 - CSR Graph Conversion",
         "assignment_01/driver/driver_csr.cpp",
         "assignment_01/src/csr.cpp",
         "assignment_01/driver/driver_csr",
         "assignment_01/tests/csr",
         {}},
    };
}

static string nativePath(const string& p) {
    return fs::path(p).make_preferred().string();
}

#ifdef _WIN32
static const string kExeSuffix = ".exe";
#else
static const string kExeSuffix = "";
#endif

static int runSystem(const string& cmd) {
#ifdef _WIN32
    string wrapped = "\"" + cmd + "\"";
    return system(wrapped.c_str());
#else
    return system(cmd.c_str());
#endif
}

static bool compileDriver(const AssignmentEntry& e) {
    ostringstream cmd;
    cmd << "g++ -O2 -std=c++17 -o \"" << nativePath(e.exeName + kExeSuffix) << "\" \""
        << nativePath(e.driverSrc) << "\" \"" << nativePath(e.extraSrc) << "\"";
    cout << "Compiling: " << cmd.str() << "endl";
    int rc = runSystem(cmd.str());
    return rc == 0;
}

static void runOnFile(const AssignmentEntry& e, const string& testFile) {
    ostringstream cmd;
    cmd << "\"" << nativePath(e.exeName + kExeSuffix) << "\" \"" << nativePath(testFile) << "\"";
    for (const auto& a : e.extraArgs) cmd << " " << a;
    // The CSR driver needs --weighted for weighted test files
    // File_format: <File_name>_weighted.txt
    // E.g.: csr_10_weighted.txt). 
    if (e.exeName.find("driver_csr") != string::npos &&
        testFile.find("_weighted.txt") != string::npos) {
        cmd << " --weighted";
    }
    cout << endl << "--- Running on " << testFile << " -------" << endl;
    int rc = runSystem(cmd.str());
    if (rc != 0) {
        cerr << "Warning: run returned non-zero exit code for " << testFile << endl;
    }
}

static void runAllTests(const AssignmentEntry& e) {
    if (!fs::exists(e.testsDir)) {
        cerr << "Error: tests directory not found: " << e.testsDir << endl;
        return;
    }
    bool anyRan = false;
    for (const auto& entry : fs::directory_iterator(e.testsDir)) {
        if (entry.path().extension() == ".txt") {
            runOnFile(e, entry.path().string());
            anyRan = true;
        }
    }
    if (!anyRan) {
        cerr << "No .txt test files found in " << e.testsDir << endl;
    }
}

int main() {
    auto catalog = buildCatalog();

    cout << "=== CS509 Common Wrapper ===\n";
    cout << "Available algorithms/assignments:\n";
    for (size_t i = 0; i < catalog.size(); ++i) {
        cout << "  " << (i + 1) << ") " << catalog[i].label << endl;
    }
    cout << "Select an option (number): ";

    int choice;
    if (!(cin >> choice) || choice < 1 || (size_t)choice > catalog.size()) {
        cerr << "Error: invalid selection." << endl;
        return 1;
    }
    const AssignmentEntry& selected = catalog[choice - 1];

    if (!fs::exists(selected.driverSrc) || !fs::exists(selected.extraSrc)) {
        cerr << "Error: required source file missing for: " << selected.label  << endl;
        return 1;
    }
    if (!compileDriver(selected)) {
        cerr << "Error: compilation failed for: " << selected.label << endl;
        return 1;
    }

    cout << "\nRun mode:\n";
    cout << "  1) Run a single test file\n";
    cout << "  2) Run all test files in " << selected.testsDir << endl;
    cout << "Select an option (number): ";

    int mode;
    if (!(cin >> mode)) {
        cerr << "Error: invalid selection." << endl;
        return 1;
    }

    if (mode == 1) {
        cout << "Enter path to test file: ";
        string path;
        cin >> path;
        if (!fs::exists(path)) {
            cerr << "Error: test file not found: " << path << endl;
            return 1;
        }
        runOnFile(selected, path);
    } else if (mode == 2) {
        runAllTests(selected);
    } else {
        cerr << "Error: invalid run mode." << endl;
        return 1;
    }

    return 0;
}
