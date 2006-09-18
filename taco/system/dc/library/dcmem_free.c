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
 * File         : dcmem_free.c
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
*		Code for dcmem_free function                                *
*                        ----------                                         *
*                                                                           *
*    Function rule : To free memory . This means to change bit in the       *
*		     allocation area from 1 to 0.			    *
*                    WARNING : This function uses a semaphore and set an    *
*                              alarm to have a time-out on semaphore        *
*                              request. The signal must be caught and the   *
*                              the signal routine must set the "time_out"   *
*                              variable to True in case of its execution    *
*                                                                           *
*    Argin : - Base address of the allocation area			    *
*	     - Base address of the buffer where an area must be cleared     *
*	     - Address of the buffer to freed				    *
*            - The buffer size						    *
*	     - The semaphore identifier					    *
*                                                                           *
*    Argout : - The error code						    *
*                                                                           *
*    This function returns DS_NOTOK if one error occurs and the error code will   *
*    be set								    *
*                                                                           *
****************************************************************************/


int dcmem_free(unsigned char *base,unsigned char *base_buf,unsigned char *tab,int buf_size,int semid,long *perr)
{
	int offset;
	int bit,byte,nb_bit;
	int tmp_bit,tmp_bit1;
	int size;
	int i;
	unsigned char fmask,lmask;
	int first_byte,last_byte,full_byte;
        struct itimerval time,old_time;
        struct sembuf sops[1];
	int resu,sig_rec;

/* Test function parameter (to avoid core dump !) */

	if (perr == NULL || buf_size == 0)
	{
		*perr = ALLOC_PARAMETER;
		return(DS_NOTOK);
	}

/* Try to get the semaphore (install an alarm to have a time-out) */

	time_out = False;
	sig_rec = False;

        time.it_value.tv_sec = 0;
        time.it_value.tv_usec = 500000;
        timerclear(&time.it_interval);
        if (setitimer(ITIMER_REAL,&time,&old_time) == -1)
	{
                *perr = ALLOC_TIMER;
                return(DS_NOTOK);
        }
        sops[0].sem_num = 0;
        sops[0].sem_op = -1;
        sops[0].sem_flg = 0;

        resu = semop(semid,sops,1);

/* Exit function if time-out. Otherwise, clear the alarm and set variable
   to release semaphore during the next call to semop */

	if ((resu == -1) && (errno == EINTR))
		sig_rec = True;
        if ((time_out == True) && (sig_rec == True))
	{
                time_out = False;
                *perr = ALLOC_TIMEOUT;
                return(DS_NOTOK);
	}
        else
	{
                sops[0].sem_op = 1;
                timerclear(&time.it_interval);
                timerclear(&time.it_value);
                if (setitimer(ITIMER_REAL,&time,&old_time) == -1)
		{
                        semop(semid,sops,1);
                        *perr = ALLOC_TIMER;
                        return(DS_NOTOK);
		}
	}


/* Compute bit position from buffer address
   (bit = first bit to clear,
    byte = number of the first byte where a bit must be cleared,
    nb_bit = number of the first bit to clear in the previous byte) */

	offset = (int)(tab - base_buf);
	bit = offset >> SHIFT;
	byte = bit >> 3;
	if ((bit & 0x7) == 0)
		nb_bit = 0;
	else
		nb_bit = bit - (byte << 3);
	
	size = buf_size >> SHIFT;
	
/* Compute the number of bits to clear in the first byte, the number of full
   byte to clear and the number of bits to clear in the last byte */

	tmp_bit = 8 - nb_bit;
	if (size <= tmp_bit)
	{
		first_byte = size;
		full_byte = 0;
		last_byte = 0;
	}
	else
	{
		first_byte = tmp_bit;
		tmp_bit1 = size - first_byte;
		full_byte = tmp_bit1 >> 3;
		last_byte = tmp_bit1 - (full_byte << 3);
	}

/* Clear bits in the first byte */

	compute_first_mask(nb_bit,first_byte,&fmask);
	base[byte] = base[byte] & fmask;
	byte++;

/* Clear full bytes */

	for (i = 0;i < full_byte;i++)
		base[byte++] = 0x0;

/* Clear bits in the last byte */

	if (last_byte != 0)
	{
		compute_last_mask(last_byte,&lmask);
		base[byte] = base[byte] & lmask;
	}

/* Leave function */

        semop(semid,sops,1);
	return(0);

}



/****************************************************************************
*                                                                           *
*		Code for compute_first_mask function                        *
*                        ------------------                                 *
*                                                                           *
*    Function rule : To compute the mask needed (for an AND operation)      *
*		     to clear the necessary bits in the first byte of the   *
*		     area to be freed					    *
*                                                                           *
*    Argin : - The position of the first bit to clear (between 0 and 7)     *
*	     - The number of bits to clear (between 1 and 8)		    *
*                                                                           *
*    Argout : - The mask						    *
*                                                                           *
****************************************************************************/


compute_first_mask(int pos,int nb_bit,unsigned char *pmask)
{
	switch(pos)
	{
		case 0 : 
		switch(nb_bit)
		{
				case 1 : *pmask = (char)0x7F;
					 break;
				case 2 : *pmask = (char)0x3F;
					 break;
				case 3 : *pmask = (char)0x1F;
					 break;
				case 4 : *pmask = (char)0xF;
					 break;
				case 5 : *pmask = (char)0x7;
					 break;
				case 6 : *pmask = (char)0x3;
				      	 break;
				case 7 : *pmask = (char)0x1;
					 break;
				case 8 : *pmask = (char)0x0;
					 break;
		}
		break;
		
		case 1 :
		switch(nb_bit)
		{
				case 1 : *pmask = (char)0xBF;
					 break;
				case 2 : *pmask = (char)0x9F;
					 break;
				case 3 : *pmask = (char)0x8F;
					 break;
				case 4 : *pmask = (char)0x87;
					 break;
				case 5 : *pmask = (char)0x83;
					 break;
				case 6 : *pmask = (char)0x81;
				      	 break;
				case 7 : *pmask = (char)0x80;
					 break;
		}
		break;
		
		case 2 : 
		switch(nb_bit)
		{
				case 1 : *pmask = (char)0xDF;
					 break;
				case 2 : *pmask = (char)0xCF;
					 break;
				case 3 : *pmask = (char)0xC7;
					 break;
				case 4 : *pmask = (char)0xC3;
					 break;
				case 5 : *pmask = (char)0xC1;
					 break;
				case 6 : *pmask = (char)0xC0;
				      	 break;
		}
		break;
		
		case 3 : 
		switch(nb_bit)
		{
				case 1 : *pmask = (char)0xEF;
					 break;
				case 2 : *pmask = (char)0xE7;
					 break;
				case 3 : *pmask = (char)0xE3;
					 break;
				case 4 : *pmask = (char)0xE1;
					 break;
				case 5 : *pmask = (char)0xE0;
					 break;
		}
		break;
		
		case 4 : 
		switch(nb_bit)
		{
				case 1 : *pmask = (char)0xF7;
					 break;
				case 2 : *pmask = (char)0xF3;
					 break;
				case 3 : *pmask = (char)0xF1;
					 break;
				case 4 : *pmask = (char)0xF0;
					 break;
		}
		break;
		
		case 5 : 
		switch(nb_bit)
		{
				case 1 : *pmask = (char)0xFB;
					 break;
				case 2 : *pmask = (char)0xF9;
					 break;
				case 3 : *pmask = (char)0xF8;
					 break;
		}
		break;
		
		case 6 : 
		switch(nb_bit)
		{
				case 1 : *pmask = (char)0xFD;
					 break;
				case 2 : *pmask = (char)0xFC;
					 break;
		}
		break;
		
		case 7 : *pmask = (char)0xFE;
		 	 break;
	}
}



/****************************************************************************
*                                                                           *
*		Code for compute_last_mask function                         *
*                        -----------------                                  *
*                                                                           *
*    Function rule : To compute the mask needed (for an AND operation)      *
*		     to clear the necessary bits in the last byte of the    *
*		     area to be freed					    *
*                                                                           *
*    Argin : - The number of bits to clear (between 1 and 8)		    *
*	     (It always starts from bit 0)				    *
*                                                                           *
*    Argout : - The mask						    *
*                                                                           *
****************************************************************************/

compute_last_mask(int nb_bit,unsigned char *pmask)
{
	switch(nb_bit) 
	{
		case 1 : *pmask = (char)0x7F;
			 break;
		case 2 : *pmask = (char)0x3F;
			 break;
		case 3 : *pmask = (char)0x1F;
			 break;
		case 4 : *pmask = (char)0x0F;
			 break;
		case 5 : *pmask = (char)0x07;
			 break;
		case 6 : *pmask = (char)0x03;
			 break;
		case 7 : *pmask = (char)0x01;
			 break;
	}
}
