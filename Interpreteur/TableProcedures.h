#ifndef TABLEPROCEDURES_H
#define	TABLEPROCEDURES_H

#include "Procedure.h"
#include <map>
#include <vector>

using namespace std;

class TableProcedures {
public:
    TableProcedures();
    TableProcedures(const TableProcedures& orig);
    ~TableProcedures();
    void addProcedure(string name, vector<SymboleValue*>* arguments, Noeud* sequinst,TableSymboles* symboles);
    Procedure* getProcedure(const string & name){ return this->m_procedures[name]; }
    void traduitEnPython(ostream & cout,unsigned int indentation);
private:
    //vector<const Procedure*> m_procedures;
    map<string, Procedure*> m_procedures;
};

#endif	/* TABLEPROCEDURES_H */

