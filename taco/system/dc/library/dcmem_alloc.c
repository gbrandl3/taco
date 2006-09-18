/******************************************************************************
 * Toolkit for building distributed control systems or any other distributed system.
 *
 * Copyright (c) 1990-2005 by European Synchrotron Radiation Facility,
 *                            Grenoble, France
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File         : dcmem_alloc.c
 *
 * Project      : Data collector
 *
 * Description  : 
 *
 * Author(s):	Emmanuel Taurel
 *		$Author: jkrueger1 $
 *
 * Original     : August 1992
 *
 * Version      : $Revision: 1.3 $
 *
 * Date         : $Date: 2006-09-18 21:48:25 $
 *
 */

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif
#include <API.h>

#include <stdio.h>
#include <dcP.h>
#if HAVE_TIME_H
#	include <time.h>
#endif
#include <errno.h>

#if HAVE_SYS_TYPES_H
#	include <sys/types.h>
#endif
#if HAVE_SYS_IPC_H
#	include <sys/ipc.h>
#endif
#if HAVE_SYS_SEM_H
#	include <sys/sem.h>
#endif

/* External variable */

extern int time_out;
extern int errno;



/****************************************************************************
*                                                                           *
*		Code for dcmem_alloc function                               *
*                        -----------                                        *
*                                                                           *
*    Function rule : To allocate place in the shared memory area            *
*		     Every block of XX bytes of the shared memory area is   *
*		     represented by a bit. If this bit is one, this means   *
*		     the area is already occupied. If this bit is 0, the    *
*		     area is free for use. To allocate place means to find  *
*		     enough consecutive bits at zero, to change them to one *
*		     and to return to the caller the address of the         *
*		     allocated memory					    *
*		     WARNING : This function uses a semaphore and set an    *
*			       alarm to have a time-out on semaphore        *
*			       request. The signal must be caught and the   *
*			       the signal routine must set the "time_out"   *
*			       variable to True in case of its execution    *
*                                                                           *
*    Argin : - Base address of the data buffer				    *
*	     - Address of the allocation table				    *
*            - The buffer size to allocate (must be a multiple of	    *
*	       BLOS_SIZE)						    *
*            - The allocation area size (in bytes)			    *
*	     - The semaphore identifier					    *
*                                                                           *
*    Argout : - An error code 						    *
*                                                                           *
*    This function returns NULL if one error occurs and the error code will *
*    be set. Otherwise, it returns the address of the buffer allocated.	    *
*                                                                           *
****************************************************************************/


unsigned char *dcmem_alloc(unsigned char *base,register char *tab,int size,int alloc_size,int semid,long *perr)
{
	int i,j,l,m,k_init,bit_num,tmp,tmp1;
	register int full_byte,first_byte,last_byte;
	int init_0;
	register unsigned char mask,last_mask,first_mask,tmp_mask;
	unsigned char f_mask;
	char bad_block = False;
	int nb_bit;
	struct itimerval time,old_time;
	struct sembuf sops[1];
	int resu,sig_rec;

	nb_bit = size >> SHIFT;

	if (nb_bit == 0) {
		*perr = ALLOC_PARAMETER;
		return(NULL);
			}

/* Try to get the semaphore (install an alarm to have a time-out) */

	time_out = False;
	sig_rec = False;

	time.it_value.tv_sec = 0;
	time.it_value.tv_usec = 500000;
	timerclear(&time.it_interval);
	if (setitimer(ITIMER_REAL,&time,&old_time) == -1) {
		*perr = ALLOC_TIMER;
		return(NULL);
							}
	sops[0].sem_num = 0;
	sops[0].sem_op = -1;
	sops[0].sem_flg = 0;
	
	resu = semop(semid,sops,1);

/* Exit function if time-out. Otherwise, clear the alarm and set variable
   to release semaphore during the next call to semop */

	if ((resu == -1) && (errno == EINTR))
		sig_rec = True;
	if ((time_out == True) && (sig_rec == True)) {
		time_out = False;
		*perr = ALLOC_TIMEOUT;
		return(NULL);
				}
	else {
		sops[0].sem_op = 1;
		timerclear(&time.it_interval);
		timerclear(&time.it_value);
		if (setitimer(ITIMER_REAL,&time,&old_time) == -1) {
			semop(semid,sops,1);
			*perr = ALLOC_TIMER;
			return(NULL);
								}
		}

/* Beginning of real work */

	for (i = 0;i < alloc_size;i++) {

for_byte:
	if (i == alloc_size) {
		semop(semid,sops,1);
		*perr = ALLOC_NO_SPACE;
		return(NULL);
			}  

		if (tab[i] != (char)0xFF) {
			k_init = 0;
			init_0 = 1;
			mask = 0x80;

			for (j = k_init;j < 8;j++) {

				if ((tab[i] & mask) == 0) {

/* A block of only one bit ? */

					if (nb_bit == 1) {
						tab[i] = tab[i] | mask;
						bit_num = (i << 3) + j;
						semop(semid,sops,1);
						
						return((bit_num << SHIFT) + base);
							}

					init_0 = j + 1;

/* Compute the number of bits at zero in this byte, the number of bytes
   which should be at zero and the number of extra bit at zero */

					if (nb_bit < (10 - init_0)) {
						first_byte = nb_bit - 1;
						last_byte = 0;
						full_byte = 0;
								}
					else {
						first_byte = 8 - init_0;
						tmp = nb_bit - first_byte - 1;
						full_byte = tmp >> 3;
						last_byte = tmp - (full_byte << 3);
						}

/* Test bits in this byte */

					first_mask = 1 << (7 - init_0);
					for (l = 0;l < first_byte;l++) {
						if ((tab[i] & first_mask) != 0) {
							bad_block = True;
							break;
									}
						first_mask = first_mask >> 1;
									}

/* If this block is too small */

					if (bad_block == True) {
						bad_block = False;
						j = init_0 + l + 1;
						switch (j) {
							case 2 : tmp_mask = 0x3f;
								 break;

							case 3 : tmp_mask = 0x1f;
								 break;

							case 4 : tmp_mask = 0xf;
								 break;

							case 5 : tmp_mask = 0x7;
								 break;

							case 6 : tmp_mask = 0x3;
								 break;

							case 7 : tmp_mask = 0x1;								 break;

							case 8 : i++;
								 goto for_byte;
								 break;
							   }

						if ((tab[i] & tmp_mask) != tmp_mask) {
							mask = 1 << (7 - j);
							j--;
							continue;
									}
						else {
							i++;
							goto for_byte;
						     }
								}

/* Test the number of bytes which should be 0 */

					tmp = i + 1;
					for (l = 0;l < full_byte;l++) {
						tmp1 = tmp + l;
						if (tmp1 == alloc_size) {
							semop(semid,sops,1);
							*perr = ALLOC_NO_SPACE;
							return(NULL);
									}
						if (tab[tmp1] != 0) {
							bad_block = True;
							break;
									}
									}

/* If this block is too small */

					if (bad_block == True) {
						bad_block = False;
						i = i + l + 1;
						goto for_byte;		
								}

/* Test the number of remainig bytes which should be zero */

					last_mask = 0x80;
					tmp = i + full_byte + 1;
					for (l = 0;l < last_byte;l++) {
						if (tmp == alloc_size) {
							semop(semid,sops,1);
							*perr = ALLOC_NO_SPACE;
							return(NULL);
									}
						if ((tab[tmp] & last_mask) != 0) {
							bad_block = True;
							break;
									}
						last_mask = last_mask >> 1;
									}

/* If this block is too small */

					if (bad_block == True) {
						bad_block = False;
						i = tmp;
						j = l;
						mask = last_mask >> 1;
						continue;
								}

/* We have found a block big enought, so mark it as occupied (set bits to 1) */

					compute_mask(init_0,first_byte + 1,&f_mask);
					m = i;
					tab[m++] = tab[m] | f_mask;

					for(l = 0;l < full_byte;l++)
						tab[m++] = (char)0xFF;

					if (last_byte != 0) {
						switch(last_byte) {
							case 1 : 
							last_mask = (char)0x80;
							break;

							case 2 : 
							last_mask = (char)0xC0;
							break;

							case 3 : 
							last_mask = (char)0xE0;
							break;

							case 4 : 
							last_mask = (char)0xF0;
							break;

							case 5 : 
							last_mask = (char)0xF8;
							break;

							case 6 : 
							last_mask = (char)0xFC;
							break;

							case 7 : 
							last_mask = (char)0xFE;
							break;
								  }
						tab[m] = tab[m] | last_mask;
							    }

/* Return block address to the caller */

					bit_num = (i << 3) + j;
					semop(semid,sops,1);
					return((bit_num << SHIFT) + base);
				    }
					mask = mask >> 1;
				    }
		}
		}

	if (i == alloc_size) {
		semop(semid,sops,1);
		*perr = ALLOC_NO_SPACE;
		return(NULL);
			     }
}




/****************************************************************************
*                                                                           *
*		Code for compute_mask function                              *
*                        ------------                                       *
*                                                                           *
*    Function rule : To compute the mask needed (for an OR operation)       *
*		     to set the necessary bits in the first byte of the     *
*		     area to be allocated				    *
*                                                                           *
*    Argin : - The position of the first bit to set (between 1 and 8)       *
*	     - The number of bits to set (between 1 and 8)		    *
*                                                                           *
*    Argout : - The mask						    *
*                                                                           *
****************************************************************************/


compute_mask(int pos,int nb_bit,unsigned char *pmask)
{
	switch(pos) {
		case 1 : switch(nb_bit) {
				case 1 : *pmask = (char)0x80;
					 break;
				case 2 : *pmask = (char)0xC0;
					 break;
				case 3 : *pmask = (char)0xE0;
					 break;
				case 4 : *pmask = (char)0xF0;
					 break;
				case 5 : *pmask = (char)0xF8;
					 break;
				case 6 : *pmask = (char)0xFC;
				      	 break;
				case 7 : *pmask = (char)0xFE;
					 break;
				case 8 : *pmask = (char)0xFF;
					 break;
					}
			 break;
		case 2 : switch(nb_bit) {
				case 1 : *pmask = (char)0x40;
					 break;
				case 2 : *pmask = (char)0x60;
					 break;
				case 3 : *pmask = (char)0x70;
					 break;
				case 4 : *pmask = (char)0x78;
					 break;
				case 5 : *pmask = (char)0x7C;
					 break;
				case 6 : *pmask = (char)0x7E;
				      	 break;
				case 7 : *pmask = (char)0x7F;
					 break;
					}
			 break;
		case 3 : switch(nb_bit) {
				case 1 : *pmask = (char)0x20;
					 break;
				case 2 : *pmask = (char)0x30;
					 break;
				case 3 : *pmask = (char)0x38;
					 break;
				case 4 : *pmask = (char)0x3C;
					 break;
				case 5 : *pmask = (char)0x3E;
					 break;
				case 6 : *pmask = (char)0x3F;
				      	 break;
					}
			 break;
		case 4 : switch(nb_bit) {
				case 1 : *pmask = (char)0x10;
					 break;
				case 2 : *pmask = (char)0x18;
					 break;
				case 3 : *pmask = (char)0x1C;
					 break;
				case 4 : *pmask = (char)0x1E;
					 break;
				case 5 : *pmask = (char)0x1F;
					 break;
					}
			 break;
		case 5 : switch(nb_bit) {
				case 1 : *pmask = (char)0x8;
					 break;
				case 2 : *pmask = (char)0xC;
					 break;
				case 3 : *pmask = (char)0xE;
					 break;
				case 4 : *pmask = (char)0xF;
					 break;
					}
			 break;
		case 6 : switch(nb_bit) {
				case 1 : *pmask = (char)0x4;
					 break;
				case 2 : *pmask = (char)0x6;
					 break;
				case 3 : *pmask = (char)0x7;
					 break;
					}
			 break;
		case 7 : switch(nb_bit) {
				case 1 : *pmask = (char)0x2;
					 break;
				case 2 : *pmask = (char)0x3;
					 break;
					}
			 break;
		case 8 : *pmask = (char)0x1;
		 	 break;
				}
}
