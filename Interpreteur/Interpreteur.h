#ifndef INTERPRETEUR_H
#define INTERPRETEUR_H

#include "Symbole.h"
#include "Lecteur.h"
#include "Exceptions.h"
#include "TableSymboles.h"
#include "TableProcedures.h"
#include "ArbreAbstrait.h"

class Interpreteur {
public:
	Interpreteur(ifstream & fichier);   // Construit un interpréteur pour interpreter
	                                    //  le programme dans  fichier 
                                      
	void analyse();                     // Si le contenu du fichier est conforme à la grammaire,
	                                    //   cette méthode se termine normalement et affiche un message "Syntaxe correcte".
                                      //   la table des symboles (ts) et l'arbre abstrait (arbre) auront été construits
	                                    // Sinon, une exception sera levée

	inline const TableSymboles & getTable () const  { return m_table;    } // accesseur	
	inline Noeud* getArbre () const { return m_arbre; }                    // accesseur
	inline int getNbErreurs () const { return m_erreurs; }                    // accesseur
	
private:
    Lecteur        m_lecteur;  // Le lecteur de symboles utilisé pour analyser le fichier
    TableSymboles  m_table;    // La table des symboles valués
    Noeud*         m_arbre;    // L'arbre abstrait
    TableProcedures m_proc;    // La table des procédures
    int            m_erreurs;  // Nombre d'erreurs
    // Implémentation de la grammaire
    Noeud*  programme();   //   <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
    Noeud*  seqInst();	   //     <seqInst> ::= <inst> { <inst> }
    Noeud*  inst();	   //        <inst> ::= <affectation> ; | <instSi>
    Noeud*  affectation(); // <affectation> ::= <variable> = <expression> 
    
    Noeud*  expression();  //  <expression> ::= <terme> { + <terme> | - <terme> }
    Noeud*  terme();       //       <terme> ::= <facteur> { * <facteur> | / <facteur> }
    Noeud*  facteur();     //     <facteur> ::= <entier>  |  <variable>  |  - <facteur>  | non <facteur> | | <facteur> | | ( <expression> )
    Noeud*  expBool();     //     <expBool> ::= <relationET> { ou <relationEt> }
    Noeud*  relationEt();  //  <relationET> ::= <relation> { et <relation> }
    Noeud*  relation();    //    <relation> ::= <expression> { <opRel> <expression> }
                           //       <opRel> ::= == | != | < | <= | > | >=

    Noeud*  chaine();      //      <chaine> ::= <CHAINE>
                           //   <opBinaire> ::= + | - | *  | / | < | > | <= | >= | == | != | et | ou
    Noeud*  procedure();   //   <programme> ::= procedure <CHAINE> ([ <variable> ] { , <variable> }) <seqInst> finproc
    Noeud*  instSi();      //      <instSi> ::= si ( <expression> ) <seqInst> { sinonsi ( <expression> ) <seqInst> } [ sinon <seqInst> ] finsi
    Noeud*  instTantQue(); // <instTantQue> ::= tantque ( <expression> ) <seqInst> fintantque
    Noeud*  instRepeter(); // <instRepeter> ::= repeter <seqInst> jusqua ( <expression> )
    Noeud*  instPour();    //    <instPour> ::= pour ( [<affectation>]; <expression>; [<affectation>] ) <seqInst> finpour
    Noeud*  instEcrire();  //  <instEcrire> ::= ecrire ( <expression> | <chaine> { , <expression> | <chaine> } )
    Noeud*  instLire();    //    <instLire> ::= lire ( <variable> { , <variable> } )
    Noeud*  instAbs();     //     <instAbs> ::= | <expression> |

    // outils pour simplifier l'analyse syntaxique
    void tester (const string & symboleAttendu) const throw (SyntaxeException);   // Si symbole courant != symboleAttendu, on lève une exception
    void testerEtAvancer(const string & symboleAttendu) throw (SyntaxeException); // Si symbole courant != symboleAttendu, on lève une exception, sinon on avance
    void erreur (const string & mess) const throw (SyntaxeException);             // Lève une exception "contenant" le message mess
    void avancerFinInstr(); // avancer jusqu'au début de l'instruction suivante
};

#endif /* INTERPRETEUR_H */
