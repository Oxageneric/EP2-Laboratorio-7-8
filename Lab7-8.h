#include <stdio.h>
#include <stdlib.h>
#define T 80

typedef struct  un_Nodo{
       int claves[2*T];
       int hijos[2*T+1];
       int cantClaves;
       int esHoja;
       int miNombre;
       }Nodo;
typedef Nodo Arbol;
//DISK-WRITE
int escrituraDisco(Nodo *nodo){
    char miNombre[50];
    sprintf(miNombre,"%i.b",nodo->miNombre);

    FILE *archivo;
    archivo=fopen(miNombre,"wb");
    if(archivo==NULL){
        return 0;
    }else{//Almacenamiento Persistente del Nodo
        fwrite(nodo,sizeof(Nodo),1,archivo);
        fclose(archivo);
        return 1;
    }
}
//DISK-READ
Nodo *lecturaDisco(int nombreNodo){
    char nombreArchivo[50];
    sprintf(nombreArchivo,"%i.b",nombreNodo);
    FILE *Archivo;
	Archivo = fopen(nombreArchivo,"rb");
    if(Archivo==NULL){
        return NULL;
    }
    Nodo *nuevoNodo;
    nuevoNodo=(Nodo*)malloc(sizeof(Nodo));
    fread(nuevoNodo,sizeof(Nodo),1,Archivo);
	printf("dato del nodo %i:%i\n",nuevoNodo->miNombre, nuevoNodo->cantClaves);
    fclose(Archivo);
    return (nuevoNodo);
}

Nodo *iniciaArbolVacio(){
	Nodo *unNodo;
	static int nombreNodo = 0; //nombre del archivo.b
    unNodo = (Nodo *)malloc(sizeof(Nodo));
    unNodo->cantClaves = 0;
    unNodo->esHoja = 1;
    unNodo->miNombre = nombreNodo;
	nombreNodo++;
    return(unNodo);
}
//B-TREE-SEARCH Pag.492 Cormen.
Nodo *BuscaArbolB(Nodo *padre, int dato) {
	int i=1;
	while (i<=padre->cantClaves && dato>padre->claves[i]){
		i++;
	}
	if (i <= padre->cantClaves && dato == padre->claves[i]){
		printf("Encontrado: %i en Archivo %i.b\n",dato,padre->miNombre);
        return (padre);
	}else if(padre->esHoja == 1){
        return NULL;
	}else{
        Nodo *hijoPredecesor;
        hijoPredecesor = lecturaDisco(padre->hijos[i]);
        BuscaArbolB(hijoPredecesor,dato);
    }
}
//B-TREE-SPLIT-CHILD Pag.494 Cormen.
int divideNodo(Nodo *Padre, int indiceHijolleno){
	int i;
	Nodo *hijoaDividir;
	Nodo *hermano;
	hermano = iniciaArbolVacio();

	hijoaDividir = lecturaDisco(Padre->hijos[indiceHijolleno]);
	hermano->esHoja = hijoaDividir->esHoja;
	hermano->cantClaves = T-1;
	for(i=1; i<= (T-1); i++){
    	hermano->claves[i]=hijoaDividir->claves[i+T];
	}
	if(hijoaDividir->esHoja == 0){
    	for(i=1;i<=T;i++){
        	hermano->hijos[i]=hijoaDividir->hijos[i+T];
        }
	}

	hijoaDividir->cantClaves=(T-1);//Apunta a la mitad izquierda
	//Mueve los hijos del Padre un espacio mas a la derecha
	for(i=Padre->cantClaves+1 ; i>(indiceHijolleno+1) ; i--){
    	Padre->hijos[i+1]=Padre->hijos[i];
	}
	Padre->hijos[indiceHijolleno+1]=hermano->miNombre;
	//Mueve las claves del Padre un espacio mas a la derecha
	for(i=Padre->cantClaves; i>(indiceHijolleno) ; i--){
    	Padre->claves[i+1]=Padre->claves[i];
	}
	Padre->claves[indiceHijolleno] = hijoaDividir->claves[T];
	Padre->cantClaves = Padre->cantClaves+1;
	Padre->esHoja = 0;
	escrituraDisco(hijoaDividir);
	escrituraDisco(hermano);
	escrituraDisco(Padre);
}
//B-TREE-INSERT NONFULL Pag.496 Cormen.
int insertarNoLleno(Nodo *Padre, int clave){
    int i = Padre->cantClaves;
    int j;
    Nodo *hijoNodo;
    if(Padre->esHoja == 1){
    	while(i>=1 && clave<Padre->claves[i]){
        	Padre->claves[i+1] = Padre->claves[i];
            i--;
        }
    	Padre->claves[i+1]=clave;
        Padre->cantClaves++;
        escrituraDisco(Padre);//Almacenamiento Persistente del Nodo
    }else{
    	while(i>=1 && clave<Padre->claves[i]){
        	i--;
    	}
        i++;
        hijoNodo = lecturaDisco(Padre->hijos[i]);
        if(hijoNodo->cantClaves == (2*T-1)){
        	divideNodo(Padre,i);
            if(clave > Padre->claves[i]){
            	i++;
                hijoNodo = lecturaDisco(Padre->hijos[i]);
            }
         }
    	insertarNoLleno(hijoNodo,clave);
    }
}
//B-TREE-INSERT Pag.495 Cormen.
Nodo *insertaClaveArbolB(Arbol *raiz,int clave){
	Nodo *aux = (Nodo *)malloc(sizeof(Nodo));
	aux = BuscaArbolB(raiz,clave);
	if(raiz->cantClaves > 0 && aux!=NULL){
		free(aux);
		return(raiz);
	}
	if(raiz->cantClaves == (2*T-1)){
    	Nodo *nuevaRaiz;
        nuevaRaiz = iniciaArbolVacio();
        nuevaRaiz->esHoja = 0;
        nuevaRaiz->hijos[1] = raiz->miNombre;
        escrituraDisco(raiz);
        divideNodo(nuevaRaiz,1);
        insertarNoLleno(nuevaRaiz,clave);
        return(nuevaRaiz);
     }else{
        insertarNoLleno(raiz,clave);
        return(raiz);
     }
}
