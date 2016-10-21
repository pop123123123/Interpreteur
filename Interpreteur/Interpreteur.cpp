#include "Interpreteur.h"
#include <stdlib.h>
#include <iostream>
#include <vector>
using namespace std;

Interpreteur::Interpreteur(ifstream & fichier) :
m_lecteur(fichier), m_arbre(nullptr),m_proc(),m_erreurs(0) {
}

void Interpreteur::traduitEnPython(ostream& cout, unsigned int indentation) {
    this->m_proc.traduitEnPython(cout , 0);
    cout << endl;
    this->getArbre()->traduitEnPython(cout, 0);
}

void Interpreteur::analyse() {
    bool proc_def = true;
    while (proc_def){
        try{
            procedureDefinition();
        }catch(ProcedureException s){
            proc_def = false;
        }
    }
    this->m_table = new TableSymboles;
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

void Interpreteur::avancerFinInstr() {
    while (m_lecteur.getSymbole() == "<VARIABLE>" && m_lecteur.getSymbole() == "si" && m_lecteur.getSymbole() == "tantque"
          && m_lecteur.getSymbole() == "pour" && m_lecteur.getSymbole() == "repeter" && m_lecteur.getSymbole() == "ecrire"
          && m_lecteur.getSymbole() == "lire" && m_lecteur.getSymbole() == "|" && m_lecteur.getSymbole() != "<FINDEFICHIER>")
        m_lecteur.avancer();
    
}

Noeud* Interpreteur::programme() {
    // <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
    try {
        //-testerEtAvancer("procedure");
        testerEtAvancer("principale");
        testerEtAvancer("(");
        testerEtAvancer(")");
        Noeud* sequence = seqInst();
        testerEtAvancer("finproc");
        tester("<FINDEFICHIER>");
        return sequence;
    } catch (SyntaxeException & se) {
        m_erreurs++;
        cout << se.what() << endl;
        avancerFinInstr();
    }
}

Noeud* Interpreteur::seqInst() {
  // <seqInst> ::= <inst> { <inst> }
  NoeudSeqInst* sequence = new NoeudSeqInst();
  do {
    sequence->ajoute(inst());
  } while (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "si" || m_lecteur.getSymbole() == "tantque"
          || m_lecteur.getSymbole() == "pour" || m_lecteur.getSymbole() == "repeter" || m_lecteur.getSymbole() == "ecrire"
          || m_lecteur.getSymbole() == "lire" || m_lecteur.getSymbole() == "|" || m_lecteur.getSymbole() == "appel" );
  // Tant que le symbole courant est un début possible d'instruction...
  // Il faut compléter cette condition chaque fois qu'on rajoute une nouvelle instruction
  return sequence;
}

Noeud* Interpreteur::inst() {
    // <inst> ::= <affectation>  ; | <instSi>
    try {
        if (m_lecteur.getSymbole() == "<VARIABLE>") {
            Noeud *affect = affectation();
            testerEtAvancer(";");
            return affect;
        } else if (m_lecteur.getSymbole() == "si")
            return instSi();
        else if (m_lecteur.getSymbole() == "tantque")
            return instTantQue();
        else if (m_lecteur.getSymbole() == "repeter")
            return instRepeter();
        else if (m_lecteur.getSymbole() == "pour")
            return instPour();
        else if (m_lecteur.getSymbole() == "ecrire")
            return instEcrire();
        else if (m_lecteur.getSymbole() == "lire")
            return instLire();
        else if (m_lecteur.getSymbole() == "appel")
            return instProcedure();
            // Compléter les alternatives chaque fois qu'on rajoute une nouvelle instruction
        else erreur("Instruction incorrecte");
    } catch (SyntaxeException & se) {
        m_erreurs++;
        cout << se.what() << endl;
        avancerFinInstr();
    }
}

Noeud* Interpreteur::affectation() {
  // <affectation> ::= <variable> = <expression> 
  tester("<VARIABLE>");
  Noeud* var = m_table->chercheAjoute(m_lecteur.getSymbole()); // La variable est ajoutée à la table et on la mémorise
  m_lecteur.avancer();
  testerEtAvancer("=");
  Noeud* exp = expression();             // On mémorise l'expression trouvée
  return new NoeudAffectation(var, exp); // On renvoie un noeud affectation
}

Noeud* Interpreteur::expression() {
    //  <expression> ::= <terme> { + <terme> | - <terme> }
    Noeud* ter = terme();
    while (m_lecteur.getSymbole() == "+" || m_lecteur.getSymbole() == "-") {
        Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
        m_lecteur.avancer();
        Noeud* terDroit = terme(); // On mémorise l'opérande droit
        ter = new NoeudOperateurBinaire(operateur, ter, terDroit); // Et on construit un noeud opérateur binaire
    }
    return ter;
}

Noeud* Interpreteur::facteur() {
  // <facteur> ::= <entier> | <variable> | - <facteur> | non <facteur> | | <facteur> | | ( <expression> )
  Noeud* fact = nullptr;
  if (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "<ENTIER>") {
    fact = m_table->chercheAjoute(m_lecteur.getSymbole()); // on ajoute la variable ou l'entier à la table
    m_lecteur.avancer();
  } else if (m_lecteur.getSymbole() == "-") { // - <facteur>
    m_lecteur.avancer();
    // on représente le moins unaire (- facteur) par une soustraction binaire (0 - facteur)
    fact = new NoeudOperateurBinaire(Symbole("-"), m_table->chercheAjoute(Symbole("0")), facteur());
  } else if (m_lecteur.getSymbole() == "non") { // non <facteur>
    m_lecteur.avancer();
    // on représente le moins unaire (- facteur) par une soustractin binaire (0 - facteur)
    fact = new NoeudOperateurBinaire(Symbole("non"), facteur(), nullptr);
  } else if (m_lecteur.getSymbole() == "|") { // expression parenthésée
    m_lecteur.avancer();
    fact = new NoeudInstAbs(facteur());
    testerEtAvancer("|");
  } else if (m_lecteur.getSymbole() == "(") { // expression parenthésée
    m_lecteur.avancer();
    fact = expression();
    testerEtAvancer(")");
  } else
    erreur("Facteur incorrect");
  return fact;
}

Noeud* Interpreteur::terme(){
    //       <terme> ::= <facteur> { * <facteur> | / <facteur> }
    Noeud* factor = facteur();
    while (m_lecteur.getSymbole() == "*" || m_lecteur.getSymbole() == "/"){
        Symbole operateur = m_lecteur.getSymbole();
        m_lecteur.avancer();
        Noeud* opDroit = facteur();
        factor = new NoeudOperateurBinaire(operateur, factor, opDroit);
    }
    return factor;
}


Noeud* Interpreteur::expBool(){
    //     <expBool> ::= <relationET> { ou <relationEt> }
    Noeud* relation = relationEt();
    while(m_lecteur.getSymbole() == "ou"){
        Symbole operateur = m_lecteur.getSymbole();
        m_lecteur.avancer();
        Noeud* opDroit = relationEt();
        relation = new NoeudOperateurBinaire(operateur, relation, opDroit);
    }
    return relation;
}

Noeud* Interpreteur::relationEt(){
    //  <relationET> ::= <relation> { et <relation> }
    Noeud* rel = relation();
    while(m_lecteur.getSymbole() == "et"){
        Symbole operateur = m_lecteur.getSymbole();
        m_lecteur.avancer();
        Noeud* opDroit = relation();
        rel = new NoeudOperateurBinaire(operateur, rel, opDroit);
    }
    return rel;
}

Noeud* Interpreteur::relation(){
    //    <relation> ::= <expression> { <opRel> <expression> }
    Noeud* relation = expression();
    while(m_lecteur.getSymbole() == "==" || m_lecteur.getSymbole() == "!=" ||
            m_lecteur.getSymbole() == "<" || m_lecteur.getSymbole() == "<=" ||
            m_lecteur.getSymbole() == ">" || m_lecteur.getSymbole() == ">=")
    {
        Symbole operateur = m_lecteur.getSymbole();
        m_lecteur.avancer();
        Noeud* opDroit = expression();
        relation = new NoeudOperateurBinaire(operateur, relation, opDroit);
    }
    return relation;
}

Noeud* Interpreteur::chaine() {
  tester("<CHAINE>");
  Symbole a = m_lecteur.getSymbole();
  return m_table->chercheAjoute(a); // on ajoute la chaine
}

void Interpreteur::procedureDefinition() {
//   <programme> ::= procedure <MOT> ([ <variable> ] { , <variable> }) <seqInst> finproc
    try{
        testerEtAvancer("procedure");
    }catch (SyntaxeException & se) {
        throw ProcedureException();
    }
    try{
        try{
            cout << m_lecteur.getSymbole() << endl;
            tester("<VARIABLE>");
        }catch(SyntaxeException & se){
            tester("principale");
            throw ProcedureException("mot clé déjà défini dans le langage");
        }
        string nom = m_lecteur.getSymbole().getChaine();
        this->m_lecteur.avancer();
        testerEtAvancer("(");
        vector<SymboleValue*> *arguments = new vector<SymboleValue*>;
        this->m_table = new TableSymboles;
        
        // Définition des arguments
        // -----------------------------------------------
        bool args = true;
        try{
            tester("<VARIABLE>");
            Symbole a = m_lecteur.getSymbole();
            m_lecteur.avancer();
            arguments->push_back(m_table->chercheAjoute(a));
        }catch(SyntaxeException e){
            args = false;
        }
        try{
            while(args){
                try{
                    this->testerEtAvancer(",");
                    tester("<VARIABLE>");
                    Symbole a = m_lecteur.getSymbole();
                    m_lecteur.avancer();
                    arguments->push_back(m_table->chercheAjoute(a));
                }catch(SyntaxeException e){
                    args = false;
                }
            }
        testerEtAvancer(")");

            // Définition du corps de la procédure
            // -----------------------------------------------

            Noeud* sequence = seqInst();
            testerEtAvancer("finproc");
            this->m_proc.addProcedure(nom,arguments,sequence,m_table);
        }catch(SyntaxeException e){
            delete arguments;
            delete m_table;
            throw e;
        }
    } catch (SyntaxeException & se) {
        m_erreurs++;
        cout << se.what() << endl;
        avancerFinInstr();
    }
}

Noeud* Interpreteur::instProcedure() {
    testerEtAvancer("appel");
    tester("<VARIABLE>");
    Procedure * proc = this->m_proc.getProcedure(this->m_lecteur.getSymbole().getChaine());
    m_lecteur.avancer();
    testerEtAvancer("(");
    vector<Noeud*> *args = new vector<Noeud*>;
    bool continuer = true;
    try{
        args->push_back(expression());
    }catch(SyntaxeException e){
        continuer = false;
    }
    while(continuer){
        try{
            testerEtAvancer(",");
            try{
                args->push_back(expression());
            }catch(SyntaxeException e){
                throw e;
                delete args;
            }
        }catch(SyntaxeException e){
            continuer = false;
        }
    }
    testerEtAvancer(")");
    testerEtAvancer(";");
    return new NoeudInstProc(proc,args);
}

Noeud* Interpreteur::instSi() {
  // <instSi> ::= si ( <expression> ) <seqInst> finsi
    vector<Noeud*> *conditions = new vector<Noeud*>();
    vector<Noeud*> *sequences = new vector<Noeud*>();
    try{
        testerEtAvancer("si");
        testerEtAvancer("(");
        Noeud* condition = expBool(); // On mémorise la condition
        conditions->push_back(condition);
        testerEtAvancer(")");
        Noeud* sequence = seqInst();     // On mémorise la séquence d'instruction
        sequences->push_back(sequence);
        bool elseif = true;
        while(elseif){
            try{
                testerEtAvancer("sinonsi");
                testerEtAvancer("(");
                Noeud* condition = expBool(); // On mémorise la condition
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
  Noeud* condition = expBool(); // On mémorise la condition
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
  Noeud* condition = expBool(); // On mémorise la condition
  testerEtAvancer(")");
  return new NoeudInstRepeter(condition, sequence); // Et on renvoie un noeud Instruction instRepeter
}

Noeud* Interpreteur::instPour() {
    //<instPour> ::= pour ( [<affectation>]; <expBool>; [<affectation>] ) <seqInst> finpour
    testerEtAvancer("pour");
    testerEtAvancer("(");
    // Test affectation d'une variable => optionnel
    Noeud* declaration = nullptr;
    try{
        declaration = affectation();
    }catch(SyntaxeException e){
    }
    testerEtAvancer(";");

    // Test condition de boucle
    Noeud* condition = expBool();
    testerEtAvancer(";");
    
    // Test incrémentation d'une variable => optionnel
    Noeud* incrementation = nullptr;
    try{
        incrementation = affectation();
    }catch(SyntaxeException e){
    }
    testerEtAvancer(")");
    Noeud* sequence = seqInst();     // On mémorise la séquence d'instruction
    testerEtAvancer("finpour");
    return new NoeudInstPour(declaration, condition, incrementation, sequence); // Et on renvoie un noeud Instruction instRepeter
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

Noeud* Interpreteur::instLire() {
    //    <instLire> ::= lire ( <variable> { , <variable> } )
    vector<Noeud*> *expressions = new vector<Noeud*>();
    try {
        testerEtAvancer("lire");
        testerEtAvancer("(");
        if(m_lecteur.getSymbole()=="<VARIABLE>")
            expressions->push_back(m_table->chercheAjoute(m_lecteur.getSymbole()));
        m_lecteur.avancer();

        while(m_lecteur.getSymbole()== ","){
            testerEtAvancer(",");
            if(m_lecteur.getSymbole()=="<VARIABLE>")
                expressions->push_back(m_table->chercheAjoute(m_lecteur.getSymbole()));
            m_lecteur.avancer();
        }
        testerEtAvancer(")");
        testerEtAvancer(";");
    } catch (SyntaxeException se){
        delete expressions;
        throw se;
    }
    return new NoeudInstLire(*expressions);
}

Noeud* Interpreteur::instAbs() {
    //    <instAbs> ::= | <expression> |
    testerEtAvancer("|");
    Noeud* variable = expression();
    testerEtAvancer("|");
    return new NoeudInstAbs(variable);
}
