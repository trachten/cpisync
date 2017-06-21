/** This code is part of the CPISync project developed at Boston University:
 *      {@link http://nislab.bu.edu}
 * 
 * The code, explanation, references, API, and a demo can be found on this
 * web page.  The main theoretical basis for the approach can be found at
 * {@link http://ipsit.bu.edu/documents/ieee-it3-web.pdf}
 *   Y. Minsky, A. Trachtenberg, and R. Zippel,
 *   "Set Reconciliation with Nearly Optimal Communication Complexity",
 *   IEEE Transactions on Information Theory, 49:9.
 * 
 * Elements of the CPISync project code have been worked on, at various points, by:
 * @author Ari Trachtenberg
 * @author Sachin Agarwal 
 * @author Paul Varghese
 * @author Jiaxi Jin
 * @author Jie Meng
 * @author Alexander Smirnov

 */
/* 
 * File:   AuxiliaryTestRunner.cpp
 * Author: trachten
 * 
 * Created on Jun 21, 2017, 4:17:10 PM
 */

// CppUnit site http://sourceforge.net/projects/cppunit/files

#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

#include <cppunit/Test.h>
#include <cppunit/TestFailure.h>
#include <cppunit/portability/Stream.h>

class ProgressListener : public CPPUNIT_NS::TestListener {
public:

    ProgressListener()
    : m_lastTestFailed(false) {
    }

    ~ProgressListener() {
    }

    void startTest(CPPUNIT_NS::Test *test) {
        CPPUNIT_NS::stdCOut() << test->getName();
        CPPUNIT_NS::stdCOut() << "\n";
        CPPUNIT_NS::stdCOut().flush();

        m_lastTestFailed = false;
    }

    void addFailure(const CPPUNIT_NS::TestFailure &failure) {
        CPPUNIT_NS::stdCOut() << " : " << (failure.isError() ? "error" : "assertion");
        m_lastTestFailed = true;
    }

    void endTest(CPPUNIT_NS::Test *test) {
        if (!m_lastTestFailed)
            CPPUNIT_NS::stdCOut() << " : OK";
        CPPUNIT_NS::stdCOut() << "\n";
    }

private:
    /// Prevents the use of the copy constructor.
    ProgressListener(const ProgressListener &copy);

    /// Prevents the use of the copy operator.
    void operator=(const ProgressListener &copy);

private:
    bool m_lastTestFailed;
};

int main() {
    // Create the event manager and test controller
    CPPUNIT_NS::TestResult controller;

    // Add a listener that colllects test result
    CPPUNIT_NS::TestResultCollector result;
    controller.addListener(&result);

    // Add a listener that print dots as test run.
    ProgressListener progress;
    controller.addListener(&progress);

    // Add the top suite to the test runner
    CPPUNIT_NS::TestRunner runner;
    runner.addTest(CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest());
    runner.run(controller);

    // Print test in a compiler compatible format.
    CPPUNIT_NS::CompilerOutputter outputter(&result, CPPUNIT_NS::stdCOut());
    outputter.write();

    return result.wasSuccessful() ? 0 : 1;
}
