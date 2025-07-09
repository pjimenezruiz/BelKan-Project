#ifndef COMPORTAMIENTORESCATADOR_H
#define COMPORTAMIENTORESCATADOR_H

#include <chrono>
#include <time.h>
#include <thread>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <iostream>
#include <iostream>
#include <list>
#include "comportamientos/comportamiento.hpp"

class ComportamientoRescatador : public Comportamiento
{

public:
  ComportamientoRescatador(unsigned int size = 0) : Comportamiento(size)
  {
    // Inicializar Variables de Estado Niveles 0,1,4
    last_action=IDLE;
    tiene_zapatillas=false;
    giro45Izq=0;
    importante=false;
    mismacasilla=0;
    curva=false;
    cuantoandando=0;
    hePasado.resize(mapaResultado.size()); 
    for (int i = 0; i<mapaResultado.size(); ++i) {
      hePasado[i].resize(mapaResultado.size(), false);
    }
    hayPlan=false;
    plan.clear();
    recargando=false;
  }
  ComportamientoRescatador(std::vector<std::vector<unsigned char>> mapaR, std::vector<std::vector<unsigned char>> mapaC) : Comportamiento(mapaR,mapaC)
  {
    // Inicializar Variables de Estado Niveles 2,3
    hayPlan=false;
    plan.clear();
    tiene_zapatillas=false;
  }
  ComportamientoRescatador(const ComportamientoRescatador &comport) : Comportamiento(comport) {}
  ~ComportamientoRescatador() {}

  Action think(Sensores sensores);

  int interact(Action accion, int valor);

  Action ComportamientoRescatadorNivel_0(Sensores sensores);
  Action ComportamientoRescatadorNivel_1(Sensores sensores);
  Action ComportamientoRescatadorNivel_2(Sensores sensores);
  Action ComportamientoRescatadorNivel_3(Sensores sensores);
  Action ComportamientoRescatadorNivel_4(Sensores sensores);

  /**
   * NIVELES 0, 1
   * Devuelve si se puede ir a una casilla en función de la diferencia de altura y de si hay zapatillas.
   */
  bool ViablePorAlturaR(char casilla, int dif, bool zap) {
    if(abs(dif)<=1 or (zap and abs(dif)<=2)) 
      return true;
    else  
      return false;
  }

  /**
   * NIVEL 0
   * Devuelve si se puede ir a una casilla en función de qué tipo es, la diferencia de altura y si chocaría con el auxiliar. 
   */
  bool ViableR(char casilla, int dif, bool zap, Sensores sensores, int pos) {
    if(importante || mismacasilla>15) {
      if(ViablePorAlturaR(casilla, dif, zap) && (casilla=='X' || casilla=='C' || casilla=='D' /** || casilla =='A' || casilla=='T' || casilla=='S' */) && sensores.agentes[pos]!='a') {
        return true;
      }
      else 
        return false;
    }
    else {
      if(ViablePorAlturaR(casilla, dif, zap) && (casilla=='X' || casilla=='C' || casilla=='D') && sensores.agentes[pos]!='a') {
        return true;
      }
      else 
        return false;
    }
    
  }

  /**
   * NIVEL 1
   * Devuelve si se puede ir a una casilla en función de qué tipo es, la diferencia de altura y si chocaría con el auxiliar. 
   */
  bool Viable1R(char casilla, int dif, bool zap, Sensores sensores, int pos) {
    if(importante || mismacasilla>15) {
      if(ViablePorAlturaR(casilla, dif, zap) && (casilla=='C' || casilla=='D' || casilla=='S' ) && sensores.agentes[pos]!='a') {
        return true;
      }
      else 
        return false;
    }
    else {
      if(ViablePorAlturaR(casilla, dif, zap) && (casilla=='C' || casilla=='D' || casilla=='S') && sensores.agentes[pos]!='a') {
        return true;
      }
      else 
        return false;
    }
    
  }

  /**
   * NIVEL 0
   * Devuelve si hay alguna casilla interesante a simple vista (inmediatamente delante) para el nivel 0 (puesto base, zapatillas, camino). Si no, devuelve -1.
   */
  int VeoCasillaInteresante0R(Sensores sensores, bool zap) {
    int casilla=-1;
    for(int i=1; i<4; i++) {  // puesto base
      if(sensores.superficie[i]=='X' && ViablePorAlturaR(sensores.superficie[i], sensores.cota[i]-sensores.cota[0], tiene_zapatillas) && sensores.agentes[i]!='a') { 
        casilla=i;
        return casilla;
      }
    }
    for(int i=1; i<4; i++) {  // zapatillas
      if(sensores.superficie[i]=='D' && !zap && ViablePorAlturaR(sensores.superficie[i], sensores.cota[i]-sensores.cota[0], tiene_zapatillas) && sensores.agentes[i]!='a') {  
        casilla=i;
        return casilla;
      }
    }
    for(int i=1; i<4; i++) {  // camino 
      if((sensores.superficie[i]=='C' && ViablePorAlturaR(sensores.superficie[i], sensores.cota[i]-sensores.cota[0], tiene_zapatillas) && sensores.agentes[i]!='a') /** || (sensores.superficie[i]=='S' && ViablePorAlturaR(sensores.superficie[i], sensores.cota[i]-sensores.cota[0], tiene_zapatillas)) */) { 
        casilla=i;
        return casilla;
      }
    }
    return casilla;
  }

  /**
   * NIVEL 1
   * Devuelve si hay alguna casilla interesante a simple vista (inmediatamente delante) para el nivel 1 (zapatillas, camino y sendero) en función de si se ha pisado ya o no. Si no, devuelve -1.
   */
  int VeoCasillaInteresante1R(Sensores sensores, bool zap) {
    int casilla=-1;
    for(int i=1; i<4; i++) {
      if(sensores.superficie[i]=='D' && !zap && ViablePorAlturaR(sensores.superficie[i], sensores.cota[i]-sensores.cota[0], tiene_zapatillas)) {  // zapatillas
        casilla=i;
        return casilla;
      }
    }
    for(int i=1; i<4; i++) {
      if((sensores.superficie[i]=='C' && !yahePasado(hePasado, sensores, i) && ViablePorAlturaR(sensores.superficie[i], sensores.cota[i]-sensores.cota[0], tiene_zapatillas)) || (sensores.superficie[i]=='S' && !yahePasado(hePasado, sensores, i) && ViablePorAlturaR(sensores.superficie[i], sensores.cota[i]-sensores.cota[0], tiene_zapatillas))) { // camino o sendero
        casilla=i;
        return casilla;
      }
    }
    for(int i=1; i<4; i++) {
      if((sensores.superficie[i]=='C' && ViablePorAlturaR(sensores.superficie[i], sensores.cota[i]-sensores.cota[0], tiene_zapatillas)) || (sensores.superficie[i]=='S' && ViablePorAlturaR(sensores.superficie[i], sensores.cota[i]-sensores.cota[0], tiene_zapatillas))) { // camino o sendero
        casilla=i;
        return casilla;
      }
    }
    return casilla;
  }

  /**
   * NIVEL 1
   * Actualiza el mapa hePasado, poniendo a true las casillas que se han pisado.
   */
  void actualizahePasado(Sensores sensores) {
    hePasado[sensores.posF][sensores.posC]=true;
  }

  /**
   * NIVEL 1
   * Almacena si una casilla ha sido pisada ya por el rescatador en función del contenido de hePasado. Si se ha pisado devuelve true, si no false. 
   * pos\in [1,3]
   */
  bool yahePasado(vector<vector<bool>> &pasado, Sensores sensores, int pos) {
    switch(sensores.rumbo) {
      case norte:
        if(pos==1) {
          if(pasado[sensores.posF-1][sensores.posC-1]==true) {
            return true;
          }
          else {
            return false;
          }
        }
        else if(pos==2) {
          if(pasado[sensores.posF-1][sensores.posC]==true) {
            return true;
          }
          else {
            return false;
          }
        }
        else {
          if(pasado[sensores.posF-1][sensores.posC+1]==true) {
            return true;
          }
          else {
            return false;
          }
        }
        break;
      case noreste:
        if(pos==1) {
          if(pasado[sensores.posF-1][sensores.posC]==true) {
            return true;
          }
          else {
            return false;
          }
        }
        else if(pos==2) {
          if(pasado[sensores.posF-1][sensores.posC+1]==true) {
            return true;
          }
          else {
            return false;
          }
        }
        else {
          if(pasado[sensores.posF][sensores.posC+1]==true) {
            return true;
          }
          else {
            return false;
          }
        }
        break;
      case este:
        if(pos==1) {
          if(pasado[sensores.posF-1][sensores.posC+1]==true) {
            return true;
          }
          else {
            return false;
          }
        }
        else if(pos==2) {
          if(pasado[sensores.posF][sensores.posC+1]==true) {
            return true;
          }
          else {
            return false;
          }
        }
        else {
          if(pasado[sensores.posF+1][sensores.posC+1]==true) {
            return true;
          }
          else {
            return false;
          }
        }
        break;
      case sureste:
        if(pos==1) {
          if(pasado[sensores.posF][sensores.posC+1]==true) {
            return true;
          }
          else {
            return false;
          }
        }
        else if(pos==2) {
          if(pasado[sensores.posF+1][sensores.posC+1]==true) {
            return true;
          }
          else {
            return false;
          }
        }
        else {
          if(pasado[sensores.posF+1][sensores.posC]==true) {
            return true;
          }
          else {
            return false;
          }
        }
        break;
      case sur:
        if(pos==1) {
          if(pasado[sensores.posF+1][sensores.posC+1]==true) {
            return true;
          }
          else {
            return false;
          }
        }
        else if(pos==2) {
          if(pasado[sensores.posF+1][sensores.posC]==true) {
            return true;
          }
          else {
            return false;
          }
        }
        else {
          if(pasado[sensores.posF+1][sensores.posC-1]==true) {
            return true;
          }
          else {
            return false;
          }
        }
        break;
      case suroeste:
        if(pos==1) {
          if(pasado[sensores.posF+1][sensores.posC]==true) {
            return true;
          }
          else {
            return false;
          }
        }
        else if(pos==2) {
          if(pasado[sensores.posF+1][sensores.posC-1]==true) {
            return true;
          }
          else {
            return false;
          }
        }
        else {
          if(pasado[sensores.posF][sensores.posC-1]==true) {
            return true;
          }
          else {
            return false;
          }
        }
        break;
      case oeste:
        if(pos==1) {
          if(pasado[sensores.posF+1][sensores.posC-1]==true) {
            return true;
          }
          else {
            return false;
          }
        }
        else if(pos==2) {
          if(pasado[sensores.posF][sensores.posC-1]==true) {
            return true;
          }
          else {
            return false;
          }
        }
        else {
          if(pasado[sensores.posF-1][sensores.posC-1]==true) {
            return true;
          }
          else {
            return false;
          }
        }
        break;
      case noroeste:
        if(pos==1) {
          if(pasado[sensores.posF][sensores.posC-1]==true) {
            return true;
          }
          else {
            return false;
          }
        }
        else if(pos==2) {
          if(pasado[sensores.posF-1][sensores.posC-1]==true) {
            return true;
          }
          else {
            return false;
          }
        }
        else {
          if(pasado[sensores.posF-1][sensores.posC]==true) {
            return true;
          }
          else {
            return false;
          }
        }
        break;
    }
  }

  /**
   * NIVELES 2, 4
   * Define el tipo de dato EstadoR
   */
  struct EstadoR {
    int f, c;
    int brujula; // Del 0 al 7 (norte, noreste, este, sureste, sur, suroeste, oeste, noroeste de menor a mayor)
    bool zapatillas;
    bool operator==(const EstadoR & nuevo) const {
      return f==nuevo.f && c==nuevo.c && brujula==nuevo.brujula && zapatillas==nuevo.zapatillas;
    }
    bool operator<(const EstadoR & nuevo) const {
      if (f < nuevo.f) return true;
      else if (f == nuevo.f and c < nuevo.c) return true;
      else if (f == nuevo.f and c == nuevo.c and brujula < nuevo.brujula) return true;
      else if (f == nuevo.f and c == nuevo.c and brujula == nuevo.brujula and zapatillas < nuevo.zapatillas) return true;
      else return false;
    }
  };

  /**
   * NIVEL 2
   * Define el tipo de dato NodoR
   */
  struct NodoR {
    EstadoR estado;
    list<Action> secuencia;
    int coste;
    bool operator==(const NodoR &nodo) const {
      return estado==nodo.estado;
    }
    bool operator<(const NodoR &node) const{
      if (estado.f < node.estado.f) return true;
      else if (estado.f == node.estado.f and estado.c < node.estado.c) return true;
      else if (estado.f == node.estado.f and estado.c == node.estado.c and estado.brujula < node.estado.brujula) return true;
      else if (estado.f == node.estado.f and estado.c == node.estado.c and estado.brujula == node.estado.brujula and estado.zapatillas < node.estado.zapatillas) return true;
      else return false;
    }
    bool operator>(const NodoR &node) const{
     return coste>node.coste;
    }
  };

  /**
   * NIVEL 2
   * Buscar el camino de Dijkstra teniendo en cuenta el consumo de energía.
   */
  list<Action> dijkstraR( EstadoR &inicial, const EstadoR &objetivo, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);
  /**
   * NIVEL 2
   * Visualiza el plan.
   */
  void VisualizaPlan(const EstadoR &st, const list<Action> &plan);
  /**
   * NIVEL 2
   * Pinta el plan.
   */
  void PintaPlan(const list<Action> &plan, bool zap);

  /**
   * NIVEL 2
   * Devuelve el estado de la siguiente casilla del rescatador después de hacer WALK.
   */
  EstadoR NextCasillaR(const EstadoR &st){
    EstadoR siguiente = st;
    switch (st.brujula)
    {
      case norte:
        siguiente.f = st.f - 1;
        break;
      case noreste:
        siguiente.f = st.f - 1;
        siguiente.c = st.c + 1;
        break;
      case este:
        siguiente.c = st.c + 1;
        break;
      case sureste:
        siguiente.f = st.f + 1;
        siguiente.c = st.c + 1;
        break;
      case sur:
        siguiente.f = st.f + 1;
        break;
      case suroeste:
        siguiente.f = st.f + 1;
        siguiente.c = st.c - 1;
        break;
      case oeste:
        siguiente.c = st.c - 1;
        break;
      case noroeste:
        siguiente.f = st.f - 1;
        siguiente.c = st.c - 1;
        break;
    }
    return siguiente;
  }

  /**
   * NIVEL 2
   * Devuelve true si la casilla es accesible, false si no.
   */
  bool CasillaAccesibleR(const EstadoR &origen, const EstadoR &destino, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura){
    int fila=terreno.size();
    int columna=terreno[0].size();
    if(destino.f<0 or destino.c<0 or destino.f>=fila or destino.c>=columna) { // se sale del mapa
      return false;
    }
    unsigned char tipo=terreno[destino.f][destino.c];
    if(tipo=='P' or tipo=='M' or tipo =='B') {  // precipicio, obstáculo o bosque
      return false;
    }
    int altura_inicial=altura[origen.f][origen.c];
    int altura_final=altura[destino.f][destino.c];
    int dif=abs(altura_inicial-altura_final);
    if((origen.zapatillas and dif<=2) or (!origen.zapatillas and dif<=1)) { // diferencia de altura superable y casilla transitable
      return true;
    }
    return false;
  }

  /**
   * NIVEL 2
   * Devuelve el estado del rescatador después de la acción indicada.
   */
  EstadoR applyR(Action accion, const EstadoR & st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura){
    EstadoR aplicar=st;
    switch(accion){
      case WALK: {
        EstadoR next=NextCasillaR(st);  // estado después de WALK
        if (CasillaAccesibleR(st, next, terreno,altura)){
          aplicar=next;
        }}
        break;
      case TURN_SR: {  
        aplicar.brujula=(aplicar.brujula+1)%8;}  // mismo estado, distinta orientación
        break;
      case TURN_L: {
        aplicar.brujula=(aplicar.brujula+6)%8;}  // mismo estado, distinta orientación
        break;
      case RUN: {
        EstadoR intermedia=NextCasillaR(st);  
        EstadoR destino=NextCasillaR(intermedia); // estado después de WALKx2 (la altura de la intermedia no cuenta, solo el tipo)
        if (intermedia.f>=0 && intermedia.f<terreno.size() && intermedia.c>=0 && intermedia.c<terreno[0].size() && destino.f>=0 && destino.f<terreno.size() && destino.c>=0 && destino.c<terreno[0].size()) {
                bool intermediaOK=(terreno[intermedia.f][intermedia.c]!='P' && terreno[intermedia.f][intermedia.c]!='M' && terreno[intermedia.f][intermedia.c]!='B');
                bool destinoOK=CasillaAccesibleR(st, destino, terreno, altura);
                if (intermediaOK && destinoOK) {
                    aplicar=destino;
                }
            }}
        break;
    }
    if(terreno[aplicar.f][aplicar.c]=='D') {
      aplicar.zapatillas=true;
    }
    return aplicar;
  }

  /**
   * NIVEL 4
   * Define el tipo de dato Nodo4
   */
  struct Nodo4 {
    EstadoR estado;
    list<Action> secuencia;
    int coste;
    int heuristica;
    bool operator<(const Nodo4 &node) const{
      if (estado.f < node.estado.f) return true;
      else if (estado.f == node.estado.f and estado.c < node.estado.c) return true;
      else if (estado.f == node.estado.f and estado.c == node.estado.c and estado.brujula < node.estado.brujula) return true;
      else if (estado.f == node.estado.f and estado.c == node.estado.c and estado.brujula == node.estado.brujula and estado.zapatillas < node.estado.zapatillas) return true;
      else return false;
    }
    bool operator>(const Nodo4 &node) const{
     return (coste+heuristica)>(node.coste+node.heuristica);
    }
  };

  /**
   * NIVEL 4
   * Distancia aérea
   */
  int HeuristicaR(const EstadoR& inicial, const EstadoR& final) {
    int x=abs(final.f-inicial.f);
    int y=abs(final.c-inicial.c);
    int diagonal=min(x,y);
    int recto=abs(x-y);
    return diagonal+recto;
  }

  /**
   * NIVEL 4
   * Algoritmo A* con la distancia de Manhattan como heurística. Se usa para trazar un plan rápido hasta el objetivo sin tener en cuenta las casillas no conocidas
   */
  list<Action> estrellaR(EstadoR origen, EstadoR destino, const vector<vector<unsigned char>> &m, const vector<vector<unsigned char>> &a);

  /**
   * NIVEL 4
   * Define si una acción dada produce un movimiento válido en función de la información de los sensores
   */
  bool AccionViableR(Action accion, Sensores sensores) {
    if(accion==TURN_SR or accion==TURN_L or accion==IDLE) {
      return true;
    }
    unsigned char tipo_actual=sensores.superficie[0];
    int altura_actual=sensores.cota[0];
    int tipo_final=sensores.superficie[2];
    int altura_final=sensores.cota[2];
    if(tipo_final=='P' or tipo_final=='M' or tipo_final=='B') {
      return false;
    }
    if((abs(altura_actual-altura_final)<=1) or (tiene_zapatillas and abs(altura_actual-altura_final)<=2)) {
      return true;
    }
    return false;
  }

  /**
   * NIVELES 0, 1, 4
   * Rellena el contenido del mapa de superficie y cota con lo que puede ver en cada iteración.
   */
  void SituarSensorEnMapaR(vector<vector<unsigned char>> &m, vector<vector<unsigned char>> &a, Sensores sensores) {
    m[sensores.posF][sensores.posC]=sensores.superficie[0];
    int pos=1;
    switch(sensores.rumbo) {
      case norte:
        m[sensores.posF-1][sensores.posC-1]=sensores.superficie[1];
        m[sensores.posF-1][sensores.posC]=sensores.superficie[2];
        m[sensores.posF-1][sensores.posC+1]=sensores.superficie[3];
        m[sensores.posF-2][sensores.posC-2]=sensores.superficie[4];
        m[sensores.posF-2][sensores.posC-1]=sensores.superficie[5];
        m[sensores.posF-2][sensores.posC]=sensores.superficie[6];
        m[sensores.posF-2][sensores.posC+1]=sensores.superficie[7];
        m[sensores.posF-2][sensores.posC+2]=sensores.superficie[8];
        m[sensores.posF-3][sensores.posC-3]=sensores.superficie[9];
        m[sensores.posF-3][sensores.posC-2]=sensores.superficie[10];
        m[sensores.posF-3][sensores.posC-1]=sensores.superficie[11];
        m[sensores.posF-3][sensores.posC]=sensores.superficie[12];
        m[sensores.posF-3][sensores.posC+1]=sensores.superficie[13];
        m[sensores.posF-3][sensores.posC+2]=sensores.superficie[14];
        m[sensores.posF-3][sensores.posC+3]=sensores.superficie[15];
        
        a[sensores.posF-1][sensores.posC-1]=sensores.cota[1];
        a[sensores.posF-1][sensores.posC]=sensores.cota[2];
        a[sensores.posF-1][sensores.posC+1]=sensores.cota[3];
        a[sensores.posF-2][sensores.posC-2]=sensores.cota[4];
        a[sensores.posF-2][sensores.posC-1]=sensores.cota[5];
        a[sensores.posF-2][sensores.posC]=sensores.cota[6];
        a[sensores.posF-2][sensores.posC+1]=sensores.cota[7];
        a[sensores.posF-2][sensores.posC+2]=sensores.cota[8];
        a[sensores.posF-3][sensores.posC-3]=sensores.cota[9];
        a[sensores.posF-3][sensores.posC-2]=sensores.cota[10];
        a[sensores.posF-3][sensores.posC-1]=sensores.cota[11];
        a[sensores.posF-3][sensores.posC]=sensores.cota[12];
        a[sensores.posF-3][sensores.posC+1]=sensores.cota[13];
        a[sensores.posF-3][sensores.posC+2]=sensores.cota[14];
        a[sensores.posF-3][sensores.posC+3]=sensores.cota[15];
        break;
      case noreste:
        m[sensores.posF-1][sensores.posC]=sensores.superficie[1];
        m[sensores.posF-1][sensores.posC+1]=sensores.superficie[2];
        m[sensores.posF][sensores.posC+1]=sensores.superficie[3];
        m[sensores.posF-2][sensores.posC]=sensores.superficie[4];
        m[sensores.posF-2][sensores.posC+1]=sensores.superficie[5];
        m[sensores.posF-2][sensores.posC+2]=sensores.superficie[6];
        m[sensores.posF-1][sensores.posC+2]=sensores.superficie[7];
        m[sensores.posF][sensores.posC+2]=sensores.superficie[8];
        m[sensores.posF-3][sensores.posC]=sensores.superficie[9];
        m[sensores.posF-3][sensores.posC+1]=sensores.superficie[10];
        m[sensores.posF-3][sensores.posC+2]=sensores.superficie[11];
        m[sensores.posF-3][sensores.posC+3]=sensores.superficie[12];
        m[sensores.posF-2][sensores.posC+3]=sensores.superficie[13];
        m[sensores.posF-1][sensores.posC+3]=sensores.superficie[14];
        m[sensores.posF][sensores.posC+3]=sensores.superficie[15];

        a[sensores.posF-1][sensores.posC]=sensores.cota[1];
        a[sensores.posF-1][sensores.posC+1]=sensores.cota[2];
        a[sensores.posF][sensores.posC+1]=sensores.cota[3];
        a[sensores.posF-2][sensores.posC]=sensores.cota[4];
        a[sensores.posF-2][sensores.posC+1]=sensores.cota[5];
        a[sensores.posF-2][sensores.posC+2]=sensores.cota[6];
        a[sensores.posF-1][sensores.posC+2]=sensores.cota[7];
        a[sensores.posF][sensores.posC+2]=sensores.cota[8];
        a[sensores.posF-3][sensores.posC]=sensores.cota[9];
        a[sensores.posF-3][sensores.posC+1]=sensores.cota[10];
        a[sensores.posF-3][sensores.posC+2]=sensores.cota[11];
        a[sensores.posF-3][sensores.posC+3]=sensores.cota[12];
        a[sensores.posF-2][sensores.posC+3]=sensores.cota[13];
        a[sensores.posF-1][sensores.posC+3]=sensores.cota[14];
        a[sensores.posF][sensores.posC+3]=sensores.cota[15];
        break;
      case este:
        m[sensores.posF-1][sensores.posC+1]=sensores.superficie[1];
        m[sensores.posF][sensores.posC+1]=sensores.superficie[2];
        m[sensores.posF+1][sensores.posC+1]=sensores.superficie[3];
        m[sensores.posF-2][sensores.posC+2]=sensores.superficie[4];
        m[sensores.posF-1][sensores.posC+2]=sensores.superficie[5];
        m[sensores.posF][sensores.posC+2]=sensores.superficie[6];
        m[sensores.posF+1][sensores.posC+2]=sensores.superficie[7];
        m[sensores.posF+2][sensores.posC+2]=sensores.superficie[8];
        m[sensores.posF-3][sensores.posC+3]=sensores.superficie[9];
        m[sensores.posF-2][sensores.posC+3]=sensores.superficie[10];
        m[sensores.posF-1][sensores.posC+3]=sensores.superficie[11];
        m[sensores.posF][sensores.posC+3]=sensores.superficie[12];
        m[sensores.posF+1][sensores.posC+3]=sensores.superficie[13];
        m[sensores.posF+2][sensores.posC+3]=sensores.superficie[14];
        m[sensores.posF+3][sensores.posC+3]=sensores.superficie[15];

        a[sensores.posF-1][sensores.posC+1]=sensores.cota[1];
        a[sensores.posF][sensores.posC+1]=sensores.cota[2];
        a[sensores.posF+1][sensores.posC+1]=sensores.cota[3];
        a[sensores.posF-2][sensores.posC+2]=sensores.cota[4];
        a[sensores.posF-1][sensores.posC+2]=sensores.cota[5];
        a[sensores.posF][sensores.posC+2]=sensores.cota[6];
        a[sensores.posF+1][sensores.posC+2]=sensores.cota[7];
        a[sensores.posF+2][sensores.posC+2]=sensores.cota[8];
        a[sensores.posF-3][sensores.posC+3]=sensores.cota[9];
        a[sensores.posF-2][sensores.posC+3]=sensores.cota[10];
        a[sensores.posF-1][sensores.posC+3]=sensores.cota[11];
        a[sensores.posF][sensores.posC+3]=sensores.cota[12];
        a[sensores.posF+1][sensores.posC+3]=sensores.cota[13];
        a[sensores.posF+2][sensores.posC+3]=sensores.cota[14];
        a[sensores.posF+3][sensores.posC+3]=sensores.cota[15];
        break;
      case sureste:
        m[sensores.posF][sensores.posC+1]=sensores.superficie[1];
        m[sensores.posF+1][sensores.posC+1]=sensores.superficie[2];
        m[sensores.posF+1][sensores.posC]=sensores.superficie[3];
        m[sensores.posF][sensores.posC+2]=sensores.superficie[4];
        m[sensores.posF+1][sensores.posC+2]=sensores.superficie[5];
        m[sensores.posF+2][sensores.posC+2]=sensores.superficie[6];
        m[sensores.posF+2][sensores.posC+1]=sensores.superficie[7];
        m[sensores.posF+2][sensores.posC]=sensores.superficie[8];
        m[sensores.posF][sensores.posC+3]=sensores.superficie[9];
        m[sensores.posF+1][sensores.posC+3]=sensores.superficie[10];
        m[sensores.posF+2][sensores.posC+3]=sensores.superficie[11];
        m[sensores.posF+3][sensores.posC+3]=sensores.superficie[12];
        m[sensores.posF+3][sensores.posC+2]=sensores.superficie[13];
        m[sensores.posF+3][sensores.posC+1]=sensores.superficie[14];
        m[sensores.posF+3][sensores.posC]=sensores.superficie[15];

        a[sensores.posF][sensores.posC+1]=sensores.cota[1];
        a[sensores.posF+1][sensores.posC+1]=sensores.cota[2];
        a[sensores.posF+1][sensores.posC]=sensores.cota[3];
        a[sensores.posF][sensores.posC+2]=sensores.cota[4];
        a[sensores.posF+1][sensores.posC+2]=sensores.cota[5];
        a[sensores.posF+2][sensores.posC+2]=sensores.cota[6];
        a[sensores.posF+2][sensores.posC+1]=sensores.cota[7];
        a[sensores.posF+2][sensores.posC]=sensores.cota[8];
        a[sensores.posF][sensores.posC+3]=sensores.cota[9];
        a[sensores.posF+1][sensores.posC+3]=sensores.cota[10];
        a[sensores.posF+2][sensores.posC+3]=sensores.cota[11];
        a[sensores.posF+3][sensores.posC+3]=sensores.cota[12];
        a[sensores.posF+3][sensores.posC+2]=sensores.cota[13];
        a[sensores.posF+3][sensores.posC+1]=sensores.cota[14];
        a[sensores.posF+3][sensores.posC]=sensores.cota[15];
        break;
      case sur:
        m[sensores.posF+1][sensores.posC+1]=sensores.superficie[1];
        m[sensores.posF+1][sensores.posC]=sensores.superficie[2];
        m[sensores.posF+1][sensores.posC-1]=sensores.superficie[3];
        m[sensores.posF+2][sensores.posC+2]=sensores.superficie[4];
        m[sensores.posF+2][sensores.posC+1]=sensores.superficie[5];
        m[sensores.posF+2][sensores.posC]=sensores.superficie[6];
        m[sensores.posF+2][sensores.posC-1]=sensores.superficie[7];
        m[sensores.posF+2][sensores.posC-2]=sensores.superficie[8];
        m[sensores.posF+3][sensores.posC+3]=sensores.superficie[9];
        m[sensores.posF+3][sensores.posC+2]=sensores.superficie[10];
        m[sensores.posF+3][sensores.posC+1]=sensores.superficie[11];
        m[sensores.posF+3][sensores.posC]=sensores.superficie[12];
        m[sensores.posF+3][sensores.posC-1]=sensores.superficie[13];
        m[sensores.posF+3][sensores.posC-2]=sensores.superficie[14];
        m[sensores.posF+3][sensores.posC-3]=sensores.superficie[15];

        a[sensores.posF+1][sensores.posC+1]=sensores.cota[1];
        a[sensores.posF+1][sensores.posC]=sensores.cota[2];
        a[sensores.posF+1][sensores.posC-1]=sensores.cota[3];
        a[sensores.posF+2][sensores.posC+2]=sensores.cota[4];
        a[sensores.posF+2][sensores.posC+1]=sensores.cota[5];
        a[sensores.posF+2][sensores.posC]=sensores.cota[6];
        a[sensores.posF+2][sensores.posC-1]=sensores.cota[7];
        a[sensores.posF+2][sensores.posC-2]=sensores.cota[8];
        a[sensores.posF+3][sensores.posC+3]=sensores.cota[9];
        a[sensores.posF+3][sensores.posC+2]=sensores.cota[10];
        a[sensores.posF+3][sensores.posC+1]=sensores.cota[11];
        a[sensores.posF+3][sensores.posC]=sensores.cota[12];
        a[sensores.posF+3][sensores.posC-1]=sensores.cota[13];
        a[sensores.posF+3][sensores.posC-2]=sensores.cota[14];
        a[sensores.posF+3][sensores.posC-3]=sensores.cota[15];
        break;
      case suroeste:
        m[sensores.posF+1][sensores.posC]=sensores.superficie[1];
        m[sensores.posF+1][sensores.posC-1]=sensores.superficie[2];
        m[sensores.posF][sensores.posC-1]=sensores.superficie[3];
        m[sensores.posF+2][sensores.posC]=sensores.superficie[4];
        m[sensores.posF+2][sensores.posC-1]=sensores.superficie[5];
        m[sensores.posF+2][sensores.posC-2]=sensores.superficie[6];
        m[sensores.posF+1][sensores.posC-2]=sensores.superficie[7];
        m[sensores.posF][sensores.posC-2]=sensores.superficie[8];
        m[sensores.posF+3][sensores.posC]=sensores.superficie[9];
        m[sensores.posF+3][sensores.posC-1]=sensores.superficie[10];
        m[sensores.posF+3][sensores.posC-2]=sensores.superficie[11];
        m[sensores.posF+3][sensores.posC-3]=sensores.superficie[12];
        m[sensores.posF+2][sensores.posC-3]=sensores.superficie[13];
        m[sensores.posF+1][sensores.posC-3]=sensores.superficie[14];
        m[sensores.posF][sensores.posC-3]=sensores.superficie[15];

        a[sensores.posF+1][sensores.posC]=sensores.cota[1];
        a[sensores.posF+1][sensores.posC-1]=sensores.cota[2];
        a[sensores.posF][sensores.posC-1]=sensores.cota[3];
        a[sensores.posF+2][sensores.posC]=sensores.cota[4];
        a[sensores.posF+2][sensores.posC-1]=sensores.cota[5];
        a[sensores.posF+2][sensores.posC-2]=sensores.cota[6];
        a[sensores.posF+1][sensores.posC-2]=sensores.cota[7];
        a[sensores.posF][sensores.posC-2]=sensores.cota[8];
        a[sensores.posF+3][sensores.posC]=sensores.cota[9];
        a[sensores.posF+3][sensores.posC-1]=sensores.cota[10];
        a[sensores.posF+3][sensores.posC-2]=sensores.cota[11];
        a[sensores.posF+3][sensores.posC-3]=sensores.cota[12];
        a[sensores.posF+2][sensores.posC-3]=sensores.cota[13];
        a[sensores.posF+1][sensores.posC-3]=sensores.cota[14];
        a[sensores.posF][sensores.posC-3]=sensores.cota[15];
        break;
      case oeste:
        m[sensores.posF+1][sensores.posC-1]=sensores.superficie[1];
        m[sensores.posF][sensores.posC-1]=sensores.superficie[2];
        m[sensores.posF-1][sensores.posC-1]=sensores.superficie[3];
        m[sensores.posF+2][sensores.posC-2]=sensores.superficie[4];
        m[sensores.posF+1][sensores.posC-2]=sensores.superficie[5];
        m[sensores.posF][sensores.posC-2]=sensores.superficie[6];
        m[sensores.posF-1][sensores.posC-2]=sensores.superficie[7];
        m[sensores.posF-2][sensores.posC-2]=sensores.superficie[8];
        m[sensores.posF+3][sensores.posC-3]=sensores.superficie[9];
        m[sensores.posF+2][sensores.posC-3]=sensores.superficie[10];
        m[sensores.posF+1][sensores.posC-3]=sensores.superficie[11];
        m[sensores.posF][sensores.posC-3]=sensores.superficie[12];
        m[sensores.posF-1][sensores.posC-3]=sensores.superficie[13];
        m[sensores.posF-2][sensores.posC-3]=sensores.superficie[14];
        m[sensores.posF-3][sensores.posC-3]=sensores.superficie[15];

        a[sensores.posF+1][sensores.posC-1]=sensores.cota[1];
        a[sensores.posF][sensores.posC-1]=sensores.cota[2];
        a[sensores.posF-1][sensores.posC-1]=sensores.cota[3];
        a[sensores.posF+2][sensores.posC-2]=sensores.cota[4];
        a[sensores.posF+1][sensores.posC-2]=sensores.cota[5];
        a[sensores.posF][sensores.posC-2]=sensores.cota[6];
        a[sensores.posF-1][sensores.posC-2]=sensores.cota[7];
        a[sensores.posF-2][sensores.posC-2]=sensores.cota[8];
        a[sensores.posF+3][sensores.posC-3]=sensores.cota[9];
        a[sensores.posF+2][sensores.posC-3]=sensores.cota[10];
        a[sensores.posF+1][sensores.posC-3]=sensores.cota[11];
        a[sensores.posF][sensores.posC-3]=sensores.cota[12];
        a[sensores.posF-1][sensores.posC-3]=sensores.cota[13];
        a[sensores.posF-2][sensores.posC-3]=sensores.cota[14];
        a[sensores.posF-3][sensores.posC-3]=sensores.cota[15];
        break;
      case noroeste:
        m[sensores.posF][sensores.posC-1]=sensores.superficie[1];
        m[sensores.posF-1][sensores.posC-1]=sensores.superficie[2];
        m[sensores.posF-1][sensores.posC]=sensores.superficie[3];
        m[sensores.posF][sensores.posC-2]=sensores.superficie[4];
        m[sensores.posF-1][sensores.posC-2]=sensores.superficie[5];
        m[sensores.posF-2][sensores.posC-2]=sensores.superficie[6];
        m[sensores.posF-2][sensores.posC-1]=sensores.superficie[7];
        m[sensores.posF-2][sensores.posC]=sensores.superficie[8];
        m[sensores.posF][sensores.posC-3]=sensores.superficie[9];
        m[sensores.posF-1][sensores.posC-3]=sensores.superficie[10];
        m[sensores.posF-2][sensores.posC-3]=sensores.superficie[11];
        m[sensores.posF-3][sensores.posC-3]=sensores.superficie[12];
        m[sensores.posF-3][sensores.posC-2]=sensores.superficie[13];
        m[sensores.posF-3][sensores.posC-1]=sensores.superficie[14];
        m[sensores.posF-3][sensores.posC]=sensores.superficie[15];

        a[sensores.posF][sensores.posC-1]=sensores.cota[1];
        a[sensores.posF-1][sensores.posC-1]=sensores.cota[2];
        a[sensores.posF-1][sensores.posC]=sensores.cota[3];
        a[sensores.posF][sensores.posC-2]=sensores.cota[4];
        a[sensores.posF-1][sensores.posC-2]=sensores.cota[5];
        a[sensores.posF-2][sensores.posC-2]=sensores.cota[6];
        a[sensores.posF-2][sensores.posC-1]=sensores.cota[7];
        a[sensores.posF-2][sensores.posC]=sensores.cota[8];
        a[sensores.posF][sensores.posC-3]=sensores.cota[9];
        a[sensores.posF-1][sensores.posC-3]=sensores.cota[10];
        a[sensores.posF-2][sensores.posC-3]=sensores.cota[11];
        a[sensores.posF-3][sensores.posC-3]=sensores.cota[12];
        a[sensores.posF-3][sensores.posC-2]=sensores.cota[13];
        a[sensores.posF-3][sensores.posC-1]=sensores.cota[14];
        a[sensores.posF-3][sensores.posC]=sensores.cota[15];
        break;
    }
  }
private:
  // Variables de Estado
  Action last_action;
  bool tiene_zapatillas;
  int giro45Izq;  
  bool importante;
  int mismacasilla;
  bool curva;
  int cuantoandando;

  list<Action> plan;
  bool hayPlan;
  vector<vector<bool>> hePasado;
  bool recargando;
};

#endif
