#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <climits>
using namespace std;

class Nodo {
public:
    char letra;
    int peso;
    vector<Nodo*> hijos;

    Nodo(char letra, int peso) {
        this->letra = letra;
        this->peso = peso;
    }

    void agregarHijo(Nodo* hijo) {
        hijos.push_back(hijo);
    }
};

int menorDistancia(vector<int>& caminos, vector<bool>& visited) {
    int menor = INT_MAX;
    int menorIndex = -1;

    for (int i = 0; i < caminos.size(); ++i) {
        if (!visited[i] && caminos[i] <= menor) {
            menor = caminos[i];
            menorIndex = i;
        }
    }
    return menorIndex;
}

pair<vector<int>, vector<int>> dijkstra(vector<vector<int>>& graf, int destino, Nodo*& arbol) {
    int n = graf.size();
    vector<int> caminosRec(n, -1);
    vector<bool> visited(n, false);
    vector<int> distancias(n, INT_MAX);

    distancias[0] = 0;

    for (int i = 0; i < n - 1; ++i) {
        int u = menorDistancia(distancias, visited);

        visited[u] = true;
        if (u == -1) break; // Si ya se visitaron todos los nodos

        if (u == destino) break; // Si alcanza el nodo destino

        for (int k = 0; k < n; ++k) {
            if (!visited[k] && graf[u][k] > 0 && distancias[u] != INT_MAX && distancias[u] + graf[u][k] < distancias[k]) {
                distancias[k] = distancias[u] + graf[u][k];
                caminosRec[k] = u;
            }
        }
    }

    if (distancias[destino] == INT_MAX) return {{}, {}};

    vector<int> caminosTomados;
    Nodo* nodoActual = new Nodo('A' + destino, distancias[destino]);
    Nodo* raiz = nodoActual; // Nodo raíz del árbol

    for (int actual = destino; actual != -1; actual = caminosRec[actual]) {
        caminosTomados.push_back(actual);
        if (caminosRec[actual] != -1) {
            Nodo* padre = new Nodo('A' + caminosRec[actual], distancias[caminosRec[actual]]);
            padre->agregarHijo(nodoActual);
            nodoActual = padre;
        }
    }

    arbol = nodoActual; // Nodo raíz actualizado
    return {caminosTomados, distancias};
}

void imprimirArbol(Nodo* nodo, string prefijo = "") {
    if (!nodo) return;

    cout << prefijo << nodo->letra << "(" << nodo->peso << ")\n";
    prefijo += "   ";
    for (auto hijo : nodo->hijos) {
        imprimirArbol(hijo, prefijo);
    }
}

void imprimirCaminoRecorrido(vector<int>& camino, vector<int>& distancias) {
    cout << "Camino recorrido: ";
    for (int i = camino.size() - 1; i >= 0; --i) {
        cout << char('A' + camino[i]) << "(" << distancias[camino[i]] << ")";
        if (i > 0) cout << " -> ";
    }
    cout << endl;
}

bool esNumeroValido(string& entrada) {
    for (char c : entrada) {
        if (!isdigit(c)) return false;
    }
    return true;
}

vector<vector<int>> leerGrafoDesdeArchivo(string& nombreArchivo) {
    vector<vector<int>> matrix;
    ifstream arch(nombreArchivo);
    string linea;

    if (getline(arch, linea)) {
        int n = stoi(linea);
        if (n > 26) {
            cout << "\nArchivo excede el maximo. Solo se permite un largo de 26 (letras del abecedario ingles). Favor de modificar el archivo.\n\n";
            exit(1);
        } else {
            while (getline(arch, linea)) {
                vector<int> fila;
                stringstream ss(linea);
                string valopos;
                while (getline(ss, valopos, ' ')) {
                    fila.push_back(stoi(valopos));
                }
                matrix.push_back(fila);
            }
        }
    } else {
        cout << "No se pudo abrir el archivo." << endl;
        exit(1);
    }

    return matrix;
}

int chartoInt(char nodo) {
    return nodo - 'A';
}

void buscarNodos(vector<vector<int>>& grafo) {
    int n = grafo.size();
    char x;
    int continuar;
    string entrada;

    do {
        cout << "Ingrese nodo destino (A -> " << char('A' + n - 1) << ") EN MAYUSCULA: ";
        cin >> x;
        int destino = chartoInt(x);
        if (destino < 0 || destino >= n) {
            cout << "Nodo invalido o no encontrado." << endl;
            continue;
        }

        Nodo* arbol = nullptr;
        auto [camino, distancias] = dijkstra(grafo, destino, arbol);

        if (distancias.empty()) {
            cout << "No hay un camino hacia el nodo " << x << endl;
        } else {
            imprimirCaminoRecorrido(camino, distancias);
            cout << "Camino recorrido (en forma de arbol):" << endl;
            imprimirArbol(arbol);
            cout << "\nDistancia total: " << distancias[destino] << endl;
        }

        do {
            cout << "\nDesea buscar otro nodo? (1 = Si, 0 = No): ";
            cin >> entrada;
            if (esNumeroValido(entrada)) {
                continuar = stoi(entrada);
                if (continuar != 1 && continuar != 0) {
                    cout << "Entrada invalida. Ingrese 1 para Si o 0 para No." << endl;
                }
            } else {
                cout << "Entrada invalida. Ingrese 1 para Si o 0 para No." << endl;
                continuar = -1;
            }
        } while (continuar != 1 && continuar != 0);

        if (continuar == 0) {
            cout << "Finalizando programa." << endl;
            break;
        }

    } while (true);
}

int main() {
    string filename = "adyMatriz.txt";
    vector<vector<int>> matrix = leerGrafoDesdeArchivo(filename);

    cout << "Matriz leida desde el archivo:" << endl;
    for (auto fila : matrix) {
        for (int dato : fila) {
            cout << dato << " ";
        }
        cout << endl;
    }

    buscarNodos(matrix);

    return 0;
}
