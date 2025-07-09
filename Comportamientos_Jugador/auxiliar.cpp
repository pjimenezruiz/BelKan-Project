#include "../Comportamientos_Jugador/auxiliar.hpp"
#include <iostream>
#include <set>
#include <queue>
#include "motorlib/util.h"

Action ComportamientoAuxiliar::think(Sensores sensores)
{
	Action accion = IDLE;

	switch (sensores.nivel)
	{
	case 0:
		accion = ComportamientoAuxiliarNivel_0 (sensores);
		break;
	case 1:
		accion = ComportamientoAuxiliarNivel_1 (sensores);
		break;
	case 2:
		// accion = ComportamientoAuxiliarNivel_2 (sensores);
		break;
	case 3:
		accion = ComportamientoAuxiliarNivel_3 (sensores);
		break;
	case 4:
		accion = ComportamientoAuxiliarNivel_4 (sensores);
		break;
	}

	return accion;
}

int ComportamientoAuxiliar::interact(Action accion, int valor)
{
	return 0;
}

/**
 * NIVEL 0
 */
Action ComportamientoAuxiliar::ComportamientoAuxiliarNivel_0(Sensores sensores)
{
	// El comportamiento de seguir un camino hasta encontrar un puesto base.
	Action accion;	
	SituarSensorEnMapaA(mapaResultado, mapaCotas, sensores);
	// Actualización de variables de estado
	//if(sensores.superficie[0]=='D') {
	//	tiene_zapatillas=true;
	//}
	int pos=VeoCasillaInteresante0A(sensores);
	
	// Definición del comportamiento
	if(sensores.superficie[0]=='X') {
		accion=IDLE;
	} 
	else if(giro45Izq!=0) {
		accion=TURN_SR;
		giro45Izq--;
	}
	/**
	 * if((sensores.superficie[2]=='P' || sensores.superficie[2]=='M' || sensores.superficie[pos_muro]=='B' || !ViablePorAlturaA(sensores.superficie[2], sensores.cota[2]-sensores.cota[0])) && muro==0) {
		muro=20;
		accion=IDLE;
	}
	else if(muro<0) {
		if(ViableA(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], sensores, 2)) {
			accion=WALK;
		}
		else {
			if(pos_muro=1) {
				accion=TURN_SR;
				giro45Izq=6;
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
			accion=TURN_SR;
			giro45Izq=6;
			pos_muro=3;
		}
		muro--;
	}
	else if(muro>0) {
		if(sensores.superficie[pos_muro]=='M' || sensores.superficie[pos_muro]=='P' || sensores.superficie[pos_muro]=='B' || !ViablePorAlturaA(sensores.superficie[pos_muro], sensores.cota[pos_muro]-sensores.cota[0])) {
			if (ViableA(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], sensores, 2)) {
				accion=WALK;
				muro--;
			}
			else {
				if(pos_muro==1) {
					accion=TURN_SR;
				}
				else {
					accion=TURN_SR;
					giro45Izq=6;
				}
				muro=0;
			}
		}
		else {
			if(pos_muro==1) {
				accion=TURN_SR;
				giro45Izq=6;
			}
			else {
				accion=TURN_SR;
			}
			muro=-2;
		}
	}
	 */
	else if(mismacasilla>7) {
		if(ViableA(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], sensores, 2)) {
			accion=WALK;
		}
		else {
			accion=TURN_SR;
			mismacasilla=0;
			giro45Izq=3;
		}
	}
	else if(cuantoandando>15) {
		int giro=rand()%3;
		if(giro==0 && ViableA(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], sensores, 2)) {
			accion=WALK;
		}
		else if(giro==1) {
			accion=TURN_SR;
		}
		else {
			accion=TURN_SR;
			giro45Izq=5;	
		}
	}
	else if((sensores.superficie[1]=='C' && sensores.superficie[2]=='C' && sensores.superficie[3]=='C' && sensores.superficie[pos]=='C') /** || (sensores.superficie[1]=='S' && sensores.superficie[2]=='S' && sensores.superficie[3]=='S' && sensores.superficie[pos]=='S') */) {
		int siguiente=rand()%6;
		if(siguiente<4 && ViablePorAlturaA(sensores.superficie[2], sensores.cota[2]-sensores.cota[0])) {
			accion=WALK;
		} 
		else if(siguiente==4) {
			accion=TURN_SR;
			giro45Izq=6;
		}
		else {
			accion=TURN_SR;
		}
	}
	else if((sensores.superficie[1]=='C' && sensores.superficie[2]=='C' && sensores.superficie[pos]=='C') /** || (sensores.superficie[1]=='S' && sensores.superficie[2]=='S' && sensores.superficie[pos]=='S') */) {
		int siguiente=rand()%2;
		if(siguiente==0 && ViablePorAlturaA(sensores.superficie[2], sensores.cota[2]-sensores.cota[0])) {
			accion=WALK;
		} 
		else {
			accion=TURN_SR;
			giro45Izq=6;
		}
	}
	else if((sensores.superficie[3]=='C' && sensores.superficie[2]=='C' && sensores.superficie[pos]=='C') /** || (sensores.superficie[3]=='S' && sensores.superficie[2]=='S' && sensores.superficie[pos]=='S') */) {
		int siguiente=rand()%2;
		if(siguiente==0 && ViablePorAlturaA(sensores.superficie[2], sensores.cota[2]-sensores.cota[0])) {
			accion=WALK;
		} 
		else {
			accion=TURN_SR;
		}
	}
	else {
		if(pos!=-1) {
			importante=true;
			if(sensores.superficie[1]=='X' && sensores.superficie[2]=='X' && ViablePorAlturaA(sensores.superficie[2], sensores.cota[2]-sensores.cota[0])) {
				accion=WALK;
			}
			else if(pos==1) {
				accion=TURN_SR;
				giro45Izq=6;
			}
			else if(pos==3) {
				accion=TURN_SR;
			}
			else if(ViableA(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], sensores, 2)) {
				accion=WALK;
			}
			else {
				int random=rand();
				if(random%2==0) {
					accion=TURN_SR;
				}
				else {
					accion=TURN_SR;
					giro45Izq=6;
				}
			}
			importante=false;
		}
		else {
			if(ViableA(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], sensores, 2)) {
				accion=WALK;
			}
			else {
				int random=rand();
				if(random%2==0) {
					accion=TURN_SR;
				}
				else {
					accion=TURN_SR;
					giro45Izq=5;
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
Action ComportamientoAuxiliar::ComportamientoAuxiliarNivel_1(Sensores sensores)
{
	// El comportamiento de descubrir la mayor cantidad de casillas camino y sendero en el mapa.
	Action accion;	
	SituarSensorEnMapaA(mapaResultado, mapaCotas, sensores);
	int pos=VeoCasillaInteresante1A(sensores);
	// Actualización de variables de estado
	// Definición del comportamiento
	if(giro45Izq!=0) {
		if(giro45Izq!=0) {
			accion=TURN_SR;
			giro45Izq--;
		}
	}
	else if(mismacasilla>15) {
		accion=TURN_SR;
		giro45Izq=3;
		mismacasilla=0;
	}
	else if(cuantoandando>7) {
		int giro=rand()%3;
		if(giro==0 && ViableA(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], sensores, 2)) {
			accion=WALK;
		}
		else if(giro==1) {
			accion=TURN_SR;
		}
		else {
			accion=TURN_SR;
			giro45Izq=6;	
		}
	}
	else if((sensores.superficie[1]=='C' && sensores.superficie[2]=='C' && sensores.superficie[3]=='C' && sensores.superficie[pos]=='C')  || (sensores.superficie[1]=='S' && sensores.superficie[2]=='S' && sensores.superficie[3]=='S' && sensores.superficie[pos]=='S') ) {
		int siguiente=rand()%6;
		if(siguiente<4 && ViablePorAlturaA(sensores.superficie[2], sensores.cota[2]-sensores.cota[0])) {
			accion=WALK;
		} 
		else if(siguiente==4) {
			accion=TURN_SR;
			giro45Izq=6;
		}
		else {
			accion=TURN_SR;
		}
	}
	else if((sensores.superficie[1]=='C' && sensores.superficie[2]=='C' && sensores.superficie[pos]=='C') || (sensores.superficie[1]=='S' && sensores.superficie[2]=='S' && sensores.superficie[pos]=='S') ) {
		int siguiente=rand()%2;
		if(siguiente==0 && ViablePorAlturaA(sensores.superficie[2], sensores.cota[2]-sensores.cota[0])) {
			accion=WALK;
		} 
		else {
			accion=TURN_SR;
			giro45Izq=6;
		}
	}
	else if((sensores.superficie[3]=='C' && sensores.superficie[2]=='C' && sensores.superficie[pos]=='C') || (sensores.superficie[3]=='S' && sensores.superficie[2]=='S' && sensores.superficie[pos]=='S') ) {
		int siguiente=rand()%2;
		if(siguiente==0 && ViablePorAlturaA(sensores.superficie[2], sensores.cota[2]-sensores.cota[0])) {
			accion=WALK;
		} 
		else {
			accion=TURN_SR;
		}
	}
	else {
		if(pos!=-1) {
			importante=true;
			/** if(sensores.superficie[1]=='X' && sensores.superficie[2]=='X' && ViablePorAlturaA(sensores.superficie[2], sensores.cota[2]-sensores.cota[0])) {
				accion=WALK;
			}
			else  */
			if(pos==1) {
				accion=TURN_SR;
				giro45Izq=6;
			}
			else if(pos==3) {
				accion=TURN_SR;
			}
			else if(Viable1A(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], sensores, 2)) {
				accion=WALK;
			}
			else {
				int random=rand();
				if(random%2==0) {
					accion=TURN_SR;
				}
				else {
					accion=TURN_SR;
					giro45Izq=6;
				}
			}
			importante=false;
		}
		else {
			if(Viable1A(sensores.superficie[2], sensores.cota[2]-sensores.cota[0], sensores, 2)) {
				accion=WALK;
			}
			else {
				int random=rand();
				if(random%2==0) {
					accion=TURN_SR;
				}
				else {
					accion=TURN_SR;
					giro45Izq=6;
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
Action ComportamientoAuxiliar::ComportamientoAuxiliarNivel_2(Sensores sensores)
{
}

/**
 * NIVEL 3
 */
void AnularMatrizA(vector<vector<unsigned char>> &m) {
	for (int i = 0; i < m[0].size(); i++)
	{
	for (int j = 0; j < m.size(); j++)
	{
	m[i][j] = 0;
	}
	}
}

/**
 * NIVEL 3
 */
void ComportamientoAuxiliar::VisualizaPlan(const EstadoA &st, const list<Action> &plan) {
	AnularMatrizA(mapaConPlan);
	EstadoA cst = st;
	auto it = plan.begin();
	while (it != plan.end())
	{
	switch (*it)
	{
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
	mapaConPlan[cst.f][cst.c] = 2;
	break;
	case TURN_SR:
	cst.brujula = (cst.brujula + 1) % 8;
	break;
	}
	it++;
	}
}

/**
 * NIVEL 3
 */
void ComportamientoAuxiliar::PintaPlan(const list<Action> &plan, bool zap) {
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
 * NIVEL 3
 */
list<Action> ComportamientoAuxiliar::estrellaA(EstadoA &inicial, const EstadoA &objetivo, const vector<vector<unsigned char>> &terreno, const vector<vector<unsigned char>> &altura) {
	list<Action> elPlan;
	priority_queue<NodoA, vector<NodoA>, greater<NodoA>> abiertos;	// ordenados de mejor a peor en coste (menor a mayor)
	set<EstadoA> cerrados;
	abiertos.push({inicial, {}, 0, HeuristicaA(inicial, objetivo, terreno, altura)});
	if(terreno[inicial.f][inicial.c]=='D') {
		inicial.zapatillas=true;
	}
	while(!abiertos.empty()) {
		NodoA comprobando=abiertos.top();
		abiertos.pop();
		EstadoA actual=comprobando.estado;
		if(cerrados.find(actual)==cerrados.end()) {
			cerrados.insert(actual);
			if(actual.f==objetivo.f && actual.c==objetivo.c) {	// llegamos al objetivo
				return comprobando.secuencia;
			}
			unsigned char tipo=terreno[actual.f][actual.c];

			// andar
			EstadoA andar=applyA(WALK, actual, terreno, altura);
			if(!(andar==actual) && cerrados.find(andar)==cerrados.end()) {
				int coste_nuevo=comprobando.coste;
				int heuristica=HeuristicaA(andar, objetivo, terreno, altura);
				int altura_inicial = altura[actual.f][actual.c];
        		int altura_final = altura[andar.f][andar.c];
        		int dif=altura_final-altura_inicial;
				if (tipo=='A') coste_nuevo+=100+10*dif;
                else if (tipo=='T') coste_nuevo+=20+5*dif;
                else if (tipo=='S') coste_nuevo+=2+1*dif;
                else coste_nuevo+=1;
				list<Action> nueva_secuencia=comprobando.secuencia;
                nueva_secuencia.push_back(WALK);
                abiertos.push({andar, nueva_secuencia, coste_nuevo, heuristica});
				//cout << actual.f << " " << actual.c << " " << andar.f << " " << andar.c << endl;
			}

			// giro derecha
			EstadoA giro_sr=applyA(TURN_SR, actual ,terreno, altura);
			if(cerrados.find(giro_sr)==cerrados.end()) {
				int coste_nuevo=comprobando.coste;
				int heuristica=HeuristicaA(giro_sr, objetivo, terreno, altura);
				if (tipo=='A') coste_nuevo+=16;
                else if (tipo=='T') coste_nuevo+=3;
                else if (tipo=='S') coste_nuevo+=1;
                else coste_nuevo+=1;
				list<Action> nueva_secuencia=comprobando.secuencia;
                nueva_secuencia.push_back(TURN_SR);
                abiertos.push({giro_sr, nueva_secuencia, coste_nuevo, heuristica});
				//cout << actual.f << " " << actual.c << " " << giro_sr.f << " " << giro_sr.c << endl;
			}
		}
	}

	return elPlan;
}

/**
 * NIVEL 3
 */
Action ComportamientoAuxiliar::ComportamientoAuxiliarNivel_3(Sensores sensores)
{
	Action accion=IDLE;

	if(!hayPlan) {
		//cout << "NO PLAN" << endl;
		EstadoA inicio, fin;
		inicio.f = sensores.posF;
		inicio.c = sensores.posC;
		inicio.brujula = sensores.rumbo;
		inicio.zapatillas = tiene_zapatillas;
		fin.f = sensores.destinoF;
		fin.c = sensores.destinoC;

		plan = estrellaA(inicio, fin, mapaResultado, mapaCotas);
		//cout << "PLAN" << plan.front() << endl;
		VisualizaPlan(inicio, plan);
		PintaPlan(plan, tiene_zapatillas);
		hayPlan=true;
	}
	if(hayPlan && plan.size()>0) {
		//cout << "SÍ PLAN" << endl;
		accion=plan.front();
		plan.pop_front();
	}
	if(plan.size()==0) {
		hayPlan=false;
	}

	return accion;
}

/**
 * NIVEL 4
 */
Action ComportamientoAuxiliar::ComportamientoAuxiliarNivel_4(Sensores sensores)
{
	Action accion=IDLE;
	return accion;
}