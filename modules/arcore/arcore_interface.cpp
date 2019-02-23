/*************************************************************************/
/*  arcore_interface.cpp                                                 */
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

#include "arcore_interface.h"

#include "platform/android/thread_jandroid.h"
#include "servers/visual/visual_server_globals.h"

// TODO remove these temporary includes once we add our GL_TEXTURE_EXTERNAL_OES support to our drivers
#include "platform_config.h"
#ifndef GLES3_INCLUDE_H
#include <GLES3/gl3.h>
#else
#include GLES3_INCLUDE_H
#endif

// only defined in gl2ext ? does this work in gles3?
#define GL_TEXTURE_EXTERNAL_OES 0x8D65

/**
	@author Bastiaan Olij <mux213@gmail.com>, Robert Hofstra <robert.hofstra@knowlogy.nl>
	ARCore interface between Android and Godot
**/

StringName ARCoreInterface::get_name() const {
	return "ARCore";
}

int ARCoreInterface::get_capabilities() const {
	return ARVRInterface::ARVR_MONO + ARVRInterface::ARVR_AR;
}

void ARCoreInterface::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_resume"), &ARCoreInterface::_resume);
}

bool ARCoreInterface::is_initialized() const {
	// if we're in the process of initialising we treat this as initialised...
	return (init_status != NOT_INITIALISED) && (init_status != INITIALISE_FAILED);
}

void ARCoreInterface::_resume() {
	if (is_initialized() && ar_session == NULL) {
		// Most of this code is based on the hello ar sample application from Google. Most remarks are
		// theirs and we'll need to do something with the suggestions made within.
		ArInstallStatus install_status;

		// Check our camera privilege
		// TODO: Note that if we asked for permission already we should just
		// check if we have permission and fail if we don't, user said no...
		if (!OS::get_singleton()->request_permission("CAMERA")) {
			init_status = REQUESTED_CAMERA;
			return;
		}

		// get some android things, ugly but we'll clean it up later...
		JNIEnv *env = ThreadAndroid::get_env();

		jclass activityThread = env->FindClass("android/app/ActivityThread");
		jmethodID currentActivityThread = env->GetStaticMethodID(activityThread, "currentActivityThread", "()Landroid/app/ActivityThread;");
		jobject activity = env->CallStaticObjectMethod(activityThread, currentActivityThread);

		jmethodID getApplication = env->GetMethodID(activityThread, "getApplication", "()Landroid/app/Application;");
		jobject context = env->CallObjectMethod(activity, getApplication);

		// If install was not yet requested, that means that we are resuming the
		// activity first time because of explicit user interaction (such as
		// launching the application)
		bool user_requested_install = init_status == WAITING_ON_INSTALL ? 0 : 1;

		// === ATTENTION!  ATTENTION!  ATTENTION! ===
		// This method can and will fail in user-facing situations.  Your
		// application must handle these cases at least somewhat gracefully.  See
		// HelloAR Java sample code for reasonable behavior.
		ArStatus status = ArCoreApk_requestInstall(env, activity, user_requested_install, &install_status);
		if (status != AR_SUCCESS) {
			// I guess we couldn't...
			print_line("ARCore: Couldn't find ARCore " + itos(status));
			return;
		}

		switch (install_status) {
			case AR_INSTALL_STATUS_INSTALLED:
				break;
			case AR_INSTALL_STATUS_INSTALL_REQUESTED:
				// ARCore is not installed, we requested the user to install it
				print_line("ARCore: ARCore installation requested from user.");

				init_status = WAITING_ON_INSTALL;

				return;
			default:
				// uh? do we have other options? should we return false here?
				break;
		}

		// === ATTENTION!  ATTENTION!  ATTENTION! ===
		// This method can and will fail in user-facing situations.  Your
		// application must handle these cases at least somewhat gracefully.  See
		// HelloAR Java sample code for reasonable behavior.
		if (ArSession_create(env, context, &ar_session) != AR_SUCCESS) {
			print_line("ARCore: ARCore couldn't be created.");
			init_status = INITIALISE_FAILED; // don't try again.
			return;
		} else if (ar_session == NULL) {
			print_line("ARCore: ARCore couldn't be created.");
			init_status = INITIALISE_FAILED; // don't try again.
			return;
		}

		ArFrame_create(ar_session, &ar_frame);
		if (ar_frame == NULL) {
			print_line("ARCore: Frame couldn't be created.");

			ArSession_destroy(ar_session);
			ar_session = NULL;

			init_status = INITIALISE_FAILED; // don't try again.
			return;
		}

		// TODO: may need to update display rotation here and flip x/y
		Size2 size = OS::get_singleton()->get_window_size();
		width = size.x;
		height = size.y;

		ArSession_setDisplayGeometry(ar_session, display_rotation, width, height);

		print_line("ARCore: Resumed.");
		init_status = INITIALISED;
	}

	if (init_status == INITIALISED && ar_session != NULL) {
		ArStatus status = ArSession_resume(ar_session);
		if (status != AR_SUCCESS) {
			// uh, what do we do in this situation?
		}
	}

	return;
}

void ARCoreInterface::_notification(int p_what) {
	switch (p_what) {
		case MainLoop::NOTIFICATION_WM_FOCUS_IN: {
			_resume();
		}; break;
		default:
			break;
	}
}

bool ARCoreInterface::initialize() {
	if (init_status == INITIALISE_FAILED) {
		// if we fully failed last time, don't try again..
		return false;
	} else if (init_status == NOT_INITIALISED) {
		print_line("ARCore: Initialising...");
		init_status = START_INITIALISE;

		// call resume for the first time ;)
		_resume();

		// If we needed to:
		// - ask permission to use the camera
		// - prompt to install ARCPRE
		// we'll finish initialising on resume which is called through _notification
	}

	return is_initialized();
}

void ARCoreInterface::uninitialize() {
	if (is_initialized()) {
		if (ar_session != NULL) {
			ArSession_destroy(ar_session);
			ArFrame_destroy(ar_frame);

			ar_session = NULL;
			ar_frame = NULL;
		}

		init_status = NOT_INITIALISED;
	}
}

Size2 ARCoreInterface::get_render_targetsize() {
	_THREAD_SAFE_METHOD_

	Size2 target_size = OS::get_singleton()->get_window_size();

	return target_size;
}

bool ARCoreInterface::is_stereo() {
	return false;
}

Transform ARCoreInterface::get_transform_for_eye(ARVRInterface::Eyes p_eye, const Transform &p_cam_transform) {
	_THREAD_SAFE_METHOD_

	Transform transform_for_eye;

	ARVRServer *arvr_server = ARVRServer::get_singleton();
	ERR_FAIL_NULL_V(arvr_server, transform_for_eye);

	if (init_status == INITIALISED) {
		float world_scale = arvr_server->get_world_scale();

		// just scale our origin point of our transform, note that we really shouldn't be using world_scale in ARKit but....
		transform_for_eye = transform;
		transform_for_eye.origin *= world_scale;

		transform_for_eye = p_cam_transform * (arvr_server->get_reference_frame()) * transform_for_eye;
	} else {
		// huh? well just return what we got....
		transform_for_eye = p_cam_transform;
	}

	return transform_for_eye;
}

CameraMatrix ARCoreInterface::get_projection_for_eye(ARVRInterface::Eyes p_eye, real_t p_aspect, real_t p_z_near, real_t p_z_far) {
	// Remember our near and far, we'll use it next frame
	z_near = p_z_near;
	z_far = p_z_far;

	return projection;
}

void ARCoreInterface::commit_for_eye(ARVRInterface::Eyes p_eye, RID p_render_target, const Rect2 &p_screen_rect) {
	_THREAD_SAFE_METHOD_

	// We must have a valid render target
	ERR_FAIL_COND(!p_render_target.is_valid());

	// Because we are rendering to our device we must use our main viewport!
	ERR_FAIL_COND(p_screen_rect == Rect2());

	// get the size of our screen
	Rect2 screen_rect = p_screen_rect;

	VSG::rasterizer->set_current_render_target(RID());
	VSG::rasterizer->blit_render_target_to_screen(p_render_target, screen_rect, 0);
}

void ARCoreInterface::process() {
	_THREAD_SAFE_METHOD_

	if (init_status != INITIALISED) {
		// not yet initialised so....
		return;
	} else if (ar_session == NULL) {
		// don't have a session yet so...
		return;
	}

	print_line("ARCore: process..");

	// Generate our texture if we haven't got one already
	if (camera_texture_id == 0) {
		// TODO: add GL_TEXTURE_EXTERNAL_OES support to GLES2/GLES3 drivers and use Godots internal texture objects
		glGenTextures(1, &camera_texture_id);
		glBindTexture(GL_TEXTURE_EXTERNAL_OES, camera_texture_id);
		glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	// Have ARCore grab a camera frame, load it into our texture object and do its funky SLAM logic
	ArSession_setCameraTextureName(ar_session, camera_texture_id);

	// TODO: also make our texture image available in a camera feed

	// Update session to get current frame and render camera background.
	if (ArSession_update(ar_session, ar_frame) != AR_SUCCESS) {
		print_line("ARCore: OnDrawFrame ArSession_update error");
	}

	ArCamera *ar_camera;
	ArFrame_acquireCamera(ar_session, ar_frame, &ar_camera);

	// process our view matrix
	float view_mat[4][4];
	ArCamera_getViewMatrix(ar_session, ar_camera, (float *)view_mat);

	// TODO: We may need to adjust this based on orientation
	transform.basis.elements[0].x = view_mat[0][0];
	transform.basis.elements[1].x = view_mat[0][1];
	transform.basis.elements[2].x = view_mat[0][2];
	transform.basis.elements[0].y = view_mat[1][0];
	transform.basis.elements[1].y = view_mat[1][1];
	transform.basis.elements[2].y = view_mat[1][2];
	transform.basis.elements[0].z = view_mat[2][0];
	transform.basis.elements[1].z = view_mat[2][1];
	transform.basis.elements[2].z = view_mat[2][2];
	transform.origin.x = view_mat[3][0];
	transform.origin.y = view_mat[3][1];
	transform.origin.z = view_mat[3][2];

	// process our projection matrix
	float projection_mat[4][4];
	ArCamera_getProjectionMatrix(ar_session, ar_camera, z_near, z_far, (float *)projection_mat);

	projection.matrix[0][0] = projection_mat[0][0];
	projection.matrix[1][0] = projection_mat[1][0];
	projection.matrix[2][0] = projection_mat[2][0];
	projection.matrix[3][0] = projection_mat[3][0];
	projection.matrix[0][1] = projection_mat[0][1];
	projection.matrix[1][1] = projection_mat[1][1];
	projection.matrix[2][1] = projection_mat[2][1];
	projection.matrix[3][1] = projection_mat[3][1];
	projection.matrix[0][2] = projection_mat[0][2];
	projection.matrix[1][2] = projection_mat[1][2];
	projection.matrix[2][2] = projection_mat[2][2];
	projection.matrix[3][2] = projection_mat[3][2];
	projection.matrix[0][3] = projection_mat[0][3];
	projection.matrix[1][3] = projection_mat[1][3];
	projection.matrix[2][3] = projection_mat[2][3];
	projection.matrix[3][3] = projection_mat[3][3];

	ArTrackingState camera_tracking_state;
	ArCamera_getTrackingState(ar_session, ar_camera, &camera_tracking_state);

	// TODO: Convert ARCore tracking state to our tracking state so we can inform the user

	ArCamera_release(ar_camera);

	// Now need to handle our anchors and such....
}

ARCoreInterface::ARCoreInterface() {
	ar_session = NULL;
	ar_frame = NULL;
	init_status = NOT_INITIALISED;
	width = 1;
	height = 1;
	display_rotation = 0;
	camera_texture_id = 0;
	z_near = 0.01;
	z_far = 1000.0;
	projection.set_perspective(60.0, 1.0, z_near, z_far, false);
}

ARCoreInterface::~ARCoreInterface() {
	// remove_all_anchors();

	// and make sure we cleanup if we haven't already
	if (is_initialized()) {
		uninitialize();
	}
}
