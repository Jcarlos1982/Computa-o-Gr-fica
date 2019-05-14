// Quadrado.c - Isabel H. Manssour
// Um programa OpenGL simples que desenha um 
// quadrado em  uma janela GLUT.
// Este código está baseado no GLRect.c, exemplo 
// disponível no livro "OpenGL SuperBible", 
// 2nd Edition, de Richard S. e Wright Jr.

#include <GL/glut.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

// Variáveis
GLfloat win, Fundo[3], Quad[3];
GLint BTMouse;

// Função callback chamada para fazer o desenho
void Desenha(void)
{
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
                   
     // Limpa a janela de visualização com a cor de fundo especificada
     glClear(GL_COLOR_BUFFER_BIT);
   
    
  
     // Desenha um quadrado preenchido com a cor corrente
     glBegin(GL_QUADS);  
               glColor3fv(Quad);    
               glVertex2i(100,150);
               glVertex2i(100,100);
               glVertex2i(150,100);
               glVertex2i(150,150);               
     glEnd();

     // Executa os comandos OpenGL
     glFlush();
}

// Função callback chamada para gerenciar eventos de teclado
void GerenciaTeclado(unsigned char key, int x, int y)
{
    switch (key) {
            case 'R': 
            case 'r':// muda a cor corrente para vermelho
                     Quad[0] = 1.0f;
                     Quad[1] = 0.0f;
                     Quad[2] = 0.0f;
                     break;
            case 'G':
            case 'g':// muda a cor corrente para verde
                     Quad[0] = 0.0f;
                     Quad[1] = 1.0f;
                     Quad[2] = 0.0f;
                     break;
            case 'B':
            case 'b':// muda a cor corrente para azul
                     Quad[0] = 0.0f;
                     Quad[1] = 0.0f;
                     Quad[2] = 1.0f;
                     break;
           case 'C':
           case 'c':// muda a cor corrente para ciano
                     Quad[0] = 0.0f;
                     Quad[1] = 1.0f;
                     Quad[2] = 1.0f;
                     break;
          case 'M':
          case 'm':// muda a cor corrente para Magenta
                     Quad[0] = 1.0f;
                     Quad[1] = 0.0f;
                     Quad[2] = 1.0f;
                     break;    
          case 'Y':
          case 'y':// muda a cor corrente para amarelo
                     Quad[0] = 1.0f;
                     Quad[1] = 1.0f;
                     Quad[2] = 0.0f;
                     break;                              
    }
    glutPostRedisplay();
}
    

// Função callback chamada para gerenciar eventos do mouse
//Muda cor ao clicar no mouse
void GerenciaMouse(int button, int state, int x, int y)
{        
    if (button == GLUT_LEFT_BUTTON)
         if (state == GLUT_DOWN) {
                Fundo[0] = (double)(rand()%99)/100; //cout<<"Cor Fundo \n"<<Fundo[0]<<"\n";
                Fundo[1] = (double)(rand()%99)/100; //cout<<Fundo[1]<<"\n";
                Fundo[2] = (double)(rand()%99)/100; //cout<<Fundo[2]<<"\n---\n";

                glClearColor(Fundo[0], Fundo[1], Fundo[2], 1.0f);

         }

     if (button == GLUT_RIGHT_BUTTON)
         if (state == GLUT_DOWN) {
                Quad[0] = (double)(rand()%99)/100; //cout<<"Cor Objeto \n"<<Quad[0]<<"\n";
                Quad[1] = (double)(rand()%99)/100; //cout<<Quad[1]<<"\n";
                Quad[2] = (double)(rand()%99)/100; //cout<<Quad[2]<<"\n---\n";
          
                glColor3f(Quad[0],Quad[1],Quad[2]);        
         }    
            
         
    glutPostRedisplay();
}


// Inicializa parâmetros de rendering
void Inicializa (void)
{   
    // Define a cor de fundo da janela de visualização como preta
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    //Começa com um quadrado branco
    Quad[0]=1.0f;
    Quad[1]=1.0f;
    Quad[2]=1.0f;
    
}

// Função callback chamada quando o tamanho da janela é alterado 
void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{
                   // Evita a divisao por zero
                   if(h == 0) h = 1;
                           
                   // Especifica as dimensões da Viewport
                   glViewport(0, 0, w, h);

                   // Inicializa o sistema de coordenadas
                   glMatrixMode(GL_PROJECTION);
                   glLoadIdentity();

                   // Estabelece a janela de seleção (left, right, bottom, top)
                   if (w <= h) 
                           gluOrtho2D (0.0f, 250.0f, 0.0f, 250.0f*h/w);
                   else 
                           gluOrtho2D (0.0f, 250.0f*w/h, 0.0f, 250.0f);
}

// Programa Principal 
int main(int argc, char** argv)
{
     glutInit(&argc, argv);
     glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
     glutInitWindowSize(400,350);
     glutInitWindowPosition(10,10);
     glutCreateWindow("Quadrado");
     glutMouseFunc(GerenciaMouse); 
     glutKeyboardFunc(GerenciaTeclado);
     glutDisplayFunc(Desenha);   
     glutReshapeFunc(AlteraTamanhoJanela);
     Inicializa();
     glutMainLoop();
}
