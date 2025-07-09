#include "../Comportamientos_Jugador/rescatador.hpp"
#include "motorlib/util.h"
#include <iostream>
#include <queue>
#include <set>

using namespace std;

Action ComportamientoRescatador::think(Sensores sensores)
{
	Action accion = IDLE;

	switch (sensores.nivel)
	{
	case 0:
		accion = ComportamientoRescatadorNivel_0 (sensores);
		break;
	case 1:
		accion = ComportamientoRescatadorNivel_1 (sensores);
		break;
	case 2:
		accion = ComportamientoRescatadorNivel_2 (sensores);
		break;
	case 3:
		// accion = ComportamientoRescatadorNivel_3 (sensores);
		break;
	case 4:
		accion = ComportamientoRescatadorNivel_4 (sensores);
		break;
	}

	return accion;
}

int ComportamientoRescatador::interact(Action accion, int valor)
{
	return 0;
}

/**
 * NIVEL 0
 */
Action ComportamientoRescatador::ComportamientoRescatadorNivel_0(Sensores sensores)
{
	Action accion;
	// El comportamiento de seguir un camino hasta encontrar un puesto base.
	SituarSensorEnMapaR(mapaResultado, mapaCotas, sensores);
	actualizahePasado(sensores);
	int pos=VeoCasillaInteresante0R(sensores, tiene_zapatillas);
	// Actualización de variables de estado
	if(sensores.superficie[0]=='D') {
		tiene_zapatillas=true;
	}

	// Definición del comportamiento
	if(sensores.superficie[0]=='X') {
		accion=IDLE;
	} 
	else if(giro45Izq!=0) {
		accion=TURN_SR;
		giro45Izq--;
	}
	/**
	 * else if(muro<0) {
		if(ViableR(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], tiene_zapatillas, sensores, 2)) {
			accion=WALK;
		}
		else {
			if(pos_muro=1) {
				accion=TURN_L;
				giro45Izq=1;
			}
			else {
				accion=TURN_SR;
			}
		}
		muro++;
	}
	else if(muro==20) {	// girar 90º para empezar a seguir el muro
		int random=rand();
		if(random%2==0) {
			accion=TURN_SR;
			pos_muro=1;
		}
		else {
			accion=TURN_L;
			giro45Izq=1;
			pos_muro=3;
		}
		muro--;
	}
	else if(muro>0) {
		if(sensores.superficie[pos_muro]=='M' || sensores.superficie[pos_muro]=='P' || sensores.superficie[pos_muro]=='B' || !ViablePorAlturaR(sensores.superficie[pos_muro], sensores.cota[pos_muro]-sensores.cota[0], tiene_zapatillas)) {
			if (ViableR(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], tiene_zapatillas, sensores, 2)) {
				accion=WALK;
				muro--;
			}
			else {
				if(pos_muro==1) {
					accion=TURN_SR;
				}
				else {
					accion=TURN_L;
					giro45Izq=1;
				}
				muro=0;
			}
		}
		else {
			if(pos_muro==1) {
				accion=TURN_L;
				giro45Izq=1;
			}
			else {
				accion=TURN_SR;
			}
			muro=-2;
		}
	}
	 */
	else if(mismacasilla>15) {
		if(ViableR(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], tiene_zapatillas, sensores, 2)) {
			accion=WALK;
		}
		else {
			accion=TURN_SR;
			mismacasilla=0;
			giro45Izq=3;
		}
	}
	else if(cuantoandando>7) {
		int giro=rand()%3;
		if(giro==0 && ViableR(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], tiene_zapatillas, sensores, 2)) {
			accion=WALK;
		}
		else if(giro==1) {
			accion=TURN_SR;
		}
		else {
			accion=TURN_L;
			giro45Izq=1;	
		}
	}
	else if((sensores.superficie[1]=='C' && sensores.superficie[2]=='C' && sensores.superficie[3]=='C' && sensores.superficie[pos]=='C') /** || (sensores.superficie[1]=='S' && sensores.superficie[2]=='S' && sensores.superficie[3]=='S' && sensores.superficie[pos]=='S') */) {
		int siguiente=rand()%3;
		if(siguiente==0 && ViablePorAlturaR(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], tiene_zapatillas) && sensores.agentes[2]!='a') {
			accion=WALK;
		} 
		else if(siguiente==1) {
			accion=TURN_L;
			giro45Izq=1;
		}
		else {
			accion=TURN_SR;
		}
	}
	else if((sensores.superficie[1]=='C' && sensores.superficie[2]=='C' && sensores.superficie[pos]=='C') /** || (sensores.superficie[1]=='S' && sensores.superficie[2]=='S' && sensores.superficie[pos]=='S') */) {
		int siguiente=rand()%2;
		if(siguiente==0 && ViablePorAlturaR(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], tiene_zapatillas) && sensores.agentes[2]!='a') {
			accion=WALK;
		} 
		else {
			accion=TURN_L;
			giro45Izq=1;
		}
	}
	else if((sensores.superficie[3]=='C' && sensores.superficie[2]=='C' && sensores.superficie[pos]=='C') /** || (sensores.superficie[3]=='S' && sensores.superficie[2]=='S' && sensores.superficie[pos]=='S') */) {
		int siguiente=rand()%2;
		if(siguiente==0 && ViablePorAlturaR(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], tiene_zapatillas) && sensores.agentes[2]!='a') {
			accion=WALK;
		} 
		else {
			accion=TURN_SR;
		}
	}
	else {
		if(pos!=-1) {
			importante=true;
			if(sensores.superficie[1]=='X' && sensores.superficie[2]=='X' && ViablePorAlturaR(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], tiene_zapatillas) && sensores.agentes[2]!='a') {
				accion=WALK;
			}
			else if(sensores.superficie[1]=='D' && sensores.superficie[2]=='D' && ViablePorAlturaR(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], tiene_zapatillas) && sensores.agentes[2]!='a') {
				accion=WALK;
			}
			else if(pos==1) {
				accion=TURN_L;
				giro45Izq=1;
			}
			else if(pos==3) {
				accion=TURN_SR;
			}
			else if(ViableR(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], tiene_zapatillas, sensores, 2)) {
				accion=WALK;
			}
			else {
				int random=rand();
				if(random%2==0) {
					accion=TURN_SR;
				}
				else {
					accion=TURN_L;
					giro45Izq=1;
				}
			}
			importante=false;
		}
		else {
			if(ViableR(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], tiene_zapatillas, sensores, 2)) {
				accion=WALK;
			}
			else {
				int random=rand();
				if(random%2==0) {
					accion=TURN_SR;
				}
				else {
					accion=TURN_L;
					giro45Izq=1;
				}
			}
		}
		
	}
	
	if(accion==WALK) {
		mismacasilla=0;
		cuantoandando++;
	}
	else {
		mismacasilla++;
		cuantoandando=0;
	}

	// Devolver la siguiente acción
	last_action=accion;
	return accion;
}

/**
 * NIVEL 1
 */
Action ComportamientoRescatador::ComportamientoRescatadorNivel_1(Sensores sensores)
{
	Action accion;
	// El comportamiento de seguir un camino hasta encontrar un puesto base.
	SituarSensorEnMapaR(mapaResultado, mapaCotas, sensores);
	actualizahePasado(sensores);
	int pos=VeoCasillaInteresante1R(sensores, tiene_zapatillas);
	// Actualización de variables de estado
	if(sensores.superficie[0]=='D') {
		tiene_zapatillas=true;
	}

	// Definición del comportamiento
	if(giro45Izq!=0) {
		accion=TURN_SR;
		giro45Izq--;
	}
	else if(mismacasilla>15) {	
		if(Viable1R(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], tiene_zapatillas, sensores, 2)) {
			accion=WALK;
			mismacasilla=0;
		}
		else {
			accion=TURN_SR;
			giro45Izq=3;
			mismacasilla++;
		}
	}
	
	 else if(cuantoandando>7) {
		int giro=rand()%3;
		if(giro==0 && ViableR(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], tiene_zapatillas, sensores, 2)) {
			accion=WALK;
		}
		else if(giro==1) {
			accion=TURN_SR;
		}
		else {
			accion=TURN_L;
			giro45Izq=1;	
		}
	}

	else if((sensores.superficie[1]=='C' && sensores.superficie[2]=='C' && sensores.superficie[3]=='C' && sensores.superficie[pos]=='C') || (sensores.superficie[1]=='S' && sensores.superficie[2]=='S' && sensores.superficie[3]=='S' && sensores.superficie[pos]=='S')) {
		int siguiente=rand()%3;
		if(siguiente==0 && ViablePorAlturaR(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], tiene_zapatillas) && sensores.agentes[2]!='a') {
			accion=WALK;
		} 
		else if(siguiente==1) {
			accion=TURN_L;
			giro45Izq=1;
		}
		else {
			accion=TURN_SR;
		}
	}
	else if((sensores.superficie[1]=='C' && sensores.superficie[2]=='C' && sensores.superficie[pos]=='C') || (sensores.superficie[1]=='S' && sensores.superficie[2]=='S' && sensores.superficie[pos]=='S')) {
		int siguiente=rand()%2;
		if(siguiente==0 && ViablePorAlturaR(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], tiene_zapatillas) && sensores.agentes[2]!='a') {
			accion=WALK;
		} 
		else {
			accion=TURN_L;
			giro45Izq=1;
		}
	}
	else if((sensores.superficie[3]=='C' && sensores.superficie[2]=='C' && sensores.superficie[pos]=='C') || (sensores.superficie[3]=='S' && sensores.superficie[2]=='S' && sensores.superficie[pos]=='S')) {
		int siguiente=rand()%2;
		if(siguiente==0 && ViablePorAlturaR(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], tiene_zapatillas) && sensores.agentes[2]!='a') {
			accion=WALK;
		} 
		else {
			accion=TURN_SR;
		}
	}
	else {
		if(pos!=-1) {
			importante=true;
			/** if(sensores.superficie[1]=='X' && sensores.superficie[2]=='X' && ViablePorAlturaR(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], tiene_zapatillas) && sensores.agentes[2]!='a') {
				accion=WALK;
			}
			else  */
			if(sensores.superficie[1]=='D' && sensores.superficie[2]=='D' && ViablePorAlturaR(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], tiene_zapatillas) && sensores.agentes[2]!='a') {
				accion=WALK;
			}
			else if(pos==1) {
				accion=TURN_L;
				giro45Izq=1;
			}
			else if(pos==3) {
				accion=TURN_SR;
			}
			else if(Viable1R(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], tiene_zapatillas, sensores, 2)) {
				accion=WALK;
			}
			else {
				int random=rand();
				if(random%2==0) {
					accion=TURN_SR;
				}
				else {
					accion=TURN_L;
					giro45Izq=1;
				}
			}
			importante=false;
		}
		else {
			if(Viable1R(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], tiene_zapatillas, sensores, 2)) {
				accion=WALK;
			}
			else {
				int random=rand();
				if(random%2==0) {
					accion=TURN_SR;
				}
				else {
					accion=TURN_L;
					giro45Izq=1;
				}
			}
		}
		
	}
	
	if(accion==WALK) {
		mismacasilla=0;
		cuantoandando++;
	}
	else {
		mismacasilla++;
		cuantoandando=0;
	}

	// Devolver la siguiente acción
	last_action=accion;
	return accion;
}
  /**
   * NIVEL 2
   */
  void AnularMatrizR(vector<vector<unsigned char>> &m) {
    for (int i = 0; i < m[0].size(); i++)
    {
      for (int j = 0; j < m.size(); j++)
      {
        m[i][j] = 0;
      }
    }
  }

  /**
   * NIVEL 2
   */
  void ComportamientoRescatador::PintaPlan(const list<Action> &plan, bool zap) {
    auto it = plan.begin();
    while (it != plan.end())
    {
    if (*it == WALK)
    {
    cout << "W ";
    }
    else if (*it == RUN)
    {
    cout << "R ";
    }
    else if (*it == TURN_SR)
    {
    cout << "r ";
    }
    else if (*it == TURN_L)
    {
    cout << "L ";
    }
    else if (*it == CALL_ON)
    {
    cout << "C ";
    }
    else if (*it == CALL_OFF)
    {
    cout << "c ";
    }
    else if (*it == IDLE)
    {
    cout << "I ";
    }
    else
    {
    cout << "-_ ";
    }
    it++;
    }
    cout << "( longitud " << plan.size();
    if (zap) cout << "[Z]";
    cout << ")\n";
}

/**
 * NIVEL 2
 */
void ComportamientoRescatador::VisualizaPlan(const EstadoR &st, const list<Action> &plan) {
	AnularMatrizR(mapaConPlan);
	EstadoR cst = st;
	auto it = plan.begin();
	while (it != plan.end())
	{
	switch (*it)
	{
	case RUN:
	switch (cst.brujula)
	{
	case 0:
	cst.f--;
	break;
	case 1:
	cst.f--;
	cst.c++;
	break;
	case 2:
	cst.c++;
	break;
	case 3:
	cst.f++;
	cst.c++;
	break;
	case 4:
	cst.f++;
	break;
	case 5:
	cst.f++;
	cst.c--;
	break;
	case 6:
	cst.c--;
	break;
	case 7:
	cst.f--;
	cst.c--;
	break;
	}
	mapaConPlan[cst.f][cst.c] = 3;
	case WALK:
	switch (cst.brujula)
	{
	case 0:
	cst.f--;
	break;
	case 1:
	cst.f--;
	cst.c++;
	break;
	case 2:
	cst.c++;
	break;
	case 3:
	cst.f++;
	cst.c++;
	break;
	case 4:
	cst.f++;
	break;
	case 5:
	cst.f++;
	cst.c--;
	break;
	case 6:
	cst.c--;
	break;
	case 7:
	cst.f--;
	cst.c--;
	break;
	}
	mapaConPlan[cst.f][cst.c] = 1;
	break;
	case TURN_SR:
	cst.brujula = (cst.brujula + 1) % 8;
	break;
	case TURN_L:
	cst.brujula = (cst.brujula + 6) % 8;
	break;
	}
	it++;
	}
}

/**
   * NIVEL 2
   * Buscar el camino de Dijkstra teniendo en cuenta el consumo de energía.
   */
  list<Action> ComportamientoRescatador::dijkstraR(EstadoR& inicial, const EstadoR &objetivo, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura) {
    list<Action> elPlan;
    priority_queue<NodoR, vector<NodoR>, greater<NodoR>> abiertos;	// ordenados de mejor a peor en coste (menor a mayor)
	set<EstadoR> cerrados;
	abiertos.push({inicial, {}, 0});
	if(terreno[inicial.f][inicial.c]=='D') {
		inicial.zapatillas=true;
	}
	//int contador=0;
	while(!abiertos.empty()) {
		//cout << contador << endl;
		NodoR comprobando=abiertos.top();
		abiertos.pop();
		EstadoR actual=comprobando.estado;
		if(cerrados.find(actual)==cerrados.end()){	// explorar comprobando
			cerrados.insert(actual);
			if(actual.f==objetivo.f && actual.c==objetivo.c) {	// llegamos al objetivo
				return comprobando.secuencia;
			}
			unsigned char tipo=terreno[actual.f][actual.c];

			// andar
			EstadoR andar=applyR(WALK, actual ,terreno, altura);
			if(!(andar==actual) && CasillaAccesibleR(actual, andar, terreno, altura) && cerrados.find(andar)==cerrados.end()) {
				int coste_nuevo=comprobando.coste;
				int altura_inicial=altura[actual.f][actual.c];
        		int altura_final=altura[andar.f][andar.c];
        		int dif=altura_final-altura_inicial;
				if (tipo=='A') coste_nuevo+=100+10*dif;
                else if (tipo=='T') coste_nuevo+=20+5*dif;
                else if (tipo=='S') coste_nuevo+=2+1*dif;
                else coste_nuevo+=1;
				list<Action> nueva_secuencia=comprobando.secuencia;
                nueva_secuencia.push_back(WALK);
                abiertos.push({andar, nueva_secuencia, coste_nuevo});
				//cout << actual.f << " " << actual.c << " " << andar.f << " " << andar.c << endl;
			}

			// correr
			EstadoR correr=applyR(RUN, actual ,terreno, altura);
			unsigned char tipo_intermedia=terreno[andar.f][andar.c];
			if(!(correr==actual) && CasillaAccesibleR(actual, correr, terreno, altura) && cerrados.find(correr)==cerrados.end() && tipo_intermedia!='P' && tipo_intermedia!='M' && tipo_intermedia!='B') {
				int coste_nuevo=comprobando.coste;
				int altura_inicial=altura[actual.f][actual.c];
        		int altura_final=altura[correr.f][correr.c];
        		int dif=altura_final-altura_inicial;
				if (tipo=='A') coste_nuevo+=150+15*dif;
                else if (tipo=='T') coste_nuevo+=35+5*dif;
                else if (tipo=='S') coste_nuevo+=3+2*dif;
                else coste_nuevo+=1;
				list<Action> nueva_secuencia=comprobando.secuencia;
                nueva_secuencia.push_back(RUN);
                abiertos.push({correr, nueva_secuencia, coste_nuevo});
				//cout << actual.f << " " << actual.c << " " << correr.f << " " << correr.c << endl;
			}

			// giro derecha
			EstadoR giro_sr=applyR(TURN_SR, actual ,terreno, altura);
			if(cerrados.find(giro_sr)==cerrados.end()) {
				int coste_nuevo=comprobando.coste;
				if (tipo=='A') coste_nuevo+=16;
                else if (tipo=='T') coste_nuevo+=3;
                else if (tipo=='S') coste_nuevo+=1;
                else coste_nuevo+=1;
				list<Action> nueva_secuencia=comprobando.secuencia;
                nueva_secuencia.push_back(TURN_SR);
                abiertos.push({giro_sr, nueva_secuencia, coste_nuevo});
				//cout << actual.f << " " << actual.c << " " << giro_sr.f << " " << giro_sr.c << endl;
			}

			// giro izquierda
			EstadoR giro_l=applyR(TURN_L, actual ,terreno, altura);
			if(cerrados.find(giro_l)==cerrados.end()) {
				int coste_nuevo=comprobando.coste;
				if (tipo=='A') coste_nuevo+=30;
                else if (tipo=='T') coste_nuevo+=5;
                else if (tipo=='S') coste_nuevo+=1;
                else coste_nuevo+=1;
				list<Action> nueva_secuencia=comprobando.secuencia;
                nueva_secuencia.push_back(TURN_L);
                abiertos.push({giro_l, nueva_secuencia, coste_nuevo});
				//cout << actual.f << " " << actual.c << " " << giro_l.f << " " << giro_l.c << endl;
			}
			//contador++;
		}
	}
	return elPlan;
  }

/**
 * NIVEL 2
 */
Action ComportamientoRescatador::ComportamientoRescatadorNivel_2(Sensores sensores)
{
	Action accion=IDLE;
	if(!hayPlan) {
		EstadoR inicio, fin;
		inicio.f=sensores.posF;
		inicio.c=sensores.posC;
		inicio.brujula=sensores.rumbo;
		inicio.zapatillas=tiene_zapatillas;
		fin.f=sensores.destinoF;
		fin.c=sensores.destinoC;

		plan=dijkstraR(inicio, fin, mapaResultado, mapaCotas);
		VisualizaPlan(inicio, plan);
		PintaPlan(plan, tiene_zapatillas);
		hayPlan=true;
	}
	if(hayPlan && plan.size()>0) {
		accion=plan.front();
		plan.pop_front();
	}
	if(plan.size()==0) {
		hayPlan=false;
	}
	return accion;
}

/**
 * NIVEL 3
 */
Action ComportamientoRescatador::ComportamientoRescatadorNivel_3(Sensores sensores)
{
}

/**
 * NIVEL 4
 */
list<Action> ComportamientoRescatador::estrellaR(EstadoR origen, EstadoR destino, const vector<vector<unsigned char>> &m, const vector<vector<unsigned char>> &a) {
	list<Action> elPlan;
	priority_queue<Nodo4, vector<Nodo4>, greater<Nodo4>> abiertos;
	set<EstadoR> cerrados;
	abiertos.push({origen, {}, 0, HeuristicaR(origen, destino)});
  return elPlan; 
}

/**
 * NIVEL 4
 */
Action ComportamientoRescatador::ComportamientoRescatadorNivel_4(Sensores sensores)
{
	Action accion=IDLE;
	
	return accion;
}