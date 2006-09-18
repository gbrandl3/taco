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
 * File         : dcmem_info.c 
 *
 * Project      : Data collector
 *
 * Description  :
 *
 * Author:	Emmanuel Taurel
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


/****************************************************************************
*                                                                           *
*		Code for dcmem_info function                                *
*                        ----------                                         *
*                                                                           *
*    Function rule : To return the free memory in a block and to return     *
*		     largest free area.					    *
*		     Every block of XX bytes of the shared memory area is   *
*		     represented by a bit. If this bit is one, this means   *
*		     the area is already occupied. If this bit is 0, the    *
*		     area is free for use. 				    *
*                                                                           *
*    Argin : - Address of the allocation table				    *
*            - The buffer size						    *
*                                                                           *
*    Argout : - The largest free area size				    *
*	      - The amount of free memory				    *
*	      - The number of free area					    *
*	      - The error code						    *
*                                                                           *
*    This function returns DS_NOTOK if one error occurs and the error code will   *
*    be set								    *
*                                                                           *
****************************************************************************/


int dcmem_info(register char *tab,int buf_size,unsigned int *lmem_free,unsigned int *mem_free,int *parea,long *perr)
{
	int i,j;
	register unsigned char mask;
	register unsigned int free,bl_free;
	int max_bl_free;
	int nb_area = 0;

/* Test function parameter (to avoid core dump !) */

	if (perr == NULL || lmem_free == NULL || mem_free == NULL) {
		*perr = ALLOC_PARAMETER;
		return(DS_NOTOK);
								}

/* Init. variables */

	max_bl_free = free = bl_free = 0;

/* Beginning of real work */

	for (i = 0;i < buf_size;i++) {
		if (tab[i] != (char)0xFF) {

/* Special case where the byte equal 0. 8 consecutive bits at 0 */

			if (tab[i] == (char)0x0) {
				free = free + 8;
				bl_free = bl_free + 8;
				if (i == (buf_size - 1)) {
					nb_area++;
					if (bl_free > max_bl_free)
						max_bl_free = bl_free;
							}
				continue;
					   }

			mask = 0x80;
			for (j = 0;j < 8;j++) {
				if ((tab[i] & mask) == 0) {

/* One bit at zero has been found .Test to see if all the following bits
   in this byte are also at 0. In this case directly test the following byte. */

					switch(j) {
						case 1 : if ((tab[i] & (char)0x3f) == 0x0) {
								free = free + 7;
								bl_free = bl_free + 7;
								j = 8;
								continue;
									}
							 break;

						case 2 : if ((tab[i] & (char)0x1f) == 0x0) {
								free = free + 6;
								bl_free = bl_free + 6;
								j = 8;
								continue;
									}
							 break;

						case 3 : if ((tab[i] & (char)0xf) == 0x0) {
								free = free + 5;
								bl_free = bl_free + 5;
								j = 8;
								continue;
									}
							 break;

						case 4 : if ((tab[i] & (char)0x7) == 0x0) {
								free = free + 4;
								bl_free = bl_free + 4;
								j = 8;
								continue;
									}
							 break;

						case 5 : if ((tab[i] & (char)0x3) == 0x0) {
								free = free + 3;
								bl_free = bl_free + 3;
								j = 8;
								continue;
									}
							 break;

						case 6 : if ((tab[i] & (char)0x1) == 0x0) {
								free = free + 2;
								bl_free = bl_free + 2;
								j = 8;
								continue;
									}
							 break;

						case 7 : free++;
							 bl_free++;
							 j = 8;
							 continue;
							 break;
							}

/* All the remainnig bit in the bytes are not equal to zero. So, just increment
   the block size and the amount of free memory */

					free++;
					bl_free++;
						}

				else {

/* A bit at one has been found. This is the end of the free area. Test to
   verify that this area is not the largest one. */

					if (bl_free > max_bl_free) 
						max_bl_free = bl_free;
					if (bl_free != 0)
						nb_area++;
					bl_free = 0;
				     }

/* Shift mask to test the following bit */

				mask = mask >> 1;
						}
				}

/* The byte is equal to 0xFF (occupied area) but may be it is the end of a
   free area which finish at a byte boundary */

			else {
				if (bl_free != 0) {
					if (bl_free > max_bl_free)
						max_bl_free = bl_free;
					if (bl_free != 0)
						nb_area++;
					bl_free = 0;
						}
				}
			}

/* Return the largest free block and the amount of free memory */

	if (max_bl_free == 0) {
		*lmem_free = (bl_free << SHIFT);  /* All the memory is free */
		*parea = 1;
				}
	else {
		*lmem_free = (max_bl_free << SHIFT);
		*parea = nb_area;
	     }
	*mem_free = (free << SHIFT);
	return(DS_OK);

}
