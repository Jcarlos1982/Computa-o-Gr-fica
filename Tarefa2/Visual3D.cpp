#include <GL/glut.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>
#include <vector>
#include <math.h>

#include <fstream> //para leitura de arquivos texto
using std::string;

using namespace std;

// Variáveis
int qtVertices=0, qtFaces=0; //usadas para criação dos vetores para guardar vertices e faces
int qtPropriedades =0;

char **fileList; //Variáveis para arquivos
char texto[100];



GLfloat win, r, g, b, angRot=0, angulo=0, fAspect,  escala=1, posX=0, posY=0, nx, ny, nz;
GLint view_w, view_h, obj=0, eixo=2;

//luzes da cena - inicial
GLfloat luzAmbiente[4]={0.2,0.2,0.2,1.0}; 
GLfloat luzDifusa[4]={0.7,0.7,0.7,1.0};	   // "cor" 
GLfloat luzEspecular[4]={1.0, 1.0, 1.0, 1.0};// "brilho" 
GLfloat posicaoLuz[4]={500.0, 500.0, -500.0, 1.0};


//posições da cena
// POSICAO INICIAL - gluLookAt(0,80,200, 0,0,0, 0,1,0);
GLdouble camX=0, camY=80, camZ=200, alvoX=0, alvoY=0, alvoZ=0, cimaX=0, cimaY=1, cimaZ=0, zNear=0.4;
GLint cena=0, zFar=500;


bool solido = false, transl=false, rot=false, luz0=true, luz1=true, malha2 = false;

//Estruturas para representar os dados dos arquivos ply

struct vertice{
    float x, y, z, nx, ny, nz ; //Temos objetos com mais propriedades porém trabalharemos apenas com estas
};

struct face{ //Triangulo
    vertice v[3]; //cada face tem 3 vértices, pois usaremos apenas triângulos
    float l1, l2, l3; //lados do triangulo
    float Area; //área do triângulo (Módulo do Determinante dividido por 2)
    float rAspecto; //razão de aspecto do trinnângulo (entre 0 e 1, sendo que mais próximo de 1 é melhor)


     //Função para calcular os lados do triangulo (raiz quadrada da soma dos quadrados da diferença)
     void calculaLados(face FC){
          l1 = abs(sqrt( pow(v[2].x - v[1].x, 2 ) + pow( v[2].y - v[1].y, 2 ) ) );
          l2 = abs(sqrt( pow(v[3].x - v[2].x, 2 ) + pow( v[3].y - v[2].y, 2 ) ) );
          l3 = abs(sqrt( pow(v[1].x - v[3].x, 2 ) + pow( v[1].y - v[3].y, 2 ) ) );
     }

     //Função para calcular a área do triangulo (face)
     void calculaArea(face FC){ //Determinante dos pontos divido por 2, em módulo
          
          Area = abs( ( ( v[3].x * v[2].y * 1) + ( v[3].y * 1 * v[1].x ) + ( 1 * v[2].x * v[1].y )  
                 - ( v[1].x * v[2].y * 1) + ( v[1].y * 1 * v[3].x ) + ( 1 * v[2].x * v[3].y ) ) / 2);
     }

     //Função para calcular a razão de aspecto do triangulo (face)
     void calculaAspecto(face FC){ //Prof. Leandro (4 x raiz de 3 x área / pela soma dos quadrados dos lados)

          rAspecto = (4 * sqrt(3 * Area) ) / (l1*l1 + l2*l2 + l3*l3);
    
     }

};

 //Cria os vetores com os tamanhos dos dados de vertices e faces lidas
 vector<vertice> v, v2 ; //v2 será usado para receber os vértices da nova triangulacao
 vector<face> f, f2 ; //f2 será usado para receber as faces da nova triangulacao




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

//Calcula o vetor normal de uma face - CORRIGIR AQUI
void calculaNormal( face FC )
{
   /* calculate Vector1 and Vector2 */
   float va[3], vb[3], vr[3], val;
   va[0] = FC.v[0].x - FC.v[1].x;
   va[1] = FC.v[0].y - FC.v[1].y;
   va[2] = FC.v[0].z - FC.v[1].z;
 
   vb[0] = FC.v[0].x - FC.v[2].x;
   vb[1] = FC.v[0].y - FC.v[2].y;
   vb[2] = FC.v[0].z - FC.v[2].z;
 
   /* cross product */
   vr[0] = va[1] * vb[2] - vb[1] * va[2];
   vr[1] = vb[0] * va[2] - va[0] * vb[2];
   vr[2] = va[0] * vb[1] - vb[0] * va[1];
 
   /* normalization factor */
   val = sqrt( vr[0]*vr[0] + vr[1]*vr[1] + vr[2]*vr[2] );
 
	
	nx = vr[0]/val;
	ny = vr[1]/val;
	nz = vr[2]/val;
 
}


// Função usada para especificar o volume de visualização
void EspecificaParametrosVisualizacao(void)
{
     
	// Especifica sistema de coordenadas de projeção
         
	glMatrixMode(GL_PROJECTION);
     glOrtho(-1000.0, 1000.0, -1000.0, 1000.0, -1000.0, 1000.0); 
	glLoadIdentity();
     
     

     // Especifica a projeção perspectiva
     gluPerspective(angulo,fAspect,zNear,zFar);

          // Especifica posição do observador e do alvo
     gluLookAt(camX,camY,camZ, alvoX, alvoY,alvoZ, cimaX,cimaY,cimaZ);

    
     
     aplicaEscala(escala);

     if(transl){ //se a translação estiver habilitada
         glTranslatef(posX, posY, 0.0); 
        
     }

     if (rot){ //se a rotação estiver habilitada
          aplicaRotacao(angRot, eixo);
     }

      
     
      
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();   

     

     	
}

// Função callback chamada para fazer o desenho
void Desenha()
{   

     EspecificaParametrosVisualizacao();

     glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT);
     
	
          for (int i=0; i< qtFaces; i++){

               calculaNormal(f[i]); //Calcula os vetores normais de cada face

               //guarda os resultados dos normais
               f[i].v[0].nx = nx; f[i].v[0].ny = ny; f[i].v[0].nz = nz; 
               f[i].v[1].nx = nx; f[i].v[1].ny = ny; f[i].v[1].nz = nz; 
               f[i].v[2].nx = nx; f[i].v[2].ny = ny; f[i].v[2].nz = nz; 
               
               

               glColor3f(r, g, b); //cor da primeira malha
               
               if (solido)
                    glBegin(GL_POLYGON);	//sólido
               else
                    glBegin(GL_LINE_STRIP);		//malha
                    

                         glNormal3f(f[i].v[0].nx, f[i].v[0].ny, f[i].v[0].nz);
                         glVertex3f(f[i].v[0].x, f[i].v[0].y, f[i].v[0].z);

                         glNormal3f(f[i].v[1].nx, f[i].v[1].ny, f[i].v[1].nz);
                         glVertex3f(f[i].v[1].x, f[i].v[1].y, f[i].v[1].z);
                         
                         glNormal3f(f[i].v[2].nx, f[i].v[2].ny, f[i].v[2].nz);
                         glVertex3f(f[i].v[2].x, f[i].v[2].y, f[i].v[2].z);  
                  
               glEnd();

               if (malha2){ //malha 2 de outra cor - OBS. ESSA MALHA SERÁ O RESULTADO MELHORADO
                              glColor3f(1.0, 1.0, 0.0);
                              glBegin(GL_LINE_STRIP);		//malha 2
                              
                                   float ft = 0.01;

                                   glNormal3f(f[i].v[0].nx + ft, f[i].v[0].ny + ft, f[i].v[0].nz + ft);
                                   glVertex3f(f[i].v[0].x, f[i].v[0].y, f[i].v[0].z);

                                   glNormal3f(f[i].v[1].nx + ft, f[i].v[1].ny + ft, f[i].v[1].nz + ft);
                                   glVertex3f(f[i].v[1].x, f[i].v[1].y, f[i].v[1].z);
                                   
                                   glNormal3f(f[i].v[2].nx + ft, f[i].v[2].ny + ft, f[i].v[2].nz + ft);
                                   glVertex3f(f[i].v[2].x, f[i].v[2].y, f[i].v[2].z);

                             glEnd();
                         }
                    
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
          
          case 'S':
          case 's': //Sólido ou não
               {
                    solido = !solido;

                    break;
               }    

          case 'M':
          case 'm': //Malha 2 ou não
               {
                    malha2 = !malha2;

                    break;
               }  

          //Translação
          case '4':{ //Esquerda
               if (transl){
                    posX--;                  
               }  
               break;   
          }     
          case '6':{ //Direita
               if (transl){
                    posX++;
               }   
               break;  
          }   
          case '2':{ //Para baixo
               if (transl){
                    posY--;
               }   
               break;  
          }   
          case '8':{ //Para cima
               if (transl){
                    posY++;
               }     
               break;
          }  
          //diagonais 
          case '1':{ //Esquerda-baixo
               if (transl){
                    posX--; posY--;
               }  
               break;   
          }   
          case '7':{ //Esquerda-cima
               if (transl){
                    posX--; posY++;
               }   
               break;  
          }   
          case '3':{ //Direta-baixo
               if (transl){
                    posX++; posY--;
               }   
               break;  
          }   
          case '9':{ //Direita-cima
               if (transl){
                    posX++; posY++;
               }   
               break;  
          }                       
                                   
    }

    
    //EspecificaParametrosVisualizacao();
    glutPostRedisplay();
}


// Função callback chamada para gerenciar eventos do teclado   
// para teclas especiais, tais como F1, PgDn e Home
void TeclasEspeciais(int key, int x, int y)  
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

   
     if(key == GLUT_KEY_F1) { //Habilita rotação
          
         rot = !rot;   

    }


     if(key == GLUT_KEY_F4) { //Habilita translação
          transl = !transl; 

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

   

   

    EspecificaParametrosVisualizacao();
    glutPostRedisplay();
}
           
// Gerenciamento do menu com as opções de cores           
void MenuCorObjeto(int op)
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

// Gerenciamento do menu com as opções de cor da iluminacção       
void MenuIluminacaoLuz(int op)
{
   switch(op) {
            case 0: { 
                      luz0 = !luz0;	   // habilita e desabilita luz 0         
                      break;
            }
            case 1: { 
                      luz1 = !luz1;	   // habilita e desabilita luz 1         
                      break;
            }
     }
     

              if (luz0) {   // Habilita a luz de número 0
                 glEnable(GL_LIGHT0);
               }else{
                   glDisable(GL_LIGHT0); 
               }

               if (luz1) {  // Habilita a luz de número 0
                 glEnable(GL_LIGHT1);
               }else{
                   glDisable(GL_LIGHT1); 
               }
                    
   
    
    EspecificaParametrosVisualizacao();
    glutPostRedisplay();
}  

// Gerenciamento do menu com as opções de cor da iluminacção       
void MenuIluminacaoCor(int op)
{
   switch(op) {
            case 0: { //Branca
                      luzDifusa[0]=1.0;	 luzDifusa[1]=1.0; luzDifusa[2]=1.0; luzDifusa[3]=1.0;  //cor
                      luzEspecular[0]=1.0; luzEspecular[1]=1.0; luzEspecular[2]=1.0; luzEspecular[3]=1.0; // "brilho" 	           
                      break;
            }
            case 1: { //Amarela
                      luzDifusa[0]=1.0;	 luzDifusa[1]=1.0; luzDifusa[2]=0.0; luzDifusa[3]=1.0;  //cor
                      luzEspecular[0]=1.0; luzEspecular[1]=1.0; luzEspecular[2]=0.0; luzEspecular[3]=1.0; // "brilho" 	           
                      break;
            }
            case 2: { //Roxa
                      luzDifusa[0]=1.0;	 luzDifusa[1]=0.0; luzDifusa[2]=1.0; luzDifusa[3]=0.0;  //cor
                      luzEspecular[0]=1.0; luzEspecular[1]=0.0; luzEspecular[2]=1.0; luzEspecular[3]=0.0; // "brilho" 	                  
                      break;
            }

            
    }

    //muda somente na luz habilita
     if (luz0){
          // Define os parâmetros da luz de número 0
          glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa );
          glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular );
          glEnable(GL_LIGHT0);
         

     }
     if (luz1){
          // Define os parâmetros da luz de número 1
          glLightfv(GL_LIGHT1, GL_DIFFUSE, luzDifusa );
          glLightfv(GL_LIGHT1, GL_SPECULAR, luzEspecular );
          glEnable(GL_LIGHT1);

     }
    
    EspecificaParametrosVisualizacao();
    glutPostRedisplay();
}   

// Gerenciamento do menu com as opções de cor da iluminacção       
void MenuIluminacaoPosicao(int op)
{
   switch(op) {
            case 0: {    
                      posicaoLuz[0]=0.0; posicaoLuz[1]=500.0; posicaoLuz[2]= 500.0; posicaoLuz[3]=1.0;         
                      break;
            }
            case 1: {    
                      posicaoLuz[0]=500.0; posicaoLuz[1]=500.0; posicaoLuz[2]=00.0; posicaoLuz[3]=1.0;          
                      break;
            }
            case 2: {    
                      posicaoLuz[0]=00.0; posicaoLuz[1]=-500.0; posicaoLuz[2]= 500.0; posicaoLuz[3]=1.0;         
                      break;
            }
            case 3: {    
                      posicaoLuz[0]=500.0; posicaoLuz[1]=-500.0; posicaoLuz[2]=00.0; posicaoLuz[3]=1.0;          
                      break;
            }
    }

     //muda somente na luz habilita
     if (luz0){
          // Define os parâmetros da luz de número 0
          glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz );
          glEnable(GL_LIGHT0);
         

     }
     if (luz1){
          // Define os parâmetros da luz de número 1
          glLightfv(GL_LIGHT1, GL_POSITION, posicaoLuz );
          glEnable(GL_LIGHT1);

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
    int menu,submenu1,submenu2, submenu3, submenu4;

    submenu1 = glutCreateMenu(MenuCorObjeto); //Cor do objeto
    glutAddMenuEntry("Vermelho",0);
    glutAddMenuEntry("Verde",1);
    glutAddMenuEntry("Azul",2);

    submenu2 = glutCreateMenu(MenuIluminacaoLuz); //Habilita e desabilita luzes da cena
    glutAddMenuEntry("Liga/Desliga Luz 1",0);
    glutAddMenuEntry("Liga/Desliga Luz 2",1);
    

    submenu3 = glutCreateMenu(MenuIluminacaoCor); //Cor das luzes da cena
    glutAddMenuEntry("Branca",0);
    glutAddMenuEntry("Amarela",1);
    glutAddMenuEntry("Roxa",2);

    submenu4 = glutCreateMenu(MenuIluminacaoPosicao); //Posição das luzes da cena
    glutAddMenuEntry("Superior esquerda",0);
    glutAddMenuEntry("Superior direita",1);
    glutAddMenuEntry("Inferior Esquerda",2);
    glutAddMenuEntry("Inferior Direta",3);


    menu = glutCreateMenu(MenuPrincipal);
    glutAddSubMenu("Cor do Objeto",submenu1);
    glutAddSubMenu("Luzes da Cena",submenu2);
    glutAddSubMenu("Cores das Luzes",submenu3);
    glutAddSubMenu("Posicao das Luzes",submenu4);


   //criar menu para a iluminacao com opções para mudar a posição e a cor das luzes
    
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
     
    // glOrtho(-1000.0, 1000.0, -1000.0, 1000.0, -1000.0, 1000.0);  

   // win=200.0f;
   
    r = 0.0f; //Cor inicial do objeto
    g = 0.0f;
    b = 1.0f;
    
    angulo=5;
   
    
    
    //Configurações de ilmuninação   

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

     //muda a cor , o brilho e a posição para a luz 1
	luzDifusa[0]=0.5;	 luzDifusa[1]=0.5; luzDifusa[2]=0.5; luzDifusa[3]=1.0;  //cor
     luzEspecular[0]=1.0; luzEspecular[1]=1.0; luzEspecular[2]=1.0; luzEspecular[3]=1.0; // "brilho" 
     posicaoLuz[0]=-500.0; posicaoLuz[1]=-500.0; posicaoLuz[2]= 500.0; posicaoLuz[3]=1.0;

     // Define os parâmetros da luz de número 1
	glLightfv(GL_LIGHT1, GL_AMBIENT, luzAmbiente); 
	glLightfv(GL_LIGHT1, GL_DIFFUSE, luzDifusa );
	glLightfv(GL_LIGHT1, GL_SPECULAR, luzEspecular );
	glLightfv(GL_LIGHT1, GL_POSITION, posicaoLuz );

     
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

//Faz a leitura das strings do arquivo
void lerDado(FILE *arq){
          //Lê um dado do arquivo e retorna em formato de string

          fscanf(arq, "%s",  texto);
}

// Programa Principal 
int main(int argc, char** argv)
{
     //Prepara a estrutura com os dados de interesse (vertices, faces, pontos ....)
     fileList = argv+1;  // salta nome do programa
   
		
		// Abrindo o arquivo
		
          FILE *arq;
          arq = fopen(fileList[0], "rb");
          if(arq == NULL)
               cout<<"Erro, nao foi possivel abrir o arquivo "<<fileList[0] <<endl;
          else{
               //Faz a leitura das linhas do arquivo
               
               while (true){ 
                    lerDado(arq);
                    if (!strcmp( texto ,"property")){
                         qtPropriedades++;
                    }
                    if (!strcmp( texto ,"vertex")){
                        lerDado(arq); //pega a quantidade de vértices
                        qtVertices = atoi(texto);
                    }
                    if (!strcmp( texto ,"face") ) {
                        lerDado(arq); //pega a quanntidade de faces
                        qtFaces = atoi(texto);    
                    }
                    
                    if (!strcmp( texto ,"end_header")){ //pára quando encontrar end_header
                        break;
                    }
                    
  
               }
               qtPropriedades--; //ajusta a quantidade de propriedades
               

               cout<<"Propriedades lidas "<<qtPropriedades<<endl;
               cout<<"Vertices lidos: "<<qtVertices<<endl;
               cout<<"Faces lidas: "<<qtFaces<<endl;  
      

          //preenche o vetor de vertices
          for (int i=0; i< qtVertices; i++){
               vertice vt;
               
               if ( (qtPropriedades) == 3){
                    lerDado(arq); vt.x = atof(texto);
                    lerDado(arq); vt.y = atof(texto);
                    lerDado(arq); vt.z = atof(texto); 
               }

          

               
               // outras propriedades
               
               if ( (qtPropriedades) > 3){
                   switch (qtPropriedades){
                        case 4: {
                             lerDado(arq); vt.x = atof(texto); 
                             lerDado(arq); vt.y = atof(texto);
                             lerDado(arq); vt.z = atof(texto);
                             lerDado(arq); vt.nx = atof(texto); 
                             break;
                        }
                        case 5: {
                             lerDado(arq); vt.x = atof(texto); 
                             lerDado(arq); vt.y = atof(texto);
                             lerDado(arq); vt.z = atof(texto);
                             lerDado(arq);  vt.nx = atof(texto);
                             lerDado(arq); vt.ny = atof(texto);
                             break;
                        }
                        case 6: { //Se tiver 6 propriedades teremos os dados do vetor normal
                             lerDado(arq); vt.x = atof(texto); 
                             lerDado(arq); vt.y = atof(texto);
                             lerDado(arq); vt.z = atof(texto);
                             lerDado(arq); vt.nx = atof(texto);
                             lerDado(arq); vt.ny = atof(texto);
                             lerDado(arq); vt.nz = atof(texto);
                             break;
                        }
                   }

               }  

               //cout<<"Verice 1: X "<< vt.x << vt.y<<  vt.z<<endl;
               v.push_back(vt);

          }

          //preenche o vetor de faces
          for (int i=0; i< qtFaces; i++){
               face fc;
               lerDado(arq); //quantidade de lados, não interessa pois sempre trataremos triângulos
               lerDado(arq); fc.v[0] = v[atoi(texto)]; 
               lerDado(arq); fc.v[1] = v[atoi(texto)];
               lerDado(arq); fc.v[2] = v[atoi(texto)];
               f.push_back(fc);
               
          }

          //cout<<"Verice : X = "<<f[0].v[0].x <<" Y = "<<f[0].v[0].y<< " Z = "<<f[0].v[0].z<< endl;

		
	}

     glutInit(&argc, argv);
     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);     
     glutInitWindowSize(1366,768); //Tela cheia

    // glutInitWindowPosition(10,10);
     glutCreateWindow("Leitor 3D.ply []  | Rotacao (F1 Habilita): X, Y, Z (I-inverter) | Escala: A - D| Setas: Posicao da Cena  | S- solido | F4: Habilita Translacao (NUM)");
     glutKeyboardFunc(GerenciaTeclado);
     glutReshapeFunc(AlteraTamanhoJanela);

     glutDisplayFunc(Desenha);
     glutMouseFunc(GerenciaMouse);    
     glutSpecialFunc(TeclasEspeciais); 

     Inicializa();
     glutMainLoop();
}
