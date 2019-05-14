#include <GL/glut.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

// Constantes
#define QUADRADO 1
#define TRIANGULO 2
#define LOSANGO   3

// Variáveis
char texto[30];
GLfloat win, r, g, b, vX, vY, movX, movY, tam, acel;
GLint view_w, view_h, primitiva, altura, comprimento;

bool automatico = false;


// Função que desenha um quadrado
void DesenhaQuadrado(void)
{
  
     glBegin(GL_QUADS);
               glVertex2f(vX, vY+tam);
               glVertex2f(vX, vY);
               glVertex2f(vX+tam, vY);
               glVertex2f(vX+tam, vY+tam);               
     glEnd();
     
}

// Função que desenha um triângulo
void DesenhaTriangulo(void)
{
     glBegin(GL_TRIANGLES);
            
               glVertex2f(vX+tam, vY+tam);
               glVertex2f(vX, vY);
               glVertex2f(vX+2*tam, vY);             
     glEnd();
}

// Função que desenha um losango
void DesenhaLosango(void)
{
    
     glBegin(GL_POLYGON);  
               glVertex2f(vX+tam, vY);
               glVertex2f(vX+2*tam, vY+tam/2);
               glVertex2f(vX+tam, vY+tam);
               glVertex2f(vX, vY+tam/2);               
     glEnd();
}

// Desenha um texto na janela GLUT
void DesenhaTexto(char *string) 
{  
  	glPushMatrix();
        // Posição no universo onde o texto será colocado          
        glRasterPos2f(-win,win-(win*0.08)); 
        // Exibe caracter a caracter
        while(*string)
             glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10,*string++); 
	glPopMatrix();
}

// Função callback chamada para fazer o desenho
void Desenha(void)
{
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
                   
     glClear(GL_COLOR_BUFFER_BIT);
     
     // Define a cor corrente
     glColor3f(r,g,b);

     // Desenha uma primitiva     
     switch (primitiva) {
            case QUADRADO:  DesenhaQuadrado();
                            break;
            case TRIANGULO: DesenhaTriangulo();                          
                            break;
            case LOSANGO:   DesenhaLosango();                       
                            break;
     }

     // Exibe a posição do mouse na janela
     glColor3f(1.0f,1.0f,1.0f);
     DesenhaTexto(texto);
     
     glutSwapBuffers();
}



// Função callback chamada quando o tamanho da janela é alterado 
void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{ 
    // Especifica as dimensões da Viewport
    glViewport(0, 0, w, h);
    view_w = w;
    view_h = h;                   

    // Inicializa o sistema de coordenadas
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D (0.0f, view_w, 0.0f, view_h);
}

// Função callback chamada sempre que o mouse é movimentado
// sobre a janela GLUT com um botão pressionado
void MoveMouseBotaoPressionado(int x, int y)
{
     sprintf(texto, "Botao pressionado (%d,%d)", x, y);
     glutPostRedisplay();
}

// Função callback chamada sempre que o mouse é movimentado
// sobre a janela GLUT 
void MoveMouse(int x, int y)
{
     sprintf(texto, "(%d,%d)", x, y);
     glutPostRedisplay();
}

// Função callback chamada para gerenciar eventos do teclado   
// para teclas especiais, tais como F1, PgDn e Home
void TeclasEspeciais(int key, int x, int y) //Aumenta e diminui a velocidade do objeto 
{
    if(key == GLUT_KEY_UP) { //seta pra cima
          
               movX *= acel;
               movY *= acel; 
             
          }
       
    if(key == GLUT_KEY_DOWN) { //seta pra baixo
         
          movX /= acel;
          movY /= acel; 
        

    }

      if(key == GLUT_KEY_LEFT) { //seta pra esquerda
          automatico = false;

    }

     if(key == GLUT_KEY_RIGHT) { //seta pra direita
          automatico = true;

    }


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

// Gerenciamento do menu com as opções de cores           
void MenuPrimitiva(int op)
{
   switch(op) {
            case 0:
                     primitiva = QUADRADO;
                     break;
            case 1:
                     primitiva = TRIANGULO;
                     break;
            case 2:
                     primitiva = LOSANGO;
                     break;
    }
    glutPostRedisplay();
}   
        
// Gerenciamento do menu principal           
void MenuPrincipal(int op)
{
}
              
// Criacao do Menu
void CriaMenu() 
{
    int menu,submenu1,submenu2;

    submenu1 = glutCreateMenu(MenuCor);
    glutAddMenuEntry("Vermelho",0);
    glutAddMenuEntry("Verde",1);
    glutAddMenuEntry("Azul",2);

    submenu2 = glutCreateMenu(MenuPrimitiva);
    glutAddMenuEntry("Quadrado",0);
    glutAddMenuEntry("Triângulo",1);
    glutAddMenuEntry("Losango",2);

    menu = glutCreateMenu(MenuPrincipal);
    glutAddSubMenu("Cor",submenu1);
    glutAddSubMenu("Primitivas",submenu2);
    
    glutAttachMenu(GLUT_LEFT_BUTTON);
}
           
// Função callback chamada para gerenciar eventos do mouse
void GerenciaMouse(int button, int state, int x, int y)
{        
    if (button == GLUT_LEFT_BUTTON)
         if (state == GLUT_DOWN) 
            CriaMenu();
         
    glutPostRedisplay();
}

// Função callback chamada pela GLUT a cada intervalo de tempo
void Timer(int value)
{
     switch(primitiva){ //Objetos diferentes tem que tratar altura e comprimento para detectar as bordas
          case QUADRADO: 
               
               // Muda a direção quando chega na borda esquerda ou direita
               if(vX > view_w - tam|| vX < 0){
                    movX = -movX+0.3;  
                    if (automatico){
                          primitiva =TRIANGULO;
                          MenuCor(rand()%3);
                        }
                         
               }
               // Muda a direção quando chega na borda superior ou inferior
               if(vY > view_h -tam|| vY < 0){
                    movY = -movY+0.5;  
                    if (automatico){
                          primitiva = TRIANGULO;
                          MenuCor(rand()%3);
                        }      
                                  
               }
                         
                         
               // Verifica as bordas. Se o objeto sair da janela 
               if(vX > view_w - tam)
                    vX = view_w - tam-1;

               if(vY > view_h - tam)
                    vY = view_h - tam-1;

                    
               break;
          case TRIANGULO: 
               
                altura=tam; comprimento=2*tam;
               // Muda a direção quando chega na borda esquerda ou direita
                if(vX > view_w - comprimento|| vX < 0){
                    movX = -movX+0.3;
                    if (automatico){
                          primitiva = LOSANGO;
                          MenuCor(rand()%3);
                        }
               }

               // Muda a direção quando chega na borda superior ou inferior
               if(vY > view_h -tam|| vY < 0){
                    movY = -movY+0.5;  
                    if (automatico){
                          primitiva = LOSANGO;
                          MenuCor(rand()%3);
                        }
               }
                         
                         
             // Verifica as bordas. Se o objeto sair da janela 
               if(vX > view_w - tam)
                    vX = view_w - tam-1;

               if(vY > view_h - tam)
                    vY = view_h - tam-1;

                    
               break;
          case LOSANGO:
               
                altura=tam; comprimento=2*tam;
                //Muda a direção quando chega na borda esquerda ou direita
                 if(vX > view_w - comprimento|| vX < 0){
                        movX = -movX+0.3;    
                        if (automatico){
                             primitiva = QUADRADO;
                          MenuCor(rand()%3);
                        }
                          
                 }
               // Muda a direção quando chega na borda superior ou inferior
               
               if(vY > view_h -altura|| vY < 0){
                    movY = -movY+0.5;     
                    if (automatico){
                          primitiva = QUADRADO;
                          MenuCor(rand()%3);
                        }
               }
                         
                         
              // Verifica as bordas. Se o objeto sair da janela 
               if(vX > view_w - comprimento)
                    vX = view_w - comprimento;

               if(vY > view_h - altura)
                    vY = view_h - altura-1;   

               break;
          
     }

    // Move o objeto
    vX += movX;
    vY += movY;

    // Redesenha o objeto com as novas coordenadas 
    glutPostRedisplay();
    glutTimerFunc(33,Timer, 1);
}

// Inicializa parâmetros
void Inicializa (void)
{   
    // Define a cor de fundo da janela de visualização como preta
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    win=200.0f;
    primitiva = QUADRADO;
    r = 0.0f; //Cor inicial do objeto
    g = 0.0f;
    b = 1.0f;
    vX = 0.0f; //posição inicial do objeto
    vY = 0.0f;
    movX = 3.0f; //tamanho do movimento
    movY = 3.0f;
    acel = 1.2; //Aceleração
    tam = 100.0; //tamanho do objeto

    strcpy(texto, "(0,0)");
}

// Programa Principal 
int main(int argc, char** argv)
{
     glutInit(&argc, argv);
     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);     
     glutInitWindowSize(1366,768);
     glutInitWindowPosition(10,10);
     glutCreateWindow("Protecao de Tela");
     glutDisplayFunc(Desenha);
     glutReshapeFunc(AlteraTamanhoJanela);
     glutMotionFunc(MoveMouseBotaoPressionado); 
     glutPassiveMotionFunc(MoveMouse);
     glutMouseFunc(GerenciaMouse);    
     glutSpecialFunc(TeclasEspeciais); 
     glutTimerFunc(33, Timer, 1);
     Inicializa();
     glutMainLoop();
}
