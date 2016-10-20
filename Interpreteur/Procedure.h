#ifndef PROCEDURE_H
#define	PROCEDURE_H

//#include "TableSymboles.h"
//#include "SymboleValue.h"
//#include "ArbreAbstrait.h"
#include <string>
#include <vector>


using namespace std;

class Noeud;
class SymboleValue;
class TableSymboles;


class Procedure {
public:
    Procedure(string & name, vector<SymboleValue*>* arguments, Noeud* sequinst, TableSymboles* symboles);
    virtual ~Procedure();
    int execute(vector<Noeud*>* arguments);
    string getName() { return name; }
    vector<SymboleValue*>* getArgs() { return this->m_args; }
    Noeud* getSequence() { return this->m_sequence; }
private:
    string name;
    vector<SymboleValue*>* m_args;
    Noeud* m_sequence;
    TableSymboles* m_symboles;
};

#endif	/* PROCEDURE_H */

