#ifndef COMMSOCKETTEST_H
#define COMMSOCKETTEST_H

#include <cppunit/extensions/HelperMacros.h>

class CommSocketTest : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(CommSocketTest);

    CPPUNIT_TEST(GetSocketInfo);
    CPPUNIT_TEST(SocketSendAndReceiveTest);

    CPPUNIT_TEST_SUITE_END();

public:
    CommSocketTest();
    ~CommSocketTest() override;

    void setUp() override;
    void tearDown() override;

private:
  	// Tests getName and getHost functions ensuring that constructor is working as intended
    void GetSocketInfo();

	// Tests sending and receiving a string of characters through a socket
    void SocketSendAndReceiveTest();

};

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( CommSocketTest, CommSocketTest );

#endif /* COMMSOCKETTEST_H */
