/** \file Viewer.cpp
 * \brief Viewer de l application.
 */


#include <cassert>
#include <cmath>
#include <cstdio>
#include <iostream>

#include "draw.h"        // pour dessiner du point de vue d'une camera
#include "Viewer.h"
#include "Scene.h"

#include "ObjetSimule.h"
#include "ObjetSimuleMSS.h"
#include "ObjetSimuleRigidBody.h"
#include "ObjetSimuleParticule.h"
#include "ObjetSimuleSPH.h"



/*
 * Constructeur.
 */
Viewer::Viewer() : App(1024, 768),
mb_cullface(true), // Par defaut - gestion des faces cachees
mb_wireframe(false), // Par defaut - affiche plein
b_draw_grid(true), // Par defaut - affiche la grille
b_draw_axe(true) // Par defaut - affiche les axes
{
}


/*
 * Constructeur dans le cas ou il y a une simulation mecanique.
 */
Viewer::Viewer(string *Fichier_Param, int NbObj) :
App(1024, 768),
mb_cullface(true), // Par defaut - gestion des faces cachees
mb_wireframe(false), // Par defaut - affiche plein
b_draw_grid(true), // Par defaut - affiche la grille
b_draw_axe(true) // Par defaut - affiche les axes
{
    
    /** Declaration du graphe de scene pour la simulation mecanique **/
    
    /// Scene construite a partir des parametres mis dans le fichier de parametres de la simulation
    _Simu = new Scene(Fichier_Param[0], NbObj);
    
    /// Ajoute les objets de la simulation au graphe de scene
    /// Objets construits a partir des parametres mis dans les fichiers de parametres des objets
    for (int i=1; i<=_Simu->_NbObj ; i++)
    {
        cout  << "Creation de l objet " << i << " de type : " << _Simu->_type_objet[i-1] << endl;
        
        if (_Simu->_type_objet[i-1] == "mss")
            _Simu->attache(new ObjetSimuleMSS(Fichier_Param[i]));
        
        else if (_Simu->_type_objet[i-1] == "particule")
            _Simu->attache(new ObjetSimuleParticule(Fichier_Param[i]));
        
        else if (_Simu->_type_objet[i-1] == "rigid")
            _Simu->attache(new ObjetSimuleRigidBody(Fichier_Param[i]));
        
        else if(_Simu->_type_objet[i-1] == "sph")
            _Simu->attache(new ObjetSimuleSPH(Fichier_Param[i]));
        
    }
    
    /// Initialisation des objets pour l'animation
    _Simu->initObjetSimule();
    
    /// Creation des maillages (de type Mesh) des objets de la scene
   _Simu->initMeshObjet();
    
}



/*
 * Aide de la fenetre d affichage.
 */
void Viewer::help()
{
    cout << "-------- HELP: -------"<< endl<< endl;
    
    cout << "   h: help" << endl << endl;
    
    cout << "   c: (des)active GL_CULL_FACE" << endl;
    cout << "   w: (des)active wireframe" << endl;
    cout << "   a: (des)active l'affichage de l'axe" << endl;
    cout << "   g: (des)active l'affichage de la grille" << endl << endl;
    
    cout << "   m+fleche/pageUp/pageDown: pour bouger point interaction" << endl << endl;
    
    cout << "   fleches/pageUp/pageDown: bouge la camera"<< endl << endl;
    
    cout << "   Ctrl+fleche/pageUp/pageDown: bouge la source de lumiere" << endl<< endl;
    
    cout << "   Souris+bouton gauche: rotation" << endl << endl;
    
    cout << "   Souris mouvement vertical+bouton droit: (de)zoom" << endl << endl;
    
}


/*
 * Affichage de la scene.
 */
int Viewer::render( )
{
    // Efface l'ecran
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Deplace la camera, lumiere, etc.
    manageCameraLight();
    
    // donne notre camera au shader
    gl.camera(m_camera);
    
    // Afichage d un cube avec texture
    //gl.texture(m_cube_texture);
    //gl.model(Translation( -3, 5, 0 ));
    //gl.draw(m_cube);
    
    // Rajouter ici les appels pour afficher votre objet non simule
    // Exemple :
    //gl.texture(m_votreObjet_texture);
    //gl.model(...);
    //gl.draw(m_votreObjet);
    
    // Gestion de la lumiere
    //gl.lighting(false);
    //gl.lighting(true);
    
    /**********************************************************************/
    /* Affichage des objets du graphe de scene de la simulation mecanique */
    /**********************************************************************/
    
    /* Gestion interaction avec la touche m */
    const float step_i = 0.01f;
    
    if (key_state(SDLK_RIGHT) && key_state(SDLK_m)) {MousePos = MousePos+Vector(step_i,0,0); }
    if (key_state(SDLK_LEFT) && key_state(SDLK_m)) {MousePos = MousePos+Vector(-step_i,0,0); }
    if (key_state(SDLK_UP) && key_state(SDLK_m)) {MousePos = MousePos+Vector(0,0,-step_i); }
    if (key_state(SDLK_DOWN) && key_state(SDLK_m)) {MousePos = MousePos+Vector(0,0,step_i); }
    if (key_state(SDLK_PAGEUP) && key_state(SDLK_m)) {MousePos = MousePos+Vector(0,step_i,0); }
    if (key_state(SDLK_PAGEDOWN) && key_state(SDLK_m)) {MousePos = MousePos+Vector(0,-step_i,0); }
    
    /// Interaction avec l utilisateur
    _Simu->Interaction(MousePos);
    MousePos = Vector(0,0,0);
    
    
    /* Affichage des objets */
    Transform T;
    T = Translation( 0, 3, 0 );
    
    ListeNoeuds::iterator e;
    int num=0;
  
    for(e=_Simu->_enfants.begin(); e!=_Simu->_enfants.end(); e++)
    {
        // Cas systeme masses-ressorts
        if (_Simu->_type_objet[num] == "mss")
        {
            // Specification de la texture de l objet simule
            if ((*e)->_use_texture)
                gl.texture(m_tissu_texture);
            
            // Transformation geometrique appliquee a l objet
            //gl.model(Translation( 0, 3, 1 ) * Scale(3, 3, 3)); // pour le drap10
            gl.model( T * Scale(0.3, 0.3, 0.3))  ; // pour le drap70
            //gl.model( T )  ; // pour le drap70

            
            // Affichage du Mesh de l objet du graphe de scene
            gl.draw((*e)->m_ObjetSimule);
            
            // Affichage d un cube au point d interaction
            // !TODO : pb avec le scale - cube plus sur le point 0 qd bouge
            // gl.texture(0);
            gl.model((Translation( Vector((*e)->Coord_Point_Inter)) * T ) * Scale(0.03, 0.03, 0.03));
            gl.draw(m_cube);
            
        }//mss
        
         // Cas systeme de particules non connectees
        else if (_Simu->_type_objet[num] == "particule" || _Simu->_type_objet[num] == "sph")
        {
            // Affichage des particules
            for (int i =0; i<(*e)->_Nb_Sommets; i++)
            {
                // Positionnement en fonction de la position de la particule
                gl.model(Translation( Vector((*e)->P[i])));
                
                // Affichage d une sphere pour modeliser une particule
                gl.draw(m_sphere);
            }
            
        }//particule
        
        // Cas systeme de particules non connectees
        else if (_Simu->_type_objet[num] == "rigid")
        {
            // Specification de la texture de l objet simule
            if ((*e)->_use_texture)
                gl.texture(m_tissu_texture);
            
            // Transformation geometrique appliquee a l objet
         //  gl.model(T * Scale(0.3, 0.3, 0.3));
            gl.model(Identity());
                
            // Affichage du Mesh de l objet du graphe de scene
            gl.draw((*e)->m_ObjetSimule);
            
        }//rigid

        
        // Affichage du plan ou se produisent les collisions
        // gl.model(Identity());
        //   gl.draw(m_plan);
        
        // Passage a l objet suivant
        num++;
        
    }
    
    return 1;
    
}


/*
 * Mise a jour de la scene.
 */
int Viewer::update( const float time, const float delta )
{
    // Mettre ici le code pour modifier votre objet au cours du temps
    
    //std::cout << " Viewer::update ...." << std::endl;
    
    /***************************************************************************/
    /* Mise a jour du maillage des objets du graphe de scene de la simulation  */
    /***************************************************************************/
    
    /// Calcul de l animation
    _Simu->Simulation(Tps);
    
    /// Mise a jour du Mesh en fct des positions calculees
    ListeNoeuds::iterator e;
    int num=0;
    
    for(e=_Simu->_enfants.begin(); e!=_Simu->_enfants.end(); e++)
    {
        // Cas systeme masses-ressorts ou objet rigide
        if ((_Simu->_type_objet[num] == "mss") || (_Simu->_type_objet[num] == "rigid"))
        {
            // Mise a jour du Mesh des objets
            (*e)->updateVertex();
            
        }
        
        // Passage a l objet suivant
        num++;
        
    }
    
    /// Le temps qui passe...
    Tps = Tps + 1;
    //cout << "Temps : " << Tps << endl;
    
    return 1;
}


/*
 * Gestion de la camera et de la lumiere.
 */
void Viewer::manageCameraLight()
{
    // Recupere les mouvements de la souris pour deplacer la camera, cf tutos/tuto6.cpp
    int mx, my;
    unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);
    
    // Deplace la camera avec la souris
    // Si le bouton du milieu est enfonce
    if((mb & SDL_BUTTON(1)) &&  (mb& SDL_BUTTON(3)))
    {
        // deplace le point de rotation
        m_camera.translation( (float) mx / (float) window_width(), (float) my / (float) window_height());
    }
    else if(mb & SDL_BUTTON(1))           // le bouton gauche est enfonce
        m_camera.rotation( mx, my);       // tourne autour de l'objet
    
    else if(mb & SDL_BUTTON(3))           // le bouton droit est enfonce
        m_camera.move( my);               // approche / eloigne l'objet
    
    // Deplace la camera avec le clavier
    if (key_state(SDLK_PAGEUP) && (!key_state(SDLK_LCTRL)) && (!key_state(SDLK_m)) ) { m_camera.translation( 0,0.01); }
    if (key_state(SDLK_PAGEDOWN) && (!key_state(SDLK_LCTRL)) && (!key_state(SDLK_m))) { m_camera.translation( 0,-0.01); }
    if (key_state(SDLK_LEFT) && (!key_state(SDLK_LCTRL))&& (!key_state(SDLK_m))) { m_camera.translation(  0.01,0); }
    if (key_state(SDLK_RIGHT) && (!key_state(SDLK_LCTRL))&& (!key_state(SDLK_m))) { m_camera.translation( -0.01,0); }
    if (key_state(SDLK_UP) && (!key_state(SDLK_LCTRL))&& (!key_state(SDLK_m))) { m_camera.move( 1); }
    if (key_state(SDLK_DOWN) && (!key_state(SDLK_LCTRL))&& (!key_state(SDLK_m))) { m_camera.move( -1); }
    
    
    // Deplace la lumiere avec le clavier
    const float step = 0.1f;
    if (key_state(SDLK_RIGHT) && key_state(SDLK_LCTRL)&& (!key_state(SDLK_m))) { gl.light( gl.light()+Vector(step,0,0)); }
    if (key_state(SDLK_LEFT) && key_state(SDLK_LCTRL)&& (!key_state(SDLK_m))) { gl.light( gl.light()+Vector(-step,0,0)); }
    if (key_state(SDLK_UP) && key_state(SDLK_LCTRL)&& (!key_state(SDLK_m))) { gl.light( gl.light()+Vector(0,0,-step)); }
    if (key_state(SDLK_DOWN) && key_state(SDLK_LCTRL)&& (!key_state(SDLK_m))) { gl.light( gl.light()+Vector(0,0,step)); }
    if (key_state(SDLK_PAGEUP) && key_state(SDLK_LCTRL)&& (!key_state(SDLK_m))) { gl.light( gl.light()+Vector(0,step,0)); }
    if (key_state(SDLK_PAGEDOWN) && key_state(SDLK_LCTRL)&& (!key_state(SDLK_m))) { gl.light( gl.light()+Vector(0,-step,0)); }
    
    
    // Afichage de l aide
    if (key_state('h'))
        help();
    
    // Affichage des faces cachees ou non
    if (key_state('c')) {
        clear_key_state('c');
        mb_cullface=!mb_cullface;
        if (mb_cullface)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);
    }
    
    // Affichage filaire ou plein
    if (key_state('w')) {
        clear_key_state('w');
        mb_wireframe=!mb_wireframe;
        
        if (mb_wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    // Grille : oui/non
    if (key_state('g')) {
        b_draw_grid = !b_draw_grid;
        clear_key_state('g');
    }
    
    // Axe : oui/non
    if (key_state('a')) {
        b_draw_axe = !b_draw_axe;
        clear_key_state('a');
    }
    
    // Camera
    gl.camera(m_camera);
    
    // Affichage de AXE et GRILLE
    gl.model( Identity() );
    if (b_draw_grid)
        gl.draw(m_grid);
    if (b_draw_axe)
        gl.draw(m_axe);
    
    // Affichage d un cube representant la position de la LIGHT
    gl.texture( 0 );
    gl.model( Translation( Vector( gl.light())) * Scale(0.3, 0.3, 0.3) );
    gl.draw(m_cube);
    
}


int Viewer::quit( )
{
    return 0;
}
