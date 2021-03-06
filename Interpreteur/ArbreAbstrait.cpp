#include <stdlib.h>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Type.h"
#include "Exceptions.h"
#include <typeinfo>
#include <iomanip>

////////////////////////////////////////////////////////////////////////////////
// NoeudSeqInst
////////////////////////////////////////////////////////////////////////////////

NoeudSeqInst::NoeudSeqInst() : m_instructions() {
}

Type * NoeudSeqInst::executer() {
  for (unsigned int i = 0; i < m_instructions.size(); i++)
    m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
  // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
  if (instruction!=nullptr) m_instructions.push_back(instruction);
}

void NoeudSeqInst::traduitEnPython(ostream& cout, unsigned int indentation) const {
    for(int i = 0; i < m_instructions.size(); i++){
        m_instructions[i]->traduitEnPython(cout,indentation);
        cout << endl;
    }
}

////////////////////////////////////////////////////////////////////////////////
// NoeudAffectation
////////////////////////////////////////////////////////////////////////////////

NoeudAffectation::NoeudAffectation(Noeud* variable, Noeud* expression)
: m_variable(variable), m_expression(expression) {
}

Type * NoeudAffectation::executer() {
  Type * valeur = m_expression->executer(); // On exécute (évalue) l'expression
  ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
  // La valeur renvoyée ne représente rien !
}

void NoeudAffectation::traduitEnPython(ostream& cout, unsigned int indentation) const {
    cout << string(indentation*3,' ');
    cout << dynamic_cast<Symbole*>(this->m_variable)->getChaine() << " = ";
    this->m_expression->traduitEnPython(cout,0);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudOperateurBinaire
////////////////////////////////////////////////////////////////////////////////

NoeudOperateurBinaire::NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit)
: m_operateur(operateur), m_operandeGauche(operandeGauche), m_operandeDroit(operandeDroit) {
}

Type * NoeudOperateurBinaire::executer() {
  Type * og, * od, * valeur;
  if (m_operandeGauche != nullptr) og = m_operandeGauche->executer(); // On évalue l'opérande gauche
  if (m_operandeDroit != nullptr) od = m_operandeDroit->executer(); // On évalue l'opérande droit
  // Et on combine les deux opérandes en fonctions de l'opérateur
  if (this->m_operateur == "+") valeur = &(*og + *od);
  else if (this->m_operateur == "-") valeur = &(*og - *od);
  else if (this->m_operateur == "*") valeur = &(*og * *od);
  else if (this->m_operateur == "==") valeur = (*og == *od) ? new Entier(0) : new Entier(1) ;
  else if (this->m_operateur == "!=") valeur = (*og != *od) ? new Entier(0) : new Entier(1) ;
  else if (this->m_operateur == "<") valeur = (*og < *od) ? new Entier(0) : new Entier(1) ;
  else if (this->m_operateur == ">") valeur = (*og > *od) ? new Entier(0) : new Entier(1) ;
  else if (this->m_operateur == "<=") valeur = (*og <= *od) ? new Entier(0) : new Entier(1) ;
  else if (this->m_operateur == ">=") valeur = (*og >= *od) ? new Entier(0) : new Entier(1) ;
  else if (this->m_operateur == "et") valeur = (*og && *od) ? new Entier(0) : new Entier(1) ;
  else if (this->m_operateur == "ou") valeur = (*og || *od) ? new Entier(0) : new Entier(1) ;
  else if (this->m_operateur == "non") valeur = (!*og) ? new Entier(0) : new Entier(1) ;
  else if (this->m_operateur == "/") {
    if (*od == *(new Entier(0))) throw DivParZeroException();
    valeur = &(*og / *od);
  }
  return valeur; // On retourne la valeur calculée
}

void NoeudOperateurBinaire::traduitEnPython(ostream& cout, unsigned int indentation) const {
    cout << string(indentation*3,' ');
    this->m_operandeGauche->traduitEnPython(cout,0);
    cout << " ";
    cout << this->m_operateur.getChaine();
    cout << " ";
    this->m_operandeDroit->traduitEnPython(cout,0);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(vector<Noeud*> condition, vector<Noeud*> sequence)
: m_condition(condition), m_sequence(sequence) {
}

Type * NoeudInstSi::executer() {
    int i = 0 ;
    while(i<m_condition.size() && i>=0){
        if (m_condition[i]->executer()){
            m_sequence[i]->executer();
            i = -1 ;
        }else{
            i++;
        }
    }
    if(i!=-1 && m_sequence.size()>m_condition.size())
        m_sequence[m_sequence.size()-1]->executer();
  // La valeur renvoyée ne représente rien !
}

void NoeudInstSi::traduitEnPython(ostream& cout, unsigned int indentation) const {
    cout << string(indentation*3,' ');
    cout << "if ";
    this->m_condition[0]->traduitEnPython(cout,0);
    cout << " :" << endl;
    this->m_sequence[0]->traduitEnPython(cout,indentation+1);
    
    for(int i = 1; i<this->m_condition.size() ; i++){
        cout << string(indentation*3,' ');
        cout << "elif ";
        this->m_condition[i]->traduitEnPython(cout,0);
        cout << " :" << endl;
        this->m_sequence[i]->traduitEnPython(cout,indentation+1);
    }
    
    if(m_sequence.size()!=m_condition.size()){
        cout << string(indentation*3,' ');
        cout << "else :" << endl;
        this->m_sequence[m_sequence.size()-1]->traduitEnPython(cout,indentation+1);
    }
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstTantQue
////////////////////////////////////////////////////////////////////////////////

NoeudInstTantQue::NoeudInstTantQue(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

Type * NoeudInstTantQue::executer() {
  while (m_condition->executer()) m_sequence->executer();
  // La valeur renvoyée ne représente rien !
}

void NoeudInstTantQue::traduitEnPython(ostream& cout, unsigned int indentation) const {

    cout << string(indentation*3,' ');
    cout << "while ";
    this->m_condition->traduitEnPython(cout,0);
    cout << " :" << endl;
    this->m_sequence->traduitEnPython(cout,indentation+1);

}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstRepeter
////////////////////////////////////////////////////////////////////////////////

NoeudInstRepeter::NoeudInstRepeter(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

Type * NoeudInstRepeter::executer() {
  do{
      m_sequence->executer(); 
  }while (!m_condition->executer());
  // La valeur renvoyée ne représente rien !
}

void NoeudInstRepeter::traduitEnPython(ostream& cout, unsigned int indentation) const {
    cout << string(indentation*3,' ') << "while not(";
    this->m_condition->traduitEnPython(cout,0);
    cout << "):" << endl;
    this->m_sequence->traduitEnPython(cout,indentation+1);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstPour
////////////////////////////////////////////////////////////////////////////////

NoeudInstPour::NoeudInstPour(Noeud* declaration, Noeud* condition, Noeud* incrementation, Noeud* sequence)
: m_declaration(declaration), m_condition(condition), m_incrementation(incrementation), m_sequence(sequence) {
}

Type * NoeudInstPour::executer() {
    if(m_declaration == nullptr && m_incrementation == nullptr){
        for(;m_condition->executer();) m_sequence->executer();
    }
    else if(m_declaration !=nullptr && m_incrementation ==nullptr){
        for(m_declaration->executer(); m_condition->executer();) m_sequence->executer();
    }
    else if(m_declaration ==nullptr && m_incrementation !=nullptr){
        for(;m_condition->executer();m_incrementation->executer()){
            m_sequence->executer();
        }
    }
    else for(m_declaration->executer(); m_condition->executer();m_incrementation->executer()){
            m_sequence->executer();
    }
    
}

void NoeudInstPour::traduitEnPython(ostream& cout, unsigned int indentation) const {
    cout << string(indentation*3,' ');
    if (this->m_declaration!=nullptr)
        this->m_declaration->traduitEnPython(cout,0);
    cout << endl << string(indentation*3,' ');
    cout << "while ";
    this->m_condition->traduitEnPython(cout,0);
    cout << " :" << endl;
    this->m_sequence->traduitEnPython(cout,indentation+1);
    if (this->m_incrementation!=nullptr)
        this->m_incrementation->traduitEnPython(cout,indentation + 1);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstEcrire
////////////////////////////////////////////////////////////////////////////////

NoeudInstEcrire::NoeudInstEcrire(vector<Noeud*> expressions)
: m_expressions(expressions) {
    
}

Type * NoeudInstEcrire::executer() {
    Noeud* it = nullptr;
    for (int i = 0 ; i < m_expressions.size() ; i++){
        it = m_expressions[i];
        if ( typeid(*it)==typeid(SymboleValue) && *((SymboleValue*)it)== "<CHAINE>"){
            SymboleValue* a = dynamic_cast<SymboleValue*>(it);
            string s = a->getChaine();
            cout << s.substr(1,s.size()-2);
        }else
            cout << it->executer();
    }
  // La valeur renvoyée ne représente rien !
}

void NoeudInstEcrire::traduitEnPython(ostream& cout, unsigned int indentation) const {
    cout << string(indentation*3,' ');
    cout << "print(";
    for (int i = 0 ; i < m_expressions.size() ; i++){
        m_expressions[i]->traduitEnPython(cout,0);
        if(i+1<m_expressions.size())
            cout<<" , ";
    }
    cout << ")";
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstLire
////////////////////////////////////////////////////////////////////////////////

NoeudInstLire::NoeudInstLire(vector<Noeud*> expressions) : m_expressions(expressions) {
}

Type * NoeudInstLire::executer() {
    for(int i=0; i < m_expressions.size(); i++){
        int a;
        cout << ((SymboleValue*)m_expressions[i])->getChaine() << " = ";
        cin >>  a;
        ((SymboleValue*) m_expressions[i])->setValeur(new Entier(a));
    }
}

void NoeudInstLire::traduitEnPython(ostream& cout, unsigned int indentation) const {
    for (int i = 0 ; i < m_expressions.size() ; i++){
        m_expressions[i]->traduitEnPython(cout,indentation);
        cout << string(indentation*3,' ');
        cout << " = input()";
    }
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstAbs
////////////////////////////////////////////////////////////////////////////////

NoeudInstAbs::NoeudInstAbs(Noeud* var) : m_variable(var){
}


Type * NoeudInstAbs::executer() {
    try{
        Entier* e = dynamic_cast<Entier*>(this->m_variable->executer());
        int a = e->getValue();
        if(a < 0)
            return new Entier(-a);
        else
            return new Entier(a);
    }catch(exception e){
        throw ExecutionException();
    }
}

void NoeudInstAbs::traduitEnPython(ostream& cout, unsigned int indentation) const {
    cout << string(indentation*3,' ') << "abs(" ;
    this->m_variable->traduitEnPython(cout,0);
    cout << ")" ;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstProc
////////////////////////////////////////////////////////////////////////////////

NoeudInstProc::NoeudInstProc(Procedure* proc, vector<Noeud*>* args) : m_proc(proc), m_args(args){

}


Type * NoeudInstProc::executer() {
    this->m_proc->execute(this->m_args);
}

void NoeudInstProc::traduitEnPython(ostream& cout, unsigned int indentation) const {
    cout << string(indentation*3,' ') << this->m_proc->getName() << "(" ;
    for (int i = 0 ; i < this->m_args->size() ; i ++){
        (*this->m_args)[i]->traduitEnPython(cout,0) ;
        if(i + 1 < this->m_args->size())
            cout << ", ";
    }
    
    cout << ")" ;
   
}
