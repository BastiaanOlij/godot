/*************************************************************************/
/*  openvr.h                                                             */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2016 Juan Linietsky, Ariel Manzur.                 */
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

#ifndef OPENVR_H
#define OPENVR_H

#include "object.h"
#include "os/main_loop.h"
#include "os/thread_safe.h"
#include "main/input_default.h"
#include "servers/arvr/arvr_interface.h"
#include "servers/arvr/arvr_positional_tracker.h"
#include "core/math/frustum.h"

#include <openvr.h>

class OpenVR : public ArVrInterface {

	GDCLASS(OpenVR, ArVrInterface);

private:
	vr::IVRSystem * hmd;
	vr::IVRRenderModels * render_models;
	vr::TrackedDevicePose_t tracked_device_pose[vr::k_unMaxTrackedDeviceCount];
	vr::VRControllerState_t tracked_device_state[vr::k_unMaxTrackedDeviceCount];

	ArVrPositionalTracker * trackers[vr::k_unMaxTrackedDeviceCount];

	void attach_device(uint32_t p_device_index);
	void detach_device(uint32_t p_device_index);
	const char * get_tracked_device_name(vr::TrackedDeviceIndex_t p_tracked_device_index, int pMaxLen) const;

protected:
	InputDefault * input;
	ArVrServer * arvr_server;

public:
	virtual bool is_installed();
	virtual bool hmd_is_present();

	virtual bool is_initialized();
	virtual bool initialize();
	virtual void uninitialize();

	virtual Point2 get_recommended_render_targetsize();
	virtual Transform get_transform_for_eye(ArVrInterface::Eyes p_eye, const Transform& p_head_position);
	virtual Rect2 get_frustum_for_eye(ArVrInterface::Eyes p_eye);

	virtual bool get_need_commit_eyes();
	virtual void commit_eye_texture(ArVrInterface::Eyes p_eye, Node* p_viewport);

	virtual void process();

	OpenVR();
	~OpenVR();
};

#endif // OPENVR_H
