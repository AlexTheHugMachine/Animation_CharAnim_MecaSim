#include <stdlib.h>


#include "Viewer.h"
#include "vec.h"

using namespace std;



int main( int argc, char **argv )
{
    /**********************************/
    /** Pour la simulation mecanique **/
    /**********************************/
    
    /// Nombre d objets presents dans la simulation mecanique
    int NbObj;
    
    /// Tableau contenant les noms des fichiers de parametres des objets de la simulation mecanique
    string *Fichier_Param;
    
    /// Pour ne pas a avoir a mettre les fichiers en parametres de l execution
    /// Pour ne pas a avoir a mettre les fichiers en parametres de l execution
    if (argc == 1)
    {
        argc=4; // modifie le nombre de parametre mis dans l execution
        
        argv = new char*[4];
        
        argv[0] = new char[256]; // Nom de l executable
        argv[1] = new char[256]; // Nb d objet dans la simulation - NB doit etre egale a 1
        argv[2] = new char[256]; // Nom du fichier de parametre de la simulation
        argv[3] = new char[256]; // Nom du fichier de parametre de l objet
        
        strcpy( argv[0], "master_meca_sim_etudiant");
        strcpy( argv[1], "1");
        strcpy( argv[2], "master_meca_sim/exec/Fichier_Param.simu");
        strcpy( argv[3], "master_meca_sim/exec/Fichier_Param.objet");
        
    }
    
    /// Fichier contenant les parametres de l execution
    if (argc >= 2)
    {
        /// Nombre d objets a simuler
        NbObj = atoi(argv[1]);
            cout << "Nombre d objets dans la simulation mecanique : " << NbObj << endl;
        
        /// Tableau contenant les noms des fichiers de parametres
        // Element 0 correspond au fichier contenant les parametres generaux de l animation
        // Element i correspond au fichier contenant les parametres de l objet i
        Fichier_Param = new string[NbObj+1];
        
        // Recupere les noms des fichiers de donnees :
        // argv[0] : executable, argv[1] : nb objet
        // Fichier_Param[0] de la simulation correspond a argv[2]
        for (int i=0; i<= NbObj; i++)
            Fichier_Param[i] = argv[i+2];
        
        cout << "Fichiers de donnees de la simulation : " << Fichier_Param[0] << endl;
        cout << "Fichier de donnees de l objet : " << Fichier_Param[1] << endl;
        
    }
    
    else
    {
        /// Usage de l execution du programme
        // Exemple pour un seul objet simule :
        // ./bin/master_meca_sim 1 ./exec/master_meca_sim/Fichier_Param.simu exec/master_meca_sim/Fichier_Param.objet
        
        cout << "Usage depuis le repertoire gkit2light:" << endl;
        cout << "<executable> NbObj <Fichier_Param_Anim> <Fichier_Param_Obj1> <Fichier_Param_Obj2> ..." << endl << endl;
        
        cout << "Exemple pour un seul objet : " << endl;
        cout << "./bin/master_meca_sim_corrige 1 master_meca_sim/exec/Fichier_Param.simu master_meca_sim/exec/Fichier_Param.objet" << endl;
        
        
        /// Arret du programme
        exit(1);
        
    }
    
    // Si pas de simulation mecanique
    //Viewer v;
    
    // Avec simulation mecanique
    Viewer v(Fichier_Param, NbObj);
    
    // Run du viewer
    v.run();
    
    
    return 0;
   
}
