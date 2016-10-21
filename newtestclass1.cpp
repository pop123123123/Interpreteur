/*
 * File:   newtestclass1.cpp
 * Author: louis
 *
 * Created on Oct 21, 2016, 1:39:31 AM
 */

#include "newtestclass1.h"
#include "Interpreteur/ArbreAbstrait.h"
#include "Interpreteur/Interpreteur.h"


CPPUNIT_TEST_SUITE_REGISTRATION(newtestclass1);

newtestclass1::newtestclass1() {
}

newtestclass1::~newtestclass1() {
}

void newtestclass1::setUp() {
}

void newtestclass1::tearDown() {
}

void newtestclass1::testExecuter() {
    ifstream fichier("testPour.txt");
    Interpreteur interpreteur(fichier);
    interpreteur.analyse();
    if (interpreteur.getArbre() != nullptr) interpreteur.getArbre()->executer();
    
    for (unsigned int i = 0; i < (*interpreteur.getTable()).getTaille(); i++)
        cout << (*interpreteur.getTable())[i].getChaine();
    cout << endl;
    
    
    if (true ) {
        CPPUNIT_ASSERT(false);
    }
    
    if (true /*check result*/) {
        CPPUNIT_ASSERT(false);
    }
    
    if (true /*check result*/) {
        CPPUNIT_ASSERT(false);
    }
    
}

