#include <stdio.h>
#include <stdlib.h>
#include "tdas/list.h"
#include "tdas/heap.h"
#include "tdas/extra.h"
#include "tdas/stack.h"
#include "tdas/queue.h"
#include <string.h>


// Definición de la estructura para el estado del puzzle
typedef struct {
    int square[3][3]; // Matriz 3x3 que representa el tablero
    int x;    // Posición x del espacio vacío
    int y;    // Posición y del espacio vacío
    int actions;
    //List* actions; //Secuencia de movimientos para llegar al estado
} State;

typedef struct Node {
        State state;
        struct Node* parent;
} Node;

int distancia_L1(State* state) {
    int ev=0;
    int k=1;
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++){
            int val=state->square[i][j];
            if (val==0) continue;
            int ii=(val)/3;
            int jj=(val)%3;
            ev+= abs(ii-i) + abs(jj-j);
        }
    return ev;
}


// Función para imprimir el estado del puzzle
void imprimirEstado(const State *estado) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (estado->square[i][j] == 0)
                printf("x "); // Imprime un espacio en blanco para el espacio vacío
            else
                printf("%d ", estado->square[i][j]);
        }
        printf("\n");
    }
}

void puzzle_completo(Node *current){
    Node *node = current;
    int pasos = current->state.actions;

    while (node != NULL) {
        printf("pasos: %d\n", pasos);
        imprimirEstado(&node->state);
        printf("\n");
        node = node->parent;
        pasos--;
        }
    free(node);
    return;
}
int verMovimiento(int x, int y) { return (x >= 0 && x < 3 && y >= 0 && y < 3); }

void movimiento(State* current, int choice) {

    int x = current->x;
    int y = current->y;
    int temp;

    if (choice == 1) { // Mover espacio vacío hacia arriba
        if (verMovimiento(x , y - 1)) {
            temp = current->square[x][y - 1];
            current->square[x][y - 1] = current->square[x][y];
            current->square[x][y] = temp;
            current->y = y - 1;
        }
    } 
    else if (choice == 2) { // Mover espacio vacío hacia abajo
        if (verMovimiento(x , y + 1)) {
            temp = current->square[x][y + 1];
            current->square[x][y + 1] = current->square[x][y];
            current->square[x][y] = temp;
            current->y = y + 1;
        }
    } 
    else if (choice == 3) { // Mover espacio vacío hacia la izquierda
        if (verMovimiento(x - 1, y)) {
            temp = current->square[x - 1][y];
            current->square[x - 1][y] = current->square[x][y];
            current->square[x][y] = temp;
            current->x = x - 1;
        }
    } 
    else if (choice == 4) { // Mover espacio vacío hacia la derecha
        if (verMovimiento(x + 1, y)) {
            temp = current->square[x + 1][y];
            current->square[x + 1][y] = current->square[x][y];
            current->square[x][y] = temp;
            current->x = x + 1;
        }
    }
}
void copiarEstado(State original, State* new) {

        for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                        new->square[i][j] = original.square[i][j];

        new->x = original.x;
        new->y = original.y;
        new->actions = original.actions;

}
int is_final_state(const State* estado) {
        State finalState = {
                {{0, 1, 2},
                 {3, 4, 5},
                 {6, 7, 8}},
                0,0
        };

        for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                        if (estado->square[i][j] != finalState.square[i][j])
                                return 0;
                        }
        }

        return 1;
}
State* transition(State* antiguoEstado, int action) {
        State *nuevoEstado = (State*) malloc(sizeof(State));
        
        copiarEstado(*antiguoEstado, nuevoEstado);
        movimiento(nuevoEstado, action);
        nuevoEstado->actions++;

        return nuevoEstado;
}
List *get_adj_nodes(Node* nodoPadre) {
        List *adjNodes = list_create();

        int x = nodoPadre->state.x;
        int y = nodoPadre->state.y;
        
        for (int i = 1; i <= 4; i++) {
                State *nuevoEstado = transition(&nodoPadre->state, i);
                if (nuevoEstado->x != x || nuevoEstado->y != y) {

                        Node *nuevoNodo = (Node*) malloc(sizeof(Node));

                        nuevoNodo->state = *nuevoEstado;
                        nuevoNodo->parent = nodoPadre;
                        list_pushBack(adjNodes, nuevoNodo);

                } else {
                        free(nuevoEstado);
                }
        }
        return adjNodes;
}
void dfs(State estado_inicial, int cont)
{
    Stack *stack = stack_create(stack);
    
    Node *nodo_raiz = (Node *) malloc(sizeof(Node));
    nodo_raiz->parent = NULL;
    nodo_raiz->state = estado_inicial;
    
    stack_push(stack, nodo_raiz);
    
    while (list_size(stack) > 0) {
        
        Node *current = (Node *) stack_top(stack);
        stack_pop(stack);
        //printf("%d\n", current->state.actions);
        if (current->state.actions >= 20)
            continue;
        
        if (is_final_state(&current->state)) {
                printf("El puzzle tomo %d pasos!\n\n", current->state.actions);
                Node *nodoActual = current;
                int pasos = current->state.actions;
                
                while (nodoActual != NULL) {
                        
                        if (pasos == 0) 
                                printf("Estado Inicial:\n");

                        else
                                printf("Paso %d:\n", pasos);                  
                        
                        imprimirEstado(&nodoActual->state);
                        printf("\n");

                        nodoActual = nodoActual->parent;
                        pasos--;

                }

                stack_clean(stack);
                free(nodoActual);
                return;
            }
        List *adjNodes = get_adj_nodes(current);
        void *adjNode = list_first(adjNodes);
        
        while (adjNode != NULL) {
            stack_push(stack, adjNode);
            adjNode = list_next(adjNodes);
        }
        list_clean(adjNodes);
        cont++;
    }
    stack_clean(stack);
}
void bfs(State estado_inicial, int cont)
{
        Queue *queue = queue_create(queue);

        Node *nodo_raiz = (Node *) malloc(sizeof(Node));
        nodo_raiz->parent = NULL;
        nodo_raiz->state = estado_inicial;
        queue_insert(queue, nodo_raiz);

        while (list_size(queue) > 0){
                Node *current = (Node *) queue_front(queue);
                queue_remove(queue);
                //printf("%d\n", current->state.actions);
                if (current->state.actions >= 20)
                        continue;
                if (is_final_state(&current->state)){
                        printf("El puzzle tomo %d pasos!\n\n", current->state.actions);
                        Node *nodoActual = current;
                        int pasos = current->state.actions;
                        while (nodoActual != NULL){
                                if (pasos == 0)
                                        printf("Estado Inicial:\n");
                                else
                                        printf("Paso %d:\n", pasos);
                                imprimirEstado(&nodoActual->state);
                                printf("\n");
                                nodoActual = nodoActual->parent;
                                pasos--;
                        }
                        queue_clean(queue);
                        free(nodoActual);
                        return;
                }
                List *adjNodes = get_adj_nodes(current);
                void *adjNode = list_first(adjNodes);
                while (adjNode != NULL){
                        queue_insert(queue, adjNode);
                        adjNode = list_next(adjNodes);
                }
                list_clean(adjNodes);
                cont++;
        }
        queue_clean(queue);
}

int main() {
    // Estado inicial del puzzle
    State estado_inicial = {
        {{0, 2, 8}, // Primera fila (0 representa espacio vacío)
         {1, 3, 4}, // Segunda fila
         {6, 5, 7}, // Tercera fila
         },  
        0, 0   // Posición del espacio vacío (fila 0, columna 1)
    };
    estado_inicial.actions = 0;

    char opcion;
    int cont = 0;
    do {
        puts("========================================");
        puts("     Escoge método de búsqueda");
        puts("========================================");
        
        puts("1) Búsqueda en Profundidad");
        puts("2) Buscar en Anchura");
        puts("3) Buscar Mejor Primero");
        puts("4) Salir");
    
        printf("Ingrese su opción: ");
        scanf(" %c", &opcion);
    
        switch (opcion) {
        case '1':
          limpiarPantalla();
          dfs(estado_inicial, cont);
          break;
        case '2':
          limpiarPantalla();
          bfs(estado_inicial, cont);
          break;
        case '3':
          //best_first(estado_inicial);
          break;
        }
        presioneTeclaParaContinuar();
        limpiarPantalla();

  } while (opcion != '4');

  return 0;
}
