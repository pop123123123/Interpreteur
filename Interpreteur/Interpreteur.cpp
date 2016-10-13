#include "Interpreteur.h"
#include <stdlib.h>
#include <iostream>
#include <vector>
using namespace std;

Interpreteur::Interpreteur(ifstream & fichier) :
m_lecteur(fichier), m_table(), m_arbre(nullptr) {
}

void Interpreteur::analyse() {
  m_arbre = programme(); // on lance l'analyse de la première règle
}

void Interpreteur::tester(const string & symboleAttendu) const throw (SyntaxeException) {
  // Teste si le symbole courant est égal au symboleAttendu... Si non, lève une exception
  static char messageWhat[256];
  if (m_lecteur.getSymbole() != symboleAttendu) {
    sprintf(messageWhat,
            "Ligne %d, Colonne %d - Erreur de syntaxe - Symbole attendu : %s - Symbole trouvé : %s",
            m_lecteur.getLigne(), m_lecteur.getColonne(),
            symboleAttendu.c_str(), m_lecteur.getSymbole().getChaine().c_str());
    throw SyntaxeException(messageWhat);
  }
}

void Interpreteur::testerEtAvancer(const string & symboleAttendu) throw (SyntaxeException) {
  // Teste si le symbole courant est égal au symboleAttendu... Si oui, avance, Sinon, lève une exception
  tester(symboleAttendu);
  m_lecteur.avancer();
}

void Interpreteur::erreur(const string & message) const throw (SyntaxeException) {
  // Lève une exception contenant le message et le symbole courant trouvé
  // Utilisé lorsqu'il y a plusieurs symboles attendus possibles...
  static char messageWhat[256];
  sprintf(messageWhat,
          "Ligne %d, Colonne %d - Erreur de syntaxe - %s - Symbole trouvé : %s",
          m_lecteur.getLigne(), m_lecteur.getColonne(), message.c_str(), m_lecteur.getSymbole().getChaine().c_str());
  throw SyntaxeException(messageWhat);
}

Noeud* Interpreteur::programme() {
  // <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
  testerEtAvancer("procedure");
  testerEtAvancer("principale");
  testerEtAvancer("(");
  testerEtAvancer(")");
  Noeud* sequence = seqInst();
  testerEtAvancer("finproc");
  tester("<FINDEFICHIER>");
  return sequence;
}

Noeud* Interpreteur::seqInst() {
  // <seqInst> ::= <inst> { <inst> }
  NoeudSeqInst* sequence = new NoeudSeqInst();
  do {
    sequence->ajoute(inst());
  } while (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "si" || m_lecteur.getSymbole() == "tantque" || m_lecteur.getSymbole() == "repeter" || m_lecteur.getSymbole() == "ecrire");
  // Tant que le symbole courant est un début possible d'instruction...
  // Il faut compléter cette condition chaque fois qu'on rajoute une nouvelle instruction
  return sequence;
}

Noeud* Interpreteur::inst() {
  // <inst> ::= <affectation>  ; | <instSi>
  if (m_lecteur.getSymbole() == "<VARIABLE>") {
    Noeud *affect = affectation();
    testerEtAvancer(";");
    return affect;
  }
  else if (m_lecteur.getSymbole() == "si")
    return instSi();
  else if (m_lecteur.getSymbole() == "tantque")
    return instTantQue();
  else if (m_lecteur.getSymbole() == "repeter")
    return instRepeter();
  else if (m_lecteur.getSymbole() == "ecrire"){//cout << "mabite";
    return instEcrire();}
  // Compléter les alternatives chaque fois qu'on rajoute une nouvelle instruction
  else erreur("Instruction incorrecte");
}

Noeud* Interpreteur::affectation() {
  // <affectation> ::= <variable> = <expression> 
  tester("<VARIABLE>");
  Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole()); // La variable est ajoutée à la table eton la mémorise
  m_lecteur.avancer();
  testerEtAvancer("=");
  Noeud* exp = expression();             // On mémorise l'expression trouvée
  return new NoeudAffectation(var, exp); // On renvoie un noeud affectation
}

Noeud* Interpreteur::expression() {
  // <expression> ::= <facteur> { <opBinaire> <facteur> }
  //  <opBinaire> ::= + | - | *  | / | < | > | <= | >= | == | != | et | ou
  Noeud* fact = facteur();
  while ( m_lecteur.getSymbole() == "+"  || m_lecteur.getSymbole() == "-"  ||
          m_lecteur.getSymbole() == "*"  || m_lecteur.getSymbole() == "/"  ||
          m_lecteur.getSymbole() == "<"  || m_lecteur.getSymbole() == "<=" ||
          m_lecteur.getSymbole() == ">"  || m_lecteur.getSymbole() == ">=" ||
          m_lecteur.getSymbole() == "==" || m_lecteur.getSymbole() == "!=" ||
          m_lecteur.getSymbole() == "et" || m_lecteur.getSymbole() == "ou"   ) {
    Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
    m_lecteur.avancer();
    Noeud* factDroit = facteur(); // On mémorise l'opérande droit
    fact = new NoeudOperateurBinaire(operateur, fact, factDroit); // Et on construuit un noeud opérateur binaire
  }
  return fact; // On renvoie fact qui pointe sur la racine de l'expression
}

Noeud* Interpreteur::facteur() {
  // <facteur> ::= <entier> | <variable> | - <facteur> | non <facteur> | ( <expression> )
  Noeud* fact = nullptr;
  if (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "<ENTIER>") {
    fact = m_table.chercheAjoute(m_lecteur.getSymbole()); // on ajoute la variable ou l'entier à la table
    m_lecteur.avancer();
  } else if (m_lecteur.getSymbole() == "-") { // - <facteur>
    m_lecteur.avancer();
    // on représente le moins unaire (- facteur) par une soustraction binaire (0 - facteur)
    fact = new NoeudOperateurBinaire(Symbole("-"), m_table.chercheAjoute(Symbole("0")), facteur());
  } else if (m_lecteur.getSymbole() == "non") { // non <facteur>
    m_lecteur.avancer();
    // on représente le moins unaire (- facteur) par une soustractin binaire (0 - facteur)
    fact = new NoeudOperateurBinaire(Symbole("non"), facteur(), nullptr);
  } else if (m_lecteur.getSymbole() == "(") { // expression parenthésée
    m_lecteur.avancer();
    fact = expression();
    testerEtAvancer(")");
  } else
    erreur("Facteur incorrect");
  return fact;
}

Noeud* Interpreteur::chaine() {
  tester("<CHAINE>");
  Symbole a = m_lecteur.getSymbole();
  return m_table.chercheAjoute(a); // on ajoute la chaine
}


Noeud* Interpreteur::instSi() {
  // <instSi> ::= si ( <expression> ) <seqInst> finsi
    vector<Noeud*> *conditions = new vector<Noeud*>();
    vector<Noeud*> *sequences = new vector<Noeud*>();
    try{
        testerEtAvancer("si");
        testerEtAvancer("(");
        Noeud* condition = expression(); // On mémorise la condition
        conditions->push_back(condition);
        testerEtAvancer(")");
        Noeud* sequence = seqInst();     // On mémorise la séquence d'instruction
        sequences->push_back(sequence);
        bool elseif = true;
        while(elseif){
            try{
                testerEtAvancer("sinonsi");
                testerEtAvancer("(");
                Noeud* condition = expression(); // On mémorise la condition
                conditions->push_back(condition);
                testerEtAvancer(")");
                Noeud* sequence = seqInst();     // On mémorise la séquence d'instruction
                sequences->push_back(sequence);
            }catch(SyntaxeException e){
                elseif = false;
            }
        }
        try{
            testerEtAvancer("sinon");
            Noeud* sequence = seqInst();     // On mémorise la séquence d'instruction
            sequences->push_back(sequence);
        }catch(SyntaxeException e){
            elseif = false;
        }
        testerEtAvancer("finsi");
        return new NoeudInstSi(*conditions, *sequences); // Et on renvoie un noeud Instruction Si
    }catch(SyntaxeException e){
        delete conditions;
        delete sequences;
        throw e;
    }
}

Noeud* Interpreteur::instTantQue() {
  // <instTantQue> ::= tantque ( <expression> ) <seqInst> fintantque
  testerEtAvancer("tantque");
  testerEtAvancer("(");
  Noeud* condition = expression(); // On mémorise la condition
  testerEtAvancer(")");
  Noeud* sequence = seqInst();     // On mémorise la séquence d'instruction
  testerEtAvancer("fintantque");
  return new NoeudInstTantQue(condition, sequence); // Et on renvoie un noeud Instruction TanqtQue
}

Noeud* Interpreteur::instRepeter() {
  // <instRepeter> ::= repeter <seqInst> jusqua ( <expression> )
  testerEtAvancer("repeter");
  Noeud* sequence = seqInst();     // On mémorise la séquence d'instruction
  testerEtAvancer("jusqua");
  testerEtAvancer("(");
  Noeud* condition = expression(); // On mémorise la condition
  testerEtAvancer(")");
  return new NoeudInstRepeter(condition, sequence); // Et on renvoie un noeud Instruction instRepeter
}

Noeud* Interpreteur::instPour() {
    //<instPour> ::= pour ( [<affectation>]; <expression>; [<affectation>] ) <seqInst> finpour
    testerEtAvancer("pour");
    testerEtAvancer("(");

    Noeud* condition = expression();

    testerEtAvancer(")");
    Noeud* sequence = seqInst();     // On mémorise la séquence d'instruction
    testerEtAvancer("finpour");
    return nullptr; //new NoeudInstRepeter(condition, sequence); // Et on renvoie un noeud Instruction instRepeter
}

Noeud* Interpreteur::instEcrire() {
//  <instEcrire> ::= ecrire ( <expression> | <chaine> { , <expression> | <chaine> } )
    vector<Noeud*> *aprint = new vector<Noeud*>();
    testerEtAvancer("ecrire");
    testerEtAvancer("(");
    try{
        aprint->push_back(chaine());
        m_lecteur.avancer();
    }catch(SyntaxeException e){
        try{
            aprint->push_back(expression());
        }catch(SyntaxeException e){
            throw e;
            delete aprint;
        }
    }
    bool continuer = true;
    while(continuer){
        try{
            testerEtAvancer(",");
            try{
                aprint->push_back(expression());
            }catch(SyntaxeException e){
                try{
                    SymboleValue * a = dynamic_cast<SymboleValue*>(chaine());
                    aprint->push_back(a);
                    m_lecteur.avancer();
                }catch(SyntaxeException e){
                    throw e;
                    delete aprint;
                }
            }
        }catch(SyntaxeException e){
            continuer = false;
        }
    }
    testerEtAvancer(")");
    testerEtAvancer(";");
    return new NoeudInstEcrire(*aprint); // Et on renvoie un noeud Instruction instEcrire
}


