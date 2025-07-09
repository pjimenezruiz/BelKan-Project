#ifndef COMPORTAMIENTOAUXILIAR_H
#define COMPORTAMIENTOAUXILIAR_H

#include <chrono>
#include <time.h>
#include <thread>
#include <cstdlib>
#include <iostream>
#include <list>
#include <cmath>
#include "comportamientos/comportamiento.hpp"

class ComportamientoAuxiliar : public Comportamiento
{

public:
  ComportamientoAuxiliar(unsigned int size = 0) : Comportamiento(size)
  {
    // Inicializar Variables de Estado Niveles 0,1,4
    last_action=IDLE;
    tiene_zapatillas=false;
    giro45Izq=0;
    importante=false;
    mismacasilla=0;
    curva=false;
    cuantoandando=0;
  }
  ComportamientoAuxiliar(std::vector<std::vector<unsigned char>> mapaR, std::vector<std::vector<unsigned char>> mapaC) : Comportamiento(mapaR,mapaC)
  {
    // Inicializar Variables de Estado Niveles 2,3
    hayPlan=false;
    plan.clear();
    tiene_zapatillas=false;
  }
  ComportamientoAuxiliar(const ComportamientoAuxiliar &comport) : Comportamiento(comport) {}
  ~ComportamientoAuxiliar() {}

  Action think(Sensores sensores);

  int interact(Action accion, int valor);

  Action ComportamientoAuxiliarNivel_0(Sensores sensores);
  Action ComportamientoAuxiliarNivel_1(Sensores sensores);
  Action ComportamientoAuxiliarNivel_2(Sensores sensores);
  Action ComportamientoAuxiliarNivel_3(Sensores sensores);
  Action ComportamientoAuxiliarNivel_4(Sensores sensores);

  /**
   * NIVEL 0
   * Devuelve si hay alguna casilla interesante a simple vista (inmediatamente delante) para el nivel 0 (puesto base y camino). Si no, devuelve -1.
   */
  int VeoCasillaInteresante0A(Sensores sensores) {
    int casilla=-1;
    for(int i=1; i<4; i++) {
      if(sensores.superficie[i]=='X' && ViablePorAlturaA(sensores.superficie[i], sensores.cota[i]-sensores.cota[0]) && sensores.agentes[i]!='r') { // puesto base
        casilla=i;
        return casilla;
      }
    }
    for(int i=1; i<4; i++) {
      if(sensores.superficie[i]=='C' && ViablePorAlturaA(sensores.superficie[i], sensores.cota[i]-sensores.cota[0]) && sensores.agentes[i]!='r') { // camino
        casilla=i;
        return casilla;
      }
    }
    /**
     * for(int i=1; i<4; i++) {
      if(sensores.superficie[i]=='S' && ViablePorAlturaA(sensores.superficie[i], sensores.cota[i]-sensores.cota[0])) { // sendero
        casilla=i;
        return casilla;
      }
    }
     */
    return casilla;
  }

  /**
   * NIVELES 0, 1
   * Devuelve si se puede ir a una casilla en función de la diferencia de altura.
   */
  bool ViablePorAlturaA(char casilla, int dif) {  
    if(abs(dif)<=1) 
      return true;
    else  
      return false;
  }

  /**
   * NIVEL 0
   * Devuelve si se puede ir a una casilla en función de qué tipo es, la diferencia de altura y si chocaría con el rescatador. 
   */
  bool ViableA(char casilla, int dif, Sensores sensores, int pos) {
    if(ViablePorAlturaA(casilla, dif) && (casilla=='X' || casilla=='C' /**or casilla =='A' or casilla=='T' or casilla=='S'*/) && sensores.agentes[pos]!='r') {
      return true;
    }
    else 
      return false;
  }

  /**
   * NIVEL 1
   * Devuelve si se puede ir a una casilla en función de qué tipo es, la diferencia de altura y si chocaría con el rescatador. 
   */
  bool Viable1A(char casilla, int dif, Sensores sensores, int pos) {
    if(importante || mismacasilla>15) {
      if(ViablePorAlturaA(casilla, dif) && ( casilla=='C'|| casilla=='S' ) && sensores.agentes[pos]!='r') {
        return true;
      }
      else 
        return false;
    }
    else {
      if(ViablePorAlturaA(casilla, dif) && (casilla=='C' || casilla=='S') && sensores.agentes[pos]!='r') {
        return true;
      }
      else 
        return false;
    }
    
  }

  /**
   * NIVEL 1
   * Devuelve si hay alguna casilla interesante a simple vista (inmediatamente delante) para el nivel 1 (camino y sendero). Si no, devuelve -1.
   */
  int VeoCasillaInteresante1A(Sensores sensores) {
    int casilla=-1;
    for(int i=1; i<4; i++) {
      if((sensores.superficie[i]=='C' && ViablePorAlturaA(sensores.superficie[i], sensores.cota[i]-sensores.cota[0])) || (sensores.superficie[i]=='S' && ViablePorAlturaA(sensores.superficie[i], sensores.cota[i]-sensores.cota[0]))) { // camino o sendero
        casilla=i;
        return casilla;
      }
    }
    return casilla;
  }

  /**
   * NIVEL 3
   */
  struct EstadoA {
    int f;
    int c;
    int brujula;
    bool zapatillas;
    bool operator==(const EstadoA &st) const{
    return f == st.f && c == st.c && brujula == st.brujula and zapatillas ==st.zapatillas;
    }
    bool operator<(const EstadoA & nuevo) const {
      if (f < nuevo.f) return true;
      else if (f == nuevo.f and c < nuevo.c) return true;
      else if (f == nuevo.f and c == nuevo.c and brujula < nuevo.brujula) return true;
      else if (f == nuevo.f and c == nuevo.c and brujula == nuevo.brujula and zapatillas < nuevo.zapatillas) return true;
      else return false;
    }
  };

  /**
   * NIVEL 3
   */
  struct NodoA{
    EstadoA estado;
    list<Action> secuencia;
    int coste;
    int heuristica;
    bool operator==(const NodoA &node) const{
      return estado == node.estado;
    }
    bool operator<(const NodoA &node) const{
      if (estado.f < node.estado.f) return true;
      else if (estado.f == node.estado.f and estado.c < node.estado.c) return true;
      else if (estado.f == node.estado.f and estado.c == node.estado.c and estado.brujula < node.estado.brujula) return true;
      else if (estado.f == node.estado.f and estado.c == node.estado.c and estado.brujula == node.estado.brujula and estado.zapatillas < node.estado.zapatillas) return true;
      else return false;
    }
    bool operator>(const NodoA &node) const{
      return (coste+heuristica)>(node.coste+node.heuristica);
    }
  };

  /**
   * NIVEL 3
   */
  void VisualizaPlan(const EstadoA &st, const list<Action> &plan);

  /**
   * NIVEL 3
   */
  void PintaPlan(const list<Action> &plan, bool zap);

  /**
   * NIVEL 3
   */
  list<Action> estrellaA(EstadoA &inicial, const EstadoA &objetivo, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura);

  /**
   * NIVEL 3
   * Devuelve un estado con la casilla siguiente al hacer WALK desde st.
   */
  EstadoA NextCasillaAuxiliar(const EstadoA &st){
    EstadoA siguiente = st;
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
    }
    return siguiente;
  }

  /**
   * NIVEl 3
   * Devuelve true si la casilla desde st al hacer WALK es accesible, false si no.
   */
  bool CasillaAccesibleAuxiliar(const EstadoA &st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura){
    EstadoA next = NextCasillaAuxiliar(st);
    if (next.f < 0 || next.f >= terreno.size() || next.c < 0 || next.c >= terreno[0].size()) {
      return false;
    }
    bool check1 = false, check2 = false, check3 = false;
    check1 = terreno[next.f][next.c] != 'P' and terreno[next.f][next.c] != 'M';
    check2 = terreno[next.f][next.c] != 'B' or (terreno[next.f][next.c] == 'B' and st.zapatillas);
    check3 = abs(altura[next.f][next.c] - altura[st.f][st.c]) <= 1;
    return check1 and check2 and check3;
  }

  /**
   * NIVEL 3
   */
  EstadoA applyA(Action accion, const EstadoA & st, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura){
    EstadoA next = st;
    switch(accion){
      case WALK:
        if (CasillaAccesibleAuxiliar(st,terreno,altura)){
          next = NextCasillaAuxiliar(st);
        }
      break;
      case TURN_SR:
        next.brujula = (next.brujula+1)%8;
      break;
    }
    if(terreno[next.f][next.c]=='D') {
      next.zapatillas=true;
    }
    return next;
  }

  /**
   * NIVEL 3
   * Heurística basada en la distancia aérea, tomando movimientos en las 8 direcciones principales.
   * No tiene en cuenta el tipo de terreno ni las diferencias de altura.
   */
  int HeuristicaA(const EstadoA& inicial, const EstadoA& final, const vector<vector<unsigned char>>& terreno, const vector<vector<unsigned char>>& altura) {
    int x=abs(final.f-inicial.f);
    int y=abs(final.c-inicial.c);
    int diagonal=min(x,y);
    int recto=abs(x-y);
    return diagonal+recto;
  }

  /**
   * NIVEL 4
   */
  bool AccionViableA(Action accion, Sensores sensores) {
    if(accion==TURN_SR or accion==IDLE) {
      return true;
    }
    unsigned char tipo_actual=sensores.superficie[0];
    int altura_actual=sensores.cota[0];
    int tipo_final=sensores.superficie[2];
    int altura_final=sensores.cota[2];
    if(tipo_final== 'P' or tipo_final=='M' or (tipo_final=='B' and !tiene_zapatillas)) {
      return false;
    }
    if(abs(altura_actual-altura_final)<=1) {
      return true;
    }
    return false;
  }

  /**
   * NIVELES 0, 1, 4
   * Rellena el contenido del mapa de superficie y cota con lo que puede ver en cada iteración.
   */
  void SituarSensorEnMapaA(vector<vector<unsigned char>> &m, vector<vector<unsigned char>> &a, Sensores sensores) {
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
  // Definir Variables de Estado
  Action last_action;
  bool tiene_zapatillas;
  int giro45Izq;  // giro45Izq=7 => TURN_SR x7
  bool importante;
  int mismacasilla;
  bool curva;
  int cuantoandando;

  bool hayPlan;
  list<Action> plan;
};

#endif
