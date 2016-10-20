#include "TableProcedures.h"
#include <stdlib.h>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"
#include <typeinfo>
#include <iomanip>

TableProcedures::TableProcedures() : m_procedures() {
}

TableProcedures::TableProcedures(const TableProcedures& orig) {
}

TableProcedures::~TableProcedures() {
}

void TableProcedures::traduitEnPython(ostream& cout, unsigned int indentation) {
    for (auto it=this->m_procedures.begin(); it!=this->m_procedures.end(); ++it){
        cout << string(indentation*3,' ') << "def " << it->first << "(" ;
        vector<SymboleValue*>* arguments = it->second->getArgs();
        for (int i = 0 ; i < arguments->size() ; i ++){
            cout << (*arguments)[i]->getChaine() ;
            if(i + 1 < arguments->size())
                cout << ", ";
        }
        cout << "):" << endl;
        it->second->getSequence()->traduitEnPython(cout,indentation + 1);
    }
}


void TableProcedures::addProcedure(string name, vector<SymboleValue*>* arguments, Noeud* sequinst, TableSymboles* symboles) {
    this->m_procedures[name] = new Procedure(name,arguments,sequinst,symboles);
}
