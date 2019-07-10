#include "Lab7-8.h"

void main(){
	int i;
    Arbol *arbol = iniciaArbolVacio();

	for(i = 0; i < 200; i++){
		arbol = insertaClaveArbolB(arbol,i);
	}
}
