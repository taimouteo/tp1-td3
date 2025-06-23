#include <vector>

#include "lib.h"
#include "calendario.h"
#include "billetera.h"
#include "blockchain.h"

using namespace std;

Billetera::Billetera(const id_billetera id, Blockchain* blockchain)
  : _id(id)
  , _blockchain(blockchain)
  , _saldo(0)
  , _dia_apertura(-1)
{}

id_billetera Billetera::id() const {                                                    // Función: O(1)
  return _id;                                                                           // O(1)
}

void Billetera::notificar_transaccion(Transaccion t) {                                  // Función: O(D + C)
  /*
   * Necesito:
   *  - Actualizar el saldo actual.
   *  - Agregar la transacción a las ultimas transacciones.
   *  - Actualizar el saldo del dia de la transaccion.
   *  - Aumentar el número de transacciones al destinatario.
   *  - Ordenar las transacciones por destinatario en orden descendiente.
   */

  // Si es la primera transferencia, guarda el dia de apertura de la billetera.
  if(_dia_apertura == -1) {                                                             // O(1)
    _dia_apertura = t._timestamp/86400;                                                 // O(1)
  }

  int dia_transferencia = t._timestamp/86400;                                           // O(1)
  int index = dia_transferencia - _dia_apertura;                                        // O(1)

  // Agrega al vector el saldo de cada uno de los dias entre la ultima transferencia y la nueva.
  while(index >= _saldo_al_fin_del_dia.size()) {                                        // O(1). D iteraciones => O(D)
   _saldo_al_fin_del_dia.push_back(_saldo);                                             // O(1)
  }

  if(t.origen == _id) {                                                                 // O(1)
    /* Actualizo el saldo actual. */
    _saldo -= t.monto;                                                                  // O(1)
    /* Actualizo el saldo del dia de la transaccion. */
    _saldo_al_fin_del_dia[index] -= t.monto;                                            // O(1)
    
    /* Aumento el número de transacciones al destinatario. */
    bool aumentado = false;                                                             // O(1)
    int posicion_destinatario;                                                          // O(1)
    
    // Si el destinatario está en el vector, aumento su #transferencias.
    for(int i = 0; i < _transferencias_por_destinatario.size(); ++i) {                  // O(1). C iteraciones => O(C)
      if(_transferencias_por_destinatario[i].first == t.destino) {                      // O(1)
        _transferencias_por_destinatario[i].second++;                                   // O(1)
        posicion_destinatario = i;                                                      // O(1)
        aumentado = true;                                                               // O(1)
      }
    }

    // Si no está en el vector, lo agrego al final.
    if(!aumentado) {
      pair<id_billetera,int> nuevo_destinatario;                                        // O(1)
      nuevo_destinatario.first = t.destino;                                             // O(1)
      nuevo_destinatario.second = 1;                                                    // O(1)
      _transferencias_por_destinatario.push_back(nuevo_destinatario);                   // O(1)
      posicion_destinatario = _transferencias_por_destinatario.size()-1;                // O(1)
    }

    /* Ordeno las transacciones por destinatario en orden descendiente. */
    // Implementamos el algoritmo BubbleSort modificado, para ubicar al destinatario
    // de manera descendente con respecto a #transferencias y creciente en antiguedad.
  
    while(posicion_destinatario != 0 && _transferencias_por_destinatario[posicion_destinatario-1].second <= _transferencias_por_destinatario[posicion_destinatario].second) {
      // O(1). C iteraciones en el peor caso => O(C).
      swap(_transferencias_por_destinatario[posicion_destinatario-1],_transferencias_por_destinatario[posicion_destinatario]);  // O(1)
      posicion_destinatario--;                                                          // O(1)
    }
  
    // Complejidad del if: O(1)*14 + O(C)*2 
    // Prop: k.f1 ∈ O(g1), f1 ∈ O(g1)
    // = O(1) + O(C)
    // Prop: f1 + f2 ∈ O(max{g1,g2}), f1 ∈ O(g1), f2 ∈ O(g2).
    // = O(C)
  } else {
    _saldo += t.monto;                                                                  // O(1)
    _saldo_al_fin_del_dia[index] += t.monto;                                            // O(1)

    // Complejidad del else: O(1)*2
    // Prop: k.f1 ∈ O(g1), f1 ∈ O(g1)
    // = O(1)
  }

  /* Agrego la transacción a las ultimas transacciones. */
  _ultimas_transacciones.push_back(t);                                                  // O(1)

  // Complejidad de la función:
  // O(1)*6 + O(D) + O(C)
  // Prop: k.f1 ∈ O(g1), f1 ∈ O(g1)
  // = O(1) + O(D) + O(C)
  // f1 + f2 ∈ O(max{g1,g2}), f1 ∈ O(g1), f2 ∈ O(g2).
  // = O(D + C)
}


monto Billetera::saldo() const {                                                        // Función: O(1)
  return _saldo;                                                                        // O(1)
}

monto Billetera::saldo_al_fin_del_dia(timestamp t) const {                              // Función: O(1)
  int dia_chequear = (Calendario::principio_del_dia(t))/86400;                          // O(1)
  int dia_desde_apertura = dia_chequear - _dia_apertura;                                // O(1)

  // Si el ultimo indice del vector de saldos es menor que el dia a chequear, significa que el dia no esta
  // registrado en el vector de saldos y entonces devolvemos el ultimo saldo registrado.
  if(dia_desde_apertura <= _saldo_al_fin_del_dia.size()-1) return _saldo_al_fin_del_dia[dia_desde_apertura];  //O(1)
  else {return _saldo_al_fin_del_dia[_saldo_al_fin_del_dia.size()-1];}                  // O(1)

  // O(1)*4
  // Prop: k.f1 ∈ O(g1), f1 ∈ O(g1)
  // = O(1)
}

vector<Transaccion> Billetera::ultimas_transacciones(int k) const {                     // Función: O(K)
  vector<Transaccion> primerasKtransacc;                                                // O(1)
  for(int i = 0; i < _ultimas_transacciones.size() && i < k; ++i) {                     // O(1). K iteraciones => O(K)
    primerasKtransacc.push_back(_ultimas_transacciones[_ultimas_transacciones.size()-1-i]); // O(1)
  }
  
  return primerasKtransacc;                                                             // O(1)

  // O(1)*2 + O(K)
  // Prop: k.f1 ∈ O(g1), f1 ∈ O(g1)
  // = O(1) + O(K)
  // Prop: f1 + f2 ∈ O(max{g1,g2}), f1 ∈ O(g1), f2 ∈ O(g2).
  // = O(K)
}

vector<id_billetera> Billetera::detinatarios_mas_frecuentes(int k) const {              // Función: O(K)
  vector<id_billetera> primerosKdestinatarios;                                          // O(1)
  for(int i = 0; i < _transferencias_por_destinatario.size() && i < k; i++) {           // O(1). K iteraciones => O(K)
    primerosKdestinatarios.push_back(_transferencias_por_destinatario[i].first);        // O(1)
  }
  return primerosKdestinatarios;                                                        // O(1)

  // O(1)*2 + O(K)
  // Prop: k.f1 ∈ O(g1), f1 ∈ O(g1)
  // O(1) + O(K)
  // f1 + f2 ∈ O(max{g1,g2}), f1 ∈ O(g1), f2 ∈ O(g2).
  // = O(K)
}