/*************************************************************************/
/*  arvr_interface.h                                                     */
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
#ifndef ARVR_INTERFACE_H
#define ARVR_INTERFACE_H

#include "servers/arvr_server.h"
#include "scene/main/viewport.h"

class ArVrInterface : public Object {
	GDCLASS(ArVrInterface, Object);

private:
	StringName name;  // (unique) name of the tracker

protected:
	static void _bind_methods();


public:
	enum Eyes {
		EYE_LEFT,
		EYE_RIGHT
	};

	void set_name(const String p_name);
	StringName get_name() const;

	virtual bool is_installed() = 0;
	virtual bool hmd_is_present() = 0;

	virtual bool is_initialized() = 0;
	virtual bool initialize() = 0;
	virtual void uninitialize() = 0;

	virtual Point2 get_recommended_render_targetsize() = 0;
	virtual Transform get_transform_for_eye(ArVrInterface::Eyes p_eye, const Transform& p_head_position) = 0;
///@TODO need to add support to Variant so we can return Frustum here instead of Rect2
	virtual Rect2 get_frustum_for_eye(ArVrInterface::Eyes p_eye) = 0;

	virtual bool get_need_commit_eyes() = 0;
	virtual void commit_eye_texture(ArVrInterface::Eyes p_eye, Node* p_viewport) = 0;

	virtual void process() = 0;

	ArVrInterface();
	virtual ~ArVrInterface();
};

VARIANT_ENUM_CAST(ArVrInterface::Eyes);

#endif