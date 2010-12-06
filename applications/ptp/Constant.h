/*
Copyright(C) 2007 Giada Giorgi

This file is part of X-Simulator.

    X-Simulator is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    X-Simulator is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* Dichiarazione costanti usate dal simulatore. */
/* SYNC_BYTE = DREQ_BYTE = 164, DRES_BYTE = 98  */
#define MAX_SLAVE 100
#define SYNC_BYTE 164
#define DRES_BYTE 164 
#define DREQ_BYTE 164 
#define FUP_BYTE  164
#define OFFSET	  0
#define DRIFT	  1
#define CONSTDELAY 17E-3
#define UDP_EVENT_PORT 319
#define UDP_GENERAL_PORT 320

/*Tipi di nodo PTP*/
enum{SLAVE=0, MASTER=1};

/* Tipi di messaggio scambiati tra master e slave. */
enum{SYNC=0, DREQ=1, DRES=2, FUP=3};


/* Tipi di messaggio scambiati tra nodo e clock. */
enum{OFF=-1, TIME_REQ=0, TIME_RES=1, OFFSET_ADJ=2, FREQ_ADJ=3};


