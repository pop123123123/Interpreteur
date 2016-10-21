/*
 * File:   newtestclass1.h
 * Author: louis
 *
 * Created on Oct 21, 2016, 1:39:31 AM
 */

#ifndef NEWTESTCLASS1_H
#define	NEWTESTCLASS1_H

#include <cppunit/extensions/HelperMacros.h>

class newtestclass1 : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(newtestclass1);

    CPPUNIT_TEST(testExecuter);

    CPPUNIT_TEST_SUITE_END();

public:
    newtestclass1();
    virtual ~newtestclass1();
    void setUp();
    void tearDown();

private:
    void testExecuter();

};

#endif	/* NEWTESTCLASS1_H */

