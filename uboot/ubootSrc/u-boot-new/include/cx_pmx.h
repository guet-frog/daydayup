/* 
 * Copyright (c) 2014-2018, Shanghai SemiClouder Ltd. All rights reserved.
 *
 * Author:
 *	Warnus   <huxianan@cloudersemi.com>
 *       
 *  Revision History: 
 *  ----------------- 
 *  1.1  XXX 12/29/2014 XXX      Initialized by warnus
 *  
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details
 *
 */
#ifndef __CX_PMX_H
#define __CX_PMX_H

struct cx_pmx_platdata {
	char *pinctrl;
	unsigned start;
	unsigned physize;
};

extern int clouder_pmx_init(struct cx_pmx_platdata *pdata);

#endif
