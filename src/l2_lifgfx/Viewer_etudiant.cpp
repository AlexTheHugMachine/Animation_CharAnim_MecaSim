
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>

#include "draw.h"        // pour dessiner du point de vue d'une camera
#include "Viewer_etudiant.h"

using namespace std;





/*
 * Exemple de definition de fonction permettant la creation du Mesh de votre Objet.
 */
//void ViewerEtudiant::init_votreObjet()
//{
//}





/*
 * Fonction dans laquelle les initialisations sont faites.
 */
int ViewerEtudiant::init()
{
    Viewer::init();
    
    m_camera.lookat( Point(0,0,0), 150 );
    
    
    /// Appel des fonctions init_votreObjet pour creer les Mesh
    
    
    /// Chargement des textures
    
    
    return 0;
}



/*
 * Exemple de definition de fonction permettant l affichage
 * de 'votreObjet' subissant la Transform T
 */
//void ViewerEtudiant::draw_votreObjet(const Transform& T)
//{
// gl.texture(....);
// gl.model( T );
// gl.draw( m_votreObjet );
//}


/*
 * Fonction dans laquelle les appels pour les affichages sont effectues.
 */
int ViewerEtudiant::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    manageCameraLight();

    gl.camera(m_camera);
    
    
    /// Appel de la fonction render() de la class Viewer
    /// pour que vous voyez apparaitre un premier objet
    /// a supprimer ensuite
    Viewer::render();
    
    
    /// Appel des fonctions du type 'draw_votreObjet'

    
    return 1;
    
}


/*
 * Fonction dans laquelle les mises a jours sont effectuees.
 */
int ViewerEtudiant::update( const float time, const float delta )
{
    // time est le temps ecoule depuis le demarrage de l'application, en millisecondes,
    // delta est le temps ecoule depuis l'affichage de la derniere image / le dernier appel a draw(), en millisecondes.
    
    
    
    return 0;
}


/*
 * Constructeur.
 */

ViewerEtudiant::ViewerEtudiant() : Viewer()
{
}


/*
 * Programme principal.
 */
int main( int argc, char **argv )
{
    ViewerEtudiant v;
    v.run();
    return 0;
}
