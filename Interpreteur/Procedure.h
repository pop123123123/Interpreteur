#ifndef PROCEDURE_H
#define	PROCEDURE_H

#include "TableSymboles.h"
#include "SymboleValue.h"
#include "ArbreAbstrait.h"

using namespace std;


class Procedure {
public:
    Procedure(string & name, vector<SymboleValue*>* arguments, Noeud* sequinst, TableSymboles* symboles);
    Procedure(const Procedure& orig);
    virtual ~Procedure();
private:
    string name;
    vector<SymboleValue*>* m_args;
    Noeud* m_sequence;
    TableSymboles* m_symboles;
};

#endif	/* PROCEDURE_H */

