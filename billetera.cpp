#include <vector>

#include "lib.h"
#include "calendario.h"
#include "billetera.h"
#include "blockchain.h"

using namespace std;

/**
 * To-Do:
 *  - Todos los dias, por más que no se modifique el saldo (no hay transferencias), tienen que estar en saldo
 *    por dia. SOLUCION: usar la función dia() y cada vez que se agregue una transferencia nueva, añadir todos
 *    los dias anteriores (que no esten en el map). OTRA SOLUCION (más facil): que en la funcion saldo_al_final_del_dia,
 *    si el dia buscado no es una clave del map, que devuelva el dia anterior mas proximo.
 *  - Usar la funcion dia() para hacer más legible el código (en notificar_transacciones).
 *  - Debugear los tests. Trabado en si_consulto_saldo_entre_creacion_y_primera_transaccion_real_retorna_el_saldo_inicial
 *  - Chequear y actualizar las complejidades.
 *  - Hacer el código más entendible.
 */

Billetera::Billetera(const id_billetera id, Blockchain* blockchain)
  : _id(id)
  , _blockchain(blockchain)
  , _saldo(0) 
{}

id_billetera Billetera::id() const {
  return _id;
}

void Billetera::notificar_transaccion(Transaccion t) { // O(log D + log C)
  /**
   * Necesito:
   *  - Actualizar el saldo actual.
   *  - Agregar la transacción a las ultimas transacciones.
   *  - Actualizar el saldo del dia de la transaccion.
   *  - Aumentar el número de transacciones al destinatario.
   *  - Ordenar las transacciones por destinatario en orden descendiente.
   */

  timestamp dia_transferencia = Calendario::principio_del_dia(t._timestamp);    // O(1)
  timestamp dia_ultima_transferencia;
  if(_ultimas_transacciones.size()==0) {_saldo_al_fin_del_dia[dia_transferencia] = 0;}
  else {
    dia_ultima_transferencia = Calendario::principio_del_dia(_ultimas_transacciones[_ultimas_transacciones.size()-1]._timestamp);
    if(dia_transferencia != dia_ultima_transferencia) {
      _saldo_al_fin_del_dia[dia_transferencia] = _saldo;
    }
  }

  if(t.origen == _id) {                                                         // O(1)
    _saldo -= t.monto;                                                          // O(1)
    _saldo_al_fin_del_dia[dia_transferencia] -= t.monto;
    
    /** Aumento el número de transacciones al destinatario. */
    bool aumentado = false;                                                     // O(1)
    int posicion_destinatario;                                                  // O(1)
    
    // Si el destinatario está en el vector, aumento su #transferencias.
    for(int i = 0; i < _transferencias_por_destinatario.size(); ++i) {          // O(1). C iteraciones => O(C)
      if(_transferencias_por_destinatario[i].first == t.destino) {              // O(1)
        _transferencias_por_destinatario[i].second++;                           // O(1)
        posicion_destinatario = i;                                              // O(1)
        aumentado = true;                                                       // O(1)
      }
    }

    // Si no está en el vector, lo agrego al final.
    if(!aumentado) {
      pair<id_billetera,int> nuevo_destinatario;                                // O(1)
      nuevo_destinatario.first = t.destino;                                     // O(1)
      nuevo_destinatario.second = 1;                                            // O(1)
      _transferencias_por_destinatario.push_back(nuevo_destinatario);           // O(1) amortizado
      posicion_destinatario = _transferencias_por_destinatario.size()-1;        // O(1)
    }

    // Implementamos el algoritmo BubbleSort modificado, para ubicar al destinatario
    // de manera descendente con respecto a #transferencias y creciente en antiguedad.
  
    while(posicion_destinatario != 0 && _transferencias_por_destinatario[posicion_destinatario-1].second <= _transferencias_por_destinatario[posicion_destinatario].second) {
      // O(1). C iteraciones en el peor caso => O(C).
      swap(_transferencias_por_destinatario[posicion_destinatario-1],_transferencias_por_destinatario[posicion_destinatario]);
      posicion_destinatario--;
      // O(1)
    }
  
  // Complejidad del if: O(1)*8 + O(log(D)) + O(C) + O(C)
  // f1 + f2 ∈ O(max{g1,g2}), f1 ∈ O(g1), f2 ∈ O(g2).
  // = O(log(D) + C)
   

  } else {
    _saldo += t.monto;                                                          // O(1)
    _saldo_al_fin_del_dia[dia_transferencia] += t.monto;                        // O(log(D)) en el peor caso.

    // Complejidad del else: O(1) + O(log(D))
    // f1 + f2 ∈ O(max{g1,g2}), f1 ∈ O(g1), f2 ∈ O(g2).
    // = O(log(D))

  }

  _ultimas_transacciones.push_back(t);                                          // O(1) amortizado.

  // O(1) + O(1) + O(log(D) + C) + O(log(D))
  // f1 + f2 ∈ O(max{g1,g2}), f1 ∈ O(g1), f2 ∈ O(g2).
  // = O(log(D) + C)
}


monto Billetera::saldo() const { // O(1)
  return _saldo;
}

monto Billetera::saldo_al_fin_del_dia(timestamp t) const {
  timestamp dia_transferencia = Calendario::principio_del_dia(t);                       // O(1)
  return _saldo_al_fin_del_dia.at(dia_transferencia);                                   // O(log D)

  // O(1) + O(log(D))
  // f1 + f2 ∈ O(max{g1,g2}), f1 ∈ O(g1), f2 ∈ O(g2).
  // = O(log(D))
}

vector<Transaccion> Billetera::ultimas_transacciones(int k) const {
  vector<Transaccion> primerasKtransacc;                                                    // O(1)
  for(int i = 0; i < _ultimas_transacciones.size() && i < k; ++i) {                         // O(1). k iteraciones => O(k)
    primerasKtransacc.push_back(_ultimas_transacciones[_ultimas_transacciones.size()-1-i]); // O(1)
  }
  
  return primerasKtransacc;                                                                 // O(1)

  // O(1) + O(k) + O(1)
  // f1 + f2 ∈ O(max{g1,g2}), f1 ∈ O(g1), f2 ∈ O(g2).
  // = O(k)
}

vector<id_billetera> Billetera::detinatarios_mas_frecuentes(int k) const {
  vector<id_billetera> primerosKdestinatarios(k,0);                                         // O(k)
  for(int i = 0; i < _transferencias_por_destinatario.size() && i < k; ++i) {               // O(1). k iteraciones => O(k)
    primerosKdestinatarios[i] = _transferencias_por_destinatario[i].first;                  // O(1)
  }
  return primerosKdestinatarios;                                                            // O(1)

  // O(k) + O(k)
  // f1 + f2 ∈ O(max{g1,g2}), f1 ∈ O(g1), f2 ∈ O(g2).
  // = O(k)
}