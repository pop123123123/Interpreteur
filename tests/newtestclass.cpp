/*
 * File:   newtestclass.cpp
 * Author: louis
 *
 * Created on Oct 21, 2016, 1:38:32 AM
 */

#include "newtestclass.h"
#include "Interpreteur/ArbreAbstrait.h"


CPPUNIT_TEST_SUITE_REGISTRATION(newtestclass);

newtestclass::newtestclass() {
}

newtestclass::~newtestclass() {
}

void newtestclass::setUp() {
}

void newtestclass::tearDown() {
}

void newtestclass::testExecuter() {
    NoeudInstPour noeudInstPour;
    int result = noeudInstPour.executer();
    if (true /*check result*/) {
        CPPUNIT_ASSERT(false);
    }
}

