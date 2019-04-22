/*
 * EVALUATION AND USE OF THIS SOFTWARE IS SUBJECT TO THE TERMS AND
 * CONDITIONS OF THE CONTROLLING LICENSE AGREEMENT FOUND AT LICENSE.md
 * IN THIS SDK. IF YOU DO NOT AGREE TO THE LICENSE TERMS AND CONDITIONS,
 * PLEASE RETURN ALL SOURCE FILES TO SILICON LABORATORIES.
 * (c) Copyright 2018, Silicon Laboratories Inc.  All rights reserved.
 */

#pragma once

/*
 * EMWIN JPEG support requires extra RAM.  It can be disabled
 * and thus save memory by setting this to 0.  Note that the EMWIN JPEG 
 * functions will return an error if this is disabled due to the lack of 
 * memory.
 *
 * JPEG support must be enabled for the into animation to be displayed
 */
 
#define HURRICANE_CFG_SUPPORT_EMWIN_JPEG   1    