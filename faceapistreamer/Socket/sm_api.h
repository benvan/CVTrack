/*
	Copyright (C) 2008 Seeing Machines Ltd. All rights reserved.

	This file is part of the FaceTrackingAPI, also referred to as "faceAPI".

	This file may be distributed under the terms of the Seeing Machines 
	FaceTrackingAPI Non-Commercial License Agreement.

	This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
	WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

	Further information about faceAPI licensing is available at:
	http://www.seeingmachines.com/faceapi/licensing.html
*/
#ifndef SM_API_H
#define SM_API_H

/*! @file
    The master include file for the Face Tracking API. */

// Compiler and OS configuration
#include "sm_api_configure.h"

// Basic includes
#include "sm_api_returncodes.h"
#include "sm_api_string.h"
#include "sm_api_logging.h"
#include "sm_api_geomtypes.h"
#include "sm_api_imagetypes.h"
#include "sm_api_image.h"
#include "sm_api_time.h"
#include "sm_api_facelandmark.h"
#include "sm_api_facetexture.h"

// Engine includes
#include "sm_api_cameratypes.h"
#include "sm_api_camera.h"
#include "sm_api_engine.h"
#include "sm_api_videodisplay.h"
#include "sm_api_headtracker.h"
#include "sm_api_headtrackercontrols.h"
#include "sm_api_headtrackerv1controls.h"
#include "sm_api_headtrackerv2controls.h"

// File and image tracking functions
#include "sm_api_facesearch.h"
#include "sm_api_filetrack.h"

// Utility functions
#include "sm_api_coordutils.h"
#include "sm_api_ar.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*! @brief Gets the version of the API.

    This function is @a reentrant.
    @param major Major number. The API interface has changed you will need to recompile.
    @param minor Minor number. New interfaces may have been added to the API, but binary compatibility is maintained.
    @param maint Maintenance number. The API interface is unaltered. 
    @return @ref smReturnCode "SM_API_OK" if the function completed successfully.
    @note 
    - This function can be called before smAPIInit() */
SM_API(smReturnCode) smAPIVersion(int *major, int *minor, int *maint);

/*! @return SM_API_TRUE if the API is a non-commercial license, SM_API_FALSE otherwise. 
    @see @ref sm_api_license */
SM_API(smBool) smAPINonCommercialLicense();

/*! @brief Gets a string describing the license status, including any time-trial expiry date.

    @param buff Pointer to user-allocated buffer of char, or 0 to determine maximum required length.
    @param size Must point to an existing integer. Set this to the size of your input buffer. 
           If @a buff is 0, is set to the required size for the string, otherwise set to the number of characters copied including trailing null character.
    @param detailed If true, a more detailed license string is generated.
    @return SM_API_OK if function executed successfully 
    @note 
    - This function can be called before smAPIInit()

    @code
    // C++ example
    char *buff;
    int size;
    smReturnCode error = smAPILicenseInfoString(0,&size,SM_API_FALSE);
    assert(!error);
    buff = new char[size];
    error = smAPILicenseInfoString(buff,&size,SM_API_FALSE);
    assert(!error);
    cout << "API license info: " << buff << std::endl;
    @endcode */
SM_API(smReturnCode) smAPILicenseInfoString(char *buff, int *size, smBool detailed);

/*! @brief Notify faceAPI that Qt is being used by the calling process. 

    This disables the internal use of Qt GUI classes. 
    
    This call will disable all video-display related functions such as smVideoDisplayCreate().

    @note This call must be made before smAPIInit() or it has no effect. 

    @return @ref smReturnCode "SM_API_OK" if the function completed successfully. 
    @return @ref smReturnCode "SM_API_FAIL_INVALID_CALL" if smAPIInit() has already been called. */
SM_API(smReturnCode) smAPIInternalQtGuiDisable();

/*! @brief Get status of internal Qt Gui disablement. 

    @param Pointer to an existing smBool, set to SM_API_TRUE if Qt Gui usage is disabled.
    @return @ref smReturnCode "SM_API_OK" if the function completed successfully. 
    
    @see
    - smAPISetDisableInternalQtGui() */
SM_API(smBool) smAPIInternalQtGuiIsDisabled();

/*! @brief Initializes the API.

    Almost all other functions will return SM_API_FAIL_INVALID_CALL until this function is called.
    The current list of exceptions are:
    - smAPIVersion()
    - smAPINonCommercialLicense()
    - smAPILicenseInfoString()
    - smLoggingRegisterCallback()
    - smLoggingSetFileOutputEnable()
    - smLoggingGetFileOutputEnable()

    @return @ref smReturnCode "SM_API_OK" if the function completed successfully. 
    @return @ref smReturnCode "SM_API_FAIL_NO_LICENSE" if a valid license could not be determined.

    @note This function should be called on the main thread. Calling this function will
          initialize COM for the thread as a Single Threaded Apartment. At present the API
          may not work if COM has already been initialized as MTA. */
SM_API(smReturnCode) smAPIInit();

/*! @brief For production license builds, use this function instead of smAPIInit(). 
    @param dummy This parameter is reserved for future releases. Pass in null. 
    @return @ref smReturnCode "SM_API_OK" if the function completed successfully. 
    @return @ref smReturnCode "SM_API_FAIL_NO_LICENSE" if a valid license could not be determined. */
SM_API(smReturnCode) smAPIInitProductionLicense(const char *dummy);

/*! @brief Call at the end of your program to clean up API resources

    @return @ref smReturnCode "SM_API_OK" if the function completed successfully. */
SM_API(smReturnCode) smAPIQuit();

/*! @brief Manually process any window events

    This is only required for console-style applications which
    need to show a video display window and that do not have an 
    existing window event loop. It may also be called to manually
    update the video window when an application is showing a modal
    dialog.

    <b>Example code:</b>
    @code
    void main()
    {
        // Initialize API...
        // Create and start an engine...
        // Create a video-display
        while (!quit)
        {
            // Perhaps check if the ESC key has been hit
            ...
            // Refresh video display
            smAPIProcessEvents();
            // Prevent CPU overload in this loop
            Sleep(30);
        }
    }
    @endcode 

    @return @ref smReturnCode "SM_API_OK" if the function completed successfully. */
SM_API(smReturnCode) smAPIProcessEvents();

/*! @brief If a VideoDisplay is shown, then this function needs to be called before entering a modal event loop.
    
    <b>Example code:</b>
    @code
    smAPIEnterModalLoop();
    MessageBox(...);
    smAPIExitModalLoop();
    @endcode
    
    @see
    - smAPIExitModalLoop() 

    @return @ref smReturnCode "SM_API_OK" if the function completed successfully. */
SM_API(smReturnCode) smAPIEnterModalLoop();

/*! @brief If a VideoDisplay is shown, then this function needs to be called after exiting a modal event loop.

    <b>Example code:</b>
    @code
    smAPIEnterModalLoop();
    MessageBox(...);
    smAPIExitModalLoop();
    @endcode
    
    @see
    - smAPIEnterModalLoop() 
    
    @return @ref smReturnCode "SM_API_OK" if the function completed successfully. */
SM_API(smReturnCode) smAPIExitModalLoop();

/*! @brief Set the default icon for all windows that faceAPI can create. 
    @return @ref smReturnCode "SM_API_OK" if the function completed successfully. */
SM_API(smReturnCode) smAPISetIconFromPNG(smStringHandle icon_png_filename);

#ifdef __cplusplus
}
#endif
#endif
