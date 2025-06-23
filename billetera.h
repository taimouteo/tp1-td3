#ifndef BILLETERA_H
#define BILLETERA_H

#include <string>
#include <vector>
#include "lib.h"
#include "blockchain.h"

using namespace std;

/** Invariante de la clase billetera en lenguaje natural:
 *  - Hay una o más transferencias notificadas. 
 *  - La primera transacción es la transacción semilla. 
 *  - La longitud de los saldos al final de cada día es la cantidad de días que pasaron desde que se abrió la billetera hasta el día de la última. 
 *  - El día de apertura es el día de la primera transacción. 
 *  - El saldo es el saldo al final del día actual. 
 *  - Las últimas transacciones están ordenadas de manera creciente. 
 *  - El saldo al final de cada día es 100 más la suma/resta de los montos de las últimas transacciones desde el día de apertura hasta ese día. 
 *  - Las transferencias por destinatario están ordenadas decrecientemente. 
 *  - La suma de las transferencias por destinatario es igual a la cantidad de transacciones. 
 *  - Todos los ids a los que se les envió dinero están incluidos en transf por destinatarios. 
 */

class Billetera {
  public:
    /**
     * Constructor. No se utiliza directamente, si no que se asume que será
     * llamado por la blockchain al utilizar el método `abrir_billetera`.
     */
    Billetera(const id_billetera id, Blockchain* blockchain);

    /**
     *  Retorna el id de la billetera, asignado al momento de su creación.
     */
    id_billetera id() const;

    /**
     * Método utilizado para notificar a la billetera cuando se impacta en la
     * blockchain una transacción que la implica (ya sea como origen o destino).
     *
     * Este método también es notificado al registrarse la transacción semilla.
     *
     * Complejidad esperada: O(D*log(D) + C), donde:
     *   - D es la máxima cantidad de días que una billetera estuvo activa
     *   - C es la máxima cantidad de destinatarios totales a los que una billetera envió dinero
     */
    void notificar_transaccion(Transaccion t);

    /**
     * Devuelve el saldo actual de la billetera.
     *
     * Complejidad esperada: O(1)
     */
    monto saldo() const;

    /**
     * Devuelve el saldo que tenía la billetera hacia fin del día de `t`.
     *
     * Por ejemplo, si t es el 10 de enero a las 15hs, devolveremos el saldo que
     * tenía la billetera al fin del 10 de enero.
     *
     * Se asume como precondición que t es mayor o igual al momento de la
     * creación de la billetera.
     *
     * Complejidad esperada: O(log(D)), donde D es la máxima cantidad de días
     * que una billetera estuvo activa
     */
    monto saldo_al_fin_del_dia(timestamp t) const;

    /**
     * Devuelve las últimas `k` transaccionesen las que esta billetera participó
     * (ya sea como origen o destino). Incluye la transacción semilla.
     *
     * Complejidad esperada: O(k)
     */
    vector<Transaccion> ultimas_transacciones(int k) const;

    /**
     * Devuelve los ids de las `k` billeteras a las que más transacciones le
     * realizó esta billetera.
     *
     * Complejidad esperada: O(k)
     */
    vector<id_billetera> detinatarios_mas_frecuentes(int k) const;

  private:
    /** El id de la billetera */
    const id_billetera _id;

    /** Puntero a la blockchain asociada */
    Blockchain* const _blockchain;

    /** Saldo actualizado de la billetera */
    monto _saldo;

    /** Dia en que se abrio la billetera */
    int _dia_apertura;

    /** Lista las ultimas transacciones cronologicamente */
    vector<Transaccion> _ultimas_transacciones;
    
    /** Pre: Para toda k clave, EsPrincipioDeDia(k) */
    vector<monto> _saldo_al_fin_del_dia;

    /** Cuantas transferencias se le realizo a todos los destinatarios */
    vector<pair<id_billetera,int>> _transferencias_por_destinatario;
};

#endif