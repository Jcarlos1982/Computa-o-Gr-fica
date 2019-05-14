#include <GL/glut.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;


// Variáveis
GLfloat win, r, g, b, angRot=0, angulo=0, fAspect,  escala=1, posX=0, posY=0, passoX=1, passoY=1;
GLint view_w, view_h, obj=0, eixo=2;

//posições da cena
// POSICAO INICIAL - gluLookAt(0,80,200, 0,0,0, 0,1,0);
GLdouble camX=0, camY=80, camZ=200, alvoX=0, alvoY=0, alvoZ=0, cimaX=0, cimaY=1, cimaZ=0, zNear=0.4;
GLint cena=0, zFar=500, tpVisualizacao=0;


bool solido = false, transl=false, rot=false;;


//aplica translação
void aplicaTranslacao(GLdouble x, GLdouble y, GLdouble z){   
     glTranslatef(x, y, z);
}

//Aplica escala
void aplicaEscala(GLdouble escala){
     glScalef(escala, escala, escala);
}

//aplica rotação
void aplicaRotacao(GLdouble angRot, GLint eixo){
          
          switch (eixo){
          case 0: 
                    glRotated(angRot, 1.0, 0.0, 0.0);
               break;

          case 1:
                   glRotated(angRot, 0.0, 1.0, 0.0);
              break;


          case 2:
                    glRotated(angRot, 0.0, 0.0, 1.0);
               break;
     }

}

void verificaColisao(void){ //inverte o sentido ao colidir (os limites da jannela são -100 e 100)
     if(posX>= 150 || posX <= -150)
          passoX = -passoX;

     if(posY >= 120 || posY <= -120)
          passoY = -passoY;    
}

// Função usada para especificar o volume de visualização
void EspecificaParametrosVisualizacao(void)
{
	// Especifica sistema de coordenadas de projeção
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

     if (tpVisualizacao == 0){
          // Especifica a projeção perspectiva
	     gluPerspective(angulo,fAspect,zNear,zFar);

           // Especifica posição do observador e do alvo
          gluLookAt(camX,camY,camZ, alvoX, alvoY,alvoZ, cimaX,cimaY,cimaZ);

          if(transl){
               
               verificaColisao();


               posX+=passoX; 
               posY+=passoY;

               aplicaTranslacao(posX, posY, 0.0);
          }
          
          aplicaEscala(escala);

          if (rot){
            aplicaRotacao(angRot, eixo);
          }


         

          glMatrixMode(GL_MODELVIEW);
          glLoadIdentity();

     }else{
          //ERRO AQUI
          glOrtho (0.0f, view_w, 0.0f, view_h,0.0f, 500);
          glMatrixMode(GL_MODELVIEW);
          glLoadIdentity();
           // Especifica posição do observador e do alvo
          //gluLookAt(camX,camY,camZ, alvoX, alvoY,alvoZ, cimaX,cimaY,cimaZ);

     }
     
	
}

// Função callback chamada para fazer o desenho
void Desenha()
{   
     glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(r, g, b);
	switch (obj){
          case 0:  {
               if (solido)
                    glutSolidCube(50.0f); 
               else
                    glutWireCube(50.0f);
               break;
               }

              
          case 1: {
               if (solido)
                    glutSolidSphere(50.0f, 50.0f, 100);
               else
                    glutWireSphere(50.0f, 50.0f, 100);
               break;
               }
          case 2: {
               if (solido)
                   glutSolidCone(15.0f, 100.0f, 30.0f, 10);
               else
                    glutWireCone(15.0f, 100.0f, 30.0f, 10);
               break;
          }
          case 3: {
               if (solido)
                   glutSolidTorus(10.0f, 30.0f, 20, 200); 
               else
                    glutWireTorus(10.0f, 30.0f, 20, 20); 
               break;
               }  
          case 4: {
               if (solido)
                   glutSolidTeapot(50.0f);
               else
                    glutWireTeapot(50.0f); 
               break;
               }   
          case 5: {
               if (solido)
                    glutSolidTorus(10.0f, 30.0f, 6, 6);
               else
                     glutWireTorus(10.0f, 30.0f, 6, 6);
               break;
               } 
               
               /*
               - void glutWireCube(GLdouble size); 
               - void glutWireSphere(GLdouble radius, GLint slices, GLint stacks); 
               - void glutWireCone(GLdouble radius, GLdouble height, GLint slices, GLint stacks); 
               - void glutWireTorus(GLdouble innerRadius, GLdouble outerRadius, GLint nsides, GLint rings); 
               - void glutWireIcosahedron(void); 
               - void glutWireOctahedron(void); 
               - void glutWireTetrahedron(void); 
               - void glutWireDodecahedron(GLdouble radius); 
               */  
              
     }
 
     
	// Executa os comandos OpenGL
	glutSwapBuffers();
     

}



// Função callback chamada quando o tamanho da janela é alterado 
void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{ 
     
    // Para previnir uma divisão por zero
	if ( h == 0 ) h = 1;

	// Especifica o tamanho da viewport
	glViewport(0, 0, w, h);
 
	// Calcula a correção de aspecto
	fAspect = (GLfloat)w/(GLfloat)h;

	EspecificaParametrosVisualizacao();
        
    }


// Função callback chamada para gerenciar eventos de teclado
void GerenciaTeclado(unsigned char key, int x, int y)
{
     
     if (angRot < 0)
          angRot -=5;
     else
          angRot+=5;
          
    switch (key) {
            case 'X': 
            case 'x':// muda para o eixo x
                     eixo = 0; 
                     break;
            case 'Y':
            case 'y':// muda para o eixo y
                     eixo = 1;
                     break;
            case 'Z':
            case 'z':// muda para o eixo z
                     eixo = 2;
                     break;
           case 'A':
           case 'a':// Escala +
                    escala *=2; 
              
                     break;
          case 'D':
          case 'd': //Escala -
                    escala /=2; 

                    break;    
          case 'I':
          case 'i':// inverte o sentido da rotação
                     angRot = -angRot;
                     
                     break; 

          case 'R':
          case 'r': //Troca o objeto
               {
                    obj--;
                    if(obj < 0)
                      obj= 5;

                    break;
               }  

          case 'S':
          case 's': //Sólido ou não
               {
                    solido = !solido;

                    break;
               }  
                         
          case 'T':
          case 't': //Troca o objeto
               {
                    obj++;
                    if(obj > 5)
                      obj= 0;

                    break;
               }   
               

                      
                                   
    }
    EspecificaParametrosVisualizacao();
    glutPostRedisplay();
}


// Função callback chamada para gerenciar eventos do teclado   
// para teclas especiais, tais como F1, PgDn e Home
void TeclasEspeciais(int key, int x, int y) //Aumenta e diminui a velocidade do objeto 
{
    if(key == GLUT_KEY_UP) { //seta pra cima - aumenta Y
          switch(cena){ //0 - observador 1 - alvo - 2 - orientacao
               case 0: camY+=5;
                    break;
               case 1: alvoY+=5;
                    break;
               case 2: cimaX = 0; 
                    cimaY=1;
                    cimaZ = 0;                      
                    break;
          }
                                  
     }
       
    if(key == GLUT_KEY_DOWN) { //seta pra baixo - diminui Y
          switch(cena){ //0 - observador 1 - alvo - 2 - orientacao
               case 0: camY-=5;
                    break;
               case 1: alvoY-=5;
                    break;
               case 2: cimaX = 0; 
                    cimaY=1;
                    cimaZ = 0;                      
                    break;
          }
         

    }

      if(key == GLUT_KEY_LEFT) { //seta pra esquerda - diminui X
           switch(cena){ //0 - observador 1 - alvo - 2 - orientacao
               case 0: camX-=5;
                    break;
               case 1: alvoX-=5;
                    break;
               case 2: cimaX = 1; 
                       cimaY=0;
                       cimaZ = 0;                      
                    break;
          }

    }

     if(key == GLUT_KEY_RIGHT) { //seta pra direita - aumenta X
         switch(cena){ //0 - observador 1 - alvo - 2 - orientacao
               case 0: camX+=5;
                    break;
               case 1: alvoX+=5;
                    break;
               case 2: cimaX = 1; 
                       cimaY=0;
                       cimaZ = 0;                      
                    break;
          }
          

    }

    if(key == GLUT_KEY_PAGE_UP) { //page UP - aumenta Z
         switch(cena){ //0 - observador 1 - alvo - 2 - orientacao
               case 0: camZ+=5;
                    break;
               case 1: alvoZ+=5;
                    break;
               case 2: cimaX = 0; 
                       cimaY=0;
                       cimaZ = 1;                      
                    break;
          }

    }

    if(key == GLUT_KEY_PAGE_DOWN) { //Page Down - diminui Z
         switch(cena){ //0 - observador 1 - alvo - 2 - orientacao
               case 0: camZ-=5;
                    break;
               case 1: alvoZ-=5;
                    break;
               case 2: cimaX = 0; 
                       cimaY=0;
                       cimaZ = 1;                      
                    break;
          } 

    }

    if(key == GLUT_KEY_HOME) { //Home - Muda para Observador
         cena=0;

    }

    if(key == GLUT_KEY_END) { //End - Muda para o alvo
         cena=1; 

    }

    if(key == GLUT_KEY_F10) { //Orientação em X
         cimaX=1; cimaY=0; cimaZ=0;

    }
    if(key == GLUT_KEY_F11) { //Orientação em Y
         cimaX=0; cimaY=1; cimaZ=0;

    }
    if(key == GLUT_KEY_F12) { //Orientação em Z
         cimaX=0; cimaY=0; cimaZ=1;

    }

     if(key == GLUT_KEY_F4) { //Habilita translação 
         rot = !rot;   

    }

    if(key == GLUT_KEY_F1) { //Habilita translação 
         transl = !transl;   

    }

    if(key == GLUT_KEY_F2) { //Zoom - 
         if (angulo <= 160) angulo += 5;   

    }

    if(key == GLUT_KEY_F3) { //Zoom +
         if (angulo >= 10) angulo -= 5;

    }

    EspecificaParametrosVisualizacao();
    glutPostRedisplay();
}
           
// Gerenciamento do menu com as opções de cores           
void MenuCor(int op)
{
   switch(op) {
            case 0:
                     r = 1.0f;
                     g = 0.0f;
                     b = 0.0f;
                     break;
            case 1:
                     r = 0.0f;
                     g = 1.0f;
                     b = 0.0f;
                     break;
            case 2:
                     r = 0.0f;
                     g = 0.0f;
                     b = 1.0f;
                     break;
    }
    glutPostRedisplay();
}           

// Gerenciamento do menu com as opções de visualização         
void MenuVisualizacao1(int op)
{
   tpVisualizacao = 0;
   switch(op) {
            case 0: angulo = 45;   zNear = 0.4; zFar=500;                
                     break;
            case 1: angulo = 90;  zNear = 0.5; zFar=1000;                
                     break;
            case 2: angulo = 120; zNear = 0.6; zFar=1500;
                     break;
    }
    
    EspecificaParametrosVisualizacao();
    glutPostRedisplay();
}   


void MenuVisualizacao2(int op)
{
   tpVisualizacao = 1;
   switch(op) {
            case 0: 
                     break;
            case 1:
                  //   primitiva = TRIANGULO;
                     break;
            case 2:
                   //  primitiva = LOSANGO;
                     break;
    }

    EspecificaParametrosVisualizacao();
    glutPostRedisplay();
} 
        
// Gerenciamento do menu principal           
void MenuPrincipal(int op)
{
}
              
// Criacao do Menu
void CriaMenu() 
{
    int menu,submenu1,submenu2, submenu3;

    submenu1 = glutCreateMenu(MenuCor);
    glutAddMenuEntry("Vermelho",0);
    glutAddMenuEntry("Verde",1);
    glutAddMenuEntry("Azul",2);

    submenu2 = glutCreateMenu(MenuVisualizacao1);
    glutAddMenuEntry("Perspectiva angulo = 45",0);
    glutAddMenuEntry("Perspectiva angulo = 90",1);
    glutAddMenuEntry("Perspectiva angulo = 120",2);

    submenu3 = glutCreateMenu(MenuVisualizacao2);
    glutAddMenuEntry("Paralela",0);
   // glutAddMenuEntry("Paralela",1);
   // glutAddMenuEntry("Paralela",2);

    menu = glutCreateMenu(MenuPrincipal);
    glutAddSubMenu("Cor",submenu1);
    glutAddSubMenu("Projecao Perspectiva",submenu2);
    glutAddSubMenu("Projecao Paralela",submenu3);
    
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}
           
// Função callback chamada para gerenciar eventos do mouse
void GerenciaMouse(int button, int state, int x, int y)
{        
     if (state == GLUT_DOWN) 
          CriaMenu();

   
	EspecificaParametrosVisualizacao();
	glutPostRedisplay(); 

}



// Inicializa parâmetros
void Inicializa (void)
{   
    // Define a cor de fundo da janela de visualização como preta
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

   // win=200.0f;
   
    r = 0.0f; //Cor inicial do objeto
    g = 0.0f;
    b = 1.0f;
    
    angulo=45;
   
    
    
    //Configurações de ilmuninação

     GLfloat luzAmbiente[4]={0.2,0.2,0.2,1.0}; 
	GLfloat luzDifusa[4]={0.7,0.7,0.7,1.0};	   // "cor" 
	GLfloat luzEspecular[4]={1.0, 1.0, 1.0, 1.0};// "brilho" 
	GLfloat posicaoLuz[4]={500.0, 500.0, -500.0, 1.0};
    

	// Capacidade de brilho do material
	GLfloat especularidade[4]={1.0,1.0,1.0,1.0}; 
	GLint especMaterial = 60;

     // Habilita o modelo de colorização de Gouraud
     glShadeModel(GL_SMOOTH);		

	// Define a refletância do material 
	glMaterialfv(GL_FRONT,GL_SPECULAR, especularidade);
     
	// Define a concentração do brilho
	glMateriali(GL_FRONT,GL_SHININESS,especMaterial);

     // Ativa o uso da luz ambiente 
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

	// Define os parâmetros da luz de número 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente); 
	glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa );
	glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular );
	glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz );

     
     // Habilita a definição da cor do material a partir da cor corrente
	glEnable(GL_COLOR_MATERIAL);
	//Habilita o uso de iluminação
	glEnable(GL_LIGHTING);  
	// Habilita a luz de número 0
	glEnable(GL_LIGHT0);
	// Habilita o depth-buffering
	glEnable(GL_DEPTH_TEST);


	glEnable ( GL_CULL_FACE);

    
}

// Programa Principal 
int main(int argc, char** argv)
{
     glutInit(&argc, argv);
     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);     
     glutInitWindowSize(1366,768); //Tela cheia
     //glutInitWindowSize(500, 480);
     glutInitWindowPosition(10,10);
     glutCreateWindow("Protecao de Tela []  R-T: Objeto | Rotacao (F4 Habilita): X, Y, Z (I-inverter) | Escala: A - D| Setas: Posicao da Cena  | S- solido | F1: Habilita Translacao | F2-F3: Zoom");
     glutKeyboardFunc(GerenciaTeclado);
     glutDisplayFunc(Desenha);
     glutReshapeFunc(AlteraTamanhoJanela);

     glutMouseFunc(GerenciaMouse);    
     glutSpecialFunc(TeclasEspeciais); 
     Inicializa();
     glutMainLoop();
}
