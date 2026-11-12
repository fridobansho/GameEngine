#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <cmath>

namespace TestHarness {
    struct TestFailure {
        std::string file;
        int line;
        std::string expression;
        std::string message;
    };

    struct TestCase {
        std::string name;
        std::function<void()> func;
        std::vector<TestFailure> failures;
    };

    inline std::vector<TestCase>& getTests() {
        static std::vector<TestCase> tests;
        return tests;
    }

    inline TestCase*& currentTest() {
        static TestCase* cur = nullptr;
        return cur;
    }

    inline int registerTest(const std::string& name, std::function<void()> func) {
        getTests().push_back({name, func, {}});
        return static_cast<int>(getTests().size());
    }

    inline void recordFailure(const char* file, int line, const char* expr, const std::string& msg = "") {
        if (currentTest()) {
            currentTest()->failures.push_back({file, line, expr, msg});
        }
    }

    inline bool& hasGLContext() {
        static bool has = false;
        return has;
    }

    template<typename T>
    inline std::string toStr(const T& val) {
        std::ostringstream oss;
        oss << val;
        return oss.str();
    }

    inline int runAllTests() {
        int passed = 0;
        int failed = 0;
        std::cout << "========================================================\n";
        std::cout << "             Running Automated Engine Tests             \n";
        std::cout << "========================================================\n";

        for (auto& test : getTests()) {
            currentTest() = &test;
            try {
                test.func();
            } catch (const std::exception& e) {
                test.failures.push_back({__FILE__, __LINE__, "exception", std::string("Unhandled exception: ") + e.what()});
            } catch (...) {
                test.failures.push_back({__FILE__, __LINE__, "exception", "Unhandled non-std exception"});
            }

            if (test.failures.empty()) {
                std::cout << "  [PASS] " << test.name << "\n";
                passed++;
            } else {
                std::cout << "  [FAIL] " << test.name << "\n";
                for (const auto& f : test.failures) {
                    std::cout << "         " << f.file << ":" << f.line << ": FAILED (" << f.expression << ")";
                    if (!f.message.empty()) {
                        std::cout << " -> " << f.message;
                    }
                    std::cout << "\n";
                }
                failed++;
            }
        }
        std::cout << "========================================================\n";
        std::cout << "Summary: " << passed << " passed, " << failed << " failed, "
                  << (passed + failed) << " total.\n";
        std::cout << "========================================================\n";
        return (failed == 0) ? 0 : 1;
    }

    inline int runSingleTest(const std::string& name) {
        for (auto& test : getTests()) {
            if (test.name == name) {
                currentTest() = &test;
                try {
                    test.func();
                } catch (const std::exception& e) {
                    test.failures.push_back({__FILE__, __LINE__, "exception", std::string("Unhandled exception: ") + e.what()});
                } catch (...) {
                    test.failures.push_back({__FILE__, __LINE__, "exception", "Unhandled non-std exception"});
                }

                if (test.failures.empty()) {
                    std::cout << "  [PASS] " << test.name << "\n";
                    return 0;
                } else {
                    std::cout << "  [FAIL] " << test.name << "\n";
                    for (const auto& f : test.failures) {
                        std::cout << "         " << f.file << ":" << f.line << ": FAILED (" << f.expression << ")";
                        if (!f.message.empty()) {
                            std::cout << " -> " << f.message;
                        }
                        std::cout << "\n";
                    }
                    return 1;
                }
            }
        }
        std::cerr << "Unknown test: " << name << "\n";
        return 1;
    }
}

#define TEST_CASE(name) \
    static void test_func_##name(); \
    static int test_reg_##name = TestHarness::registerTest(#name, test_func_##name); \
    static void test_func_##name()

#define CHECK(expr) \
    do { \
        if (!(expr)) { \
            TestHarness::recordFailure(__FILE__, __LINE__, #expr); \
        } \
    } while (0)

#define CHECK_EQ(a, b) \
    do { \
        if (!((a) == (b))) { \
            TestHarness::recordFailure(__FILE__, __LINE__, #a " == " #b, \
                std::string("expected: ") + TestHarness::toStr(b) + ", actual: " + TestHarness::toStr(a)); \
        } \
    } while (0)

#define CHECK_CLOSE(a, b, eps) \
    do { \
        if (std::abs((a) - (b)) > (eps)) { \
            TestHarness::recordFailure(__FILE__, __LINE__, "|" #a " - " #b "| <= " #eps, \
                std::string("diff: ") + std::to_string(std::abs((a) - (b)))); \
        } \
    } while (0)
