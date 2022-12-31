/*************************************************************************/
/*  GodotXREditor.kt                                                     */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2022 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2022 Godot Engine contributors (cf. AUTHORS.md).   */
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

package org.godotengine.editor

import org.godotengine.godot.xr.XRMode

/**
 * Base class for the Godot Android XR Editor activities.
 */
open class GodotXREditor: GodotEditor() {
	override fun updateCommandLineParams(args: Array<String>?) {
		val updatedArgs = Array((args?.size?:0) + 3) {
			if (it == 0) {
				"--xr-mode"
			} else if (it == 1) {
				"on"
			}else if (it == 2) {
				XRMode.OPENXR.cmdLineArg
			} else {
				args?.get(it -3) ?: ""
			}
		}

		super.updateCommandLineParams(updatedArgs)
	}

	override fun shouldLaunchGodotInstanceAdjacent(args: Array<String>): Boolean {
		return false
	}

	override fun selectGodotInstanceTargetClass(args: Array<String>): Class<*> {
		// Parse the arguments to figure out which activity to start.
		var targetClass: Class<*> = GodotXRGame::class.java

		for (arg in args) {
			if (EDITOR_ARG == arg || EDITOR_ARG_SHORT == arg) {
				targetClass = GodotXREditor::class.java
				break
			}

			if (PROJECT_MANAGER_ARG == arg || PROJECT_MANAGER_ARG_SHORT == arg) {
				targetClass = GodotXRProjectManager::class.java
				break
			}
		}

		return targetClass
	}
}
