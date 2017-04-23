/*************************************************************************/
/*  arvr_interface.cpp                                                   */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2017 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2017 Godot Engine contributors (cf. AUTHORS.md)    */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/
#include "arvr_interface.h"

void ArVrInterface::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_name"), &ArVrInterface::get_name);

	ClassDB::bind_method(D_METHOD("is_installed"), &ArVrInterface::is_installed);
	ClassDB::bind_method(D_METHOD("hmd_is_present"), &ArVrInterface::hmd_is_present);
	ClassDB::bind_method(D_METHOD("is_initialized"), &ArVrInterface::is_initialized);
	ClassDB::bind_method(D_METHOD("initialize"), &ArVrInterface::initialize);
	ClassDB::bind_method(D_METHOD("uninitialize"), &ArVrInterface::uninitialize);

	ClassDB::bind_method(D_METHOD("get_recommended_render_targetsize"), &ArVrInterface::get_recommended_render_targetsize);
	ClassDB::bind_method(D_METHOD("get_transform_for_eye", "eye", "head_position"), &ArVrInterface::get_transform_for_eye);
	ClassDB::bind_method(D_METHOD("get_frustum_for_eye", "eye"), &ArVrInterface::get_frustum_for_eye);

	ClassDB::bind_method(D_METHOD("get_need_commit_eyes"), &ArVrInterface::get_need_commit_eyes);
	ClassDB::bind_method(D_METHOD("commit_eye_texture", "eye", "node:viewport"), &ArVrInterface::commit_eye_texture);

	///@TODO once this is automatic, we can remove this
	ClassDB::bind_method(D_METHOD("process"), &ArVrInterface::process);

	BIND_CONSTANT(EYE_LEFT);
	BIND_CONSTANT(EYE_RIGHT);
};

void ArVrInterface::set_name(const String p_name) {
	name = p_name;
};

StringName ArVrInterface::get_name() const {
	return name;
};

ArVrInterface::ArVrInterface() {
	name = "Default";
};

ArVrInterface::~ArVrInterface() {

};
