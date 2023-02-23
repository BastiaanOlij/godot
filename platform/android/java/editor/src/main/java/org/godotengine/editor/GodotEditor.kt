/**************************************************************************/
/*  GodotEditor.kt                                                        */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

package org.godotengine.editor

import android.Manifest
import android.app.ActivityManager
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.os.*
import android.util.Log
import android.widget.Toast
import androidx.annotation.CallSuper
import androidx.window.layout.WindowMetricsCalculator
import org.godotengine.godot.FullScreenGodotApp
import org.godotengine.godot.GodotLib
import org.godotengine.godot.utils.PermissionsUtil
import org.godotengine.godot.utils.ProcessPhoenix
import java.util.*
import kotlin.math.min

/**
 * Base class for the Godot Android Editor activities.
 *
 * This provides the basic templates for the activities making up this application.
 * Each derived activity runs in its own process, which enable up to have several instances of
 * the Godot engine up and running at the same time.
 *
 * It also plays the role of the primary editor window.
 */
open class GodotEditor : FullScreenGodotApp() {

	companion object {
		private val TAG = GodotEditor::class.java.simpleName

		private const val WAIT_FOR_DEBUGGER = false

		private const val COMMAND_LINE_PARAMS = "command_line_params"

		internal const val EDITOR_ARG = "--editor"
		internal const val EDITOR_ARG_SHORT = "-e"

		internal const val PROJECT_MANAGER_ARG = "--project-manager"
		internal const val PROJECT_MANAGER_ARG_SHORT = "-p"

		private val EDITOR_MAIN_INFO =
			EditorInstanceInfo(GodotEditor::class.java, 777, ":GodotEditor")
		private val PROJECT_MANAGER_INFO =
			EditorInstanceInfo(GodotProjectManager::class.java, 555, ":GodotProjectManager")
	}

	private val runGameInfo: EditorInstanceInfo by lazy {
		EditorInstanceInfo(
			GodotGame::class.java,
			667,
			":GodotGame",
			Build.VERSION.SDK_INT >= Build.VERSION_CODES.N && (isInMultiWindowMode || isLargeScreen)
		)
	}

	private val commandLineParams = ArrayList<String>()

	override fun onCreate(savedInstanceState: Bundle?) {
		PermissionsUtil.requestManifestPermissions(this)

		val params = intent.getStringArrayExtra(COMMAND_LINE_PARAMS)
		updateCommandLineParams(params)

		if (BuildConfig.BUILD_TYPE == "dev" && WAIT_FOR_DEBUGGER) {
			Debug.waitForDebugger()
		}

		super.onCreate(savedInstanceState)
	}

	override fun onGodotSetupCompleted() {
		super.onGodotSetupCompleted()
		val longPressEnabled = enableLongPressGestures()
		val panScaleEnabled = enablePanAndScaleGestures()

		runOnUiThread {
			// Enable long press, panning and scaling gestures
			godotFragment?.renderView?.inputHandler?.apply {
				enableLongPress(longPressEnabled)
				enablePanningAndScalingGestures(panScaleEnabled)
			}
		}
	}

	@CallSuper
	protected open fun updateCommandLineParams(args: Array<String>?) {
		// Update the list of command line params with the new args
		commandLineParams.clear()
		if (args != null && args.isNotEmpty()) {
			commandLineParams.addAll(listOf(*args))
		}
	}

	final override fun getCommandLine() = commandLineParams

	protected open fun getEditorInstanceInfo(args: Array<String>): EditorInstanceInfo {
		for (arg in args) {
			if (EDITOR_ARG == arg || EDITOR_ARG_SHORT == arg) {
				return EDITOR_MAIN_INFO
			}

			if (PROJECT_MANAGER_ARG == arg || PROJECT_MANAGER_ARG_SHORT == arg) {
				return PROJECT_MANAGER_INFO
			}
		}

		return runGameInfo
	}

	final override fun onNewGodotInstanceRequested(args: Array<String>): Int {
		val editorInstanceInfo = getEditorInstanceInfo(args)

		// Launch a new activity
		val newInstance = Intent(this, editorInstanceInfo.instanceClass)
			.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK)
			.putExtra(COMMAND_LINE_PARAMS, args)
		if (editorInstanceInfo.launchAdjacent && Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
			newInstance.addFlags(Intent.FLAG_ACTIVITY_LAUNCH_ADJACENT)
		}
		if (editorInstanceInfo.instanceClass == javaClass) {
			Log.d(TAG, "Restarting ${editorInstanceInfo.instanceClass}")
			ProcessPhoenix.triggerRebirth(this, newInstance)
		} else {
			Log.d(TAG, "Starting ${editorInstanceInfo.instanceClass}")
			startActivity(newInstance)
		}
		return editorInstanceInfo.instanceId
	}

	protected open fun getProcessNameForInstanceId(instanceId: Int): String {
		return when (instanceId) {
			runGameInfo.instanceId -> {
				runGameInfo.processNameSuffix
			}
			EDITOR_MAIN_INFO.instanceId -> {
				EDITOR_MAIN_INFO.processNameSuffix
			}
			PROJECT_MANAGER_INFO.instanceId -> {
				PROJECT_MANAGER_INFO.processNameSuffix
			}
			else -> ""
		}
	}

	final override fun onGodotForceQuit(godotInstanceId: Int): Boolean {
		val processNameSuffix = getProcessNameForInstanceId(godotInstanceId)
		if (processNameSuffix.isNotBlank()) {
			val activityManager = getSystemService(Context.ACTIVITY_SERVICE) as ActivityManager
			val runningProcesses = activityManager.runningAppProcesses
			for (runningProcess in runningProcesses) {
				if (runningProcess.processName.endsWith(processNameSuffix)) {
					Log.v(TAG, "Killing Godot process ${runningProcess.processName}")
					Process.killProcess(runningProcess.pid)
					return true
				}
			}
		}
		return super.onGodotForceQuit(godotInstanceId)
	}

	// Get the screen's density scale
	private val isLargeScreen: Boolean
		// Get the minimum window size // Correspond to the EXPANDED window size class.
		get() {
			val metrics = WindowMetricsCalculator.getOrCreate().computeMaximumWindowMetrics(this)

			// Get the screen's density scale
			val scale = resources.displayMetrics.density

			// Get the minimum window size
			val minSize = min(metrics.bounds.width(), metrics.bounds.height()).toFloat()
			val minSizeDp = minSize / scale
			return minSizeDp >= 840f // Correspond to the EXPANDED window size class.
		}

	override fun setRequestedOrientation(requestedOrientation: Int) {
		if (!overrideOrientationRequest()) {
			super.setRequestedOrientation(requestedOrientation)
		}
	}

	/**
	 * The Godot Android Editor sets its own orientation via its AndroidManifest
	 */
	protected open fun overrideOrientationRequest() = true

	/**
	 * Enable long press gestures for the Godot Android editor.
	 */
	protected open fun enableLongPressGestures() =
		java.lang.Boolean.parseBoolean(GodotLib.getEditorSetting("interface/touchscreen/enable_long_press_as_right_click"))

	/**
	 * Enable pan and scale gestures for the Godot Android editor.
	 */
	protected open fun enablePanAndScaleGestures() =
		java.lang.Boolean.parseBoolean(GodotLib.getEditorSetting("interface/touchscreen/enable_pan_and_scale_gestures"))

	override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
		super.onActivityResult(requestCode, resultCode, data)
		// Check if we got the MANAGE_EXTERNAL_STORAGE permission
		if (requestCode == PermissionsUtil.REQUEST_MANAGE_EXTERNAL_STORAGE_REQ_CODE) {
			if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
				if (!Environment.isExternalStorageManager()) {
					Toast.makeText(
						this,
						R.string.denied_storage_permission_error_msg,
						Toast.LENGTH_LONG
					).show()
				}
			}
		}
	}

	override fun onRequestPermissionsResult(
		requestCode: Int,
		permissions: Array<String?>,
		grantResults: IntArray
	) {
		super.onRequestPermissionsResult(requestCode, permissions, grantResults)
		// Check if we got access to the necessary storage permissions
		if (requestCode == PermissionsUtil.REQUEST_ALL_PERMISSION_REQ_CODE) {
			if (Build.VERSION.SDK_INT < Build.VERSION_CODES.R) {
				var hasReadAccess = false
				var hasWriteAccess = false
				for (i in permissions.indices) {
					if (Manifest.permission.READ_EXTERNAL_STORAGE == permissions[i] && grantResults[i] == PackageManager.PERMISSION_GRANTED) {
						hasReadAccess = true
					}
					if (Manifest.permission.WRITE_EXTERNAL_STORAGE == permissions[i] && grantResults[i] == PackageManager.PERMISSION_GRANTED) {
						hasWriteAccess = true
					}
				}
				if (!hasReadAccess || !hasWriteAccess) {
					Toast.makeText(
						this,
						R.string.denied_storage_permission_error_msg,
						Toast.LENGTH_LONG
					).show()
				}
			}
		}
	}
}
