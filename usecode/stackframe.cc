
/*
 *  stackframe.cc - a usecode interpreter stack frame
 *
 *  Copyright (C) 2002  The Exult Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <iostream>
#include <iomanip>

#include "useval.h"
#include "stackframe.h"
#include "ucinternal.h"
#include "utils.h"
#include "ucfunction.h"


int Stack_frame::LastCallChainID = 0;

Stack_frame::Stack_frame(Usecode_function *fun,
                         int event,
                         Game_object *caller,
                         int chain, int depth)
	: function(fun), ip(nullptr), data(nullptr), externs(nullptr), code(nullptr), endp(nullptr),
	  line_number(-1), call_chain(chain), call_depth(depth),
	  num_externs(0), num_args(0), num_vars(0), locals(nullptr), eventid(event),
	  caller_item(shared_from_obj(caller)), save_sp(nullptr) /* seting the vars */ {
	ip = function->code;
	endp = ip + function->len;
	//DMA(pointer based) Object constsruction? 
	int data_len;
	if (!fun->extended)
		data_len = Read2(ip);   // Get length of (text/Usecodes) data.
	else
		data_len = Read4s(ip); // 32 bit lengths

	data = ip; //set the data to the internal pointer 

	ip += data_len;         // Point past text.
	num_args = Read2(ip);   // # of args. this function takes.

	// Local variables follow args.
	num_vars = Read2(ip);

	// Allocate locals.
	int num_locals = num_vars + num_args;
	locals = new Usecode_value[num_locals];

	num_externs = Read2(ip); // external function references
	externs = ip;

	ip += 2 * num_externs; // now points to actual code

	code = ip;
}//end of constructor 

Stack_frame::~Stack_frame() {
	delete[] locals;
}

std::ostream &operator<<(std::ostream &out, Stack_frame &frame) {
	// #depth: 0xIP in 0xfunid ( obj=..., event=..., arguments )

	// TODO: include any debugging info
	// #depth: 0xIP in functionname (obj=...,event=..., arg1name=..., ...)

	out << "#" << frame.call_depth << ": 0x"
	    << std::hex << std::setw(4) << std::setfill('0')
	    << static_cast<int>(frame.ip - frame.code) << " in 0x"
	    << std::setw(4) << frame.function->id
	    << "(obj=" << std::setw(8) << frame.caller_item.get()
	    << ",ev=" << frame.eventid
	    << std::setfill(' ') << std::dec;

	for (int i = 0; i < frame.num_args; i++)
		out << ", " << frame.locals[i];

	out << ")";

	return out;
}
FILE& WriteSelfToOutputFile(FILE* out) {
	// #depth: 0xIP in 0xfunid ( obj=..., event=..., arguments )

	// TODO: include any debugging info
	// #depth: 0xIP in functionname (obj=...,event=..., arg1name=..., ...)
	fprintf(out, "#%d hex 0x%X (obj=%s, ev = %d) ",
			this.call_depth, static_cast<int>(this.ip - frame.code),
			this.caller_item.get()->get_name().c_str(), 	
			this.function->id,
			this.eventid);

	for (int i = 0; i < frame.num_args; i++)
	out << ", " << frame.locals[i];

	//out << ")";

	return *out;
}
