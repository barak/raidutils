/* Copyright (c) 1996-2004, Adaptec Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of the Adaptec Corporation nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

//File -  SCSI_LOG.CPP
//***************************************************************************
//
//Description:
//
//    This file contains function definitions for the dptSCSIlog_C
//class.
//
//Author:       Doug Anderson
//Date:         5/6/93
//
//Editors:
//
//Remarks:
//
//
//***************************************************************************


//Include Files -------------------------------------------------------------


#define dptNVRAM_computeChecksum        /* not inlined */
#include        "dpt_log.hpp"


//Function - dptSCSIlog_C::dptSCSIlog_C() - start
//===========================================================================
//
//Description:
//
//    This function is the constructor for the dptSCSIlog_C class.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

dptSCSIlog_C::dptSCSIlog_C() :
	param_P(NULL),
	paramBytes(0),
	validBytes(0),
	header_P(NULL)
{


}
//dptSCSIlog_C::dptSCSIlog_C() - end


//Function - dptSCSIlog_C - start
//===========================================================================
//
//Description:
//
//    These functions are virtual functions that do nothing in this class.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void    dptSCSIlog_C::reverseParam() {}

void    dptSCSIlog_C::calcInit() {}

void    dptSCSIlog_C::calcInfo() {}

void    dptSCSIlog_C::curOffsetInit() {}

void    dptSCSIlog_C::calcCurOffset() {}

//dptSCSIlog_C - end


//Function - dptSCSIlog_C::clear() - start
//===========================================================================
//
//Description:
//
//    This function initializes all data.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void    dptSCSIlog_C::clear()
{

    param_P = (logParam_S *)(header_P = (logHeader_S *)NULL);
    validBytes = paramBytes = 0;
    curOffsetInit();

}
//dptSCSIlog_C::clear() - end


//Function - dptSCSIlog_C::initSense() - start
//===========================================================================
//
//Description:
//
//    This function initializes the specified buffer as a log sense
//buffer.  This function will initialize all pointers and variables.
//One of three additional functions can be performed on the log buffer:
//  1. Nothing (Just initialize the pointers and variables)
//  2. Reverse the log sense information
//  3. Execute a virtual function to compute special initialization
//     information.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------
void SwapLog(logHeader_S * header_P)
{
    // Reverse the log header page length
    if (header_P != (logHeader_S *)NULL) {
        header_P->scsiSwap();
    }
}


uCHAR * dptSCSIlog_C::initSense(void *buff_P,uSHORT option)
{

    // Dis-associate the log data from any buffer
    clear();

    if (buff_P != (void *)NULL) {
        // Cast the data buffer as a log page header
        header_P = (logHeader_S *) buff_P;
        if (option==1) {
            // Reverse the page length
            SwapLog (header_P);
        }
        // Calculate the log sense info (optionally reverse bytes)
        calcSenseInfo(option);
    }

    return (data_P());

}
//dptSCSIlog_C::initSense() - end


//Function - dptSCSIlog_C::reset() - start
//===========================================================================
//
//Description:
//
//    This function initializes the CCB's log sense/select pointers.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uCHAR * dptSCSIlog_C::reset()
{

    if (header_P != (logHeader_S *)NULL) {
        paramBytes   = 0;
        curOffsetInit();
        param_P = (logParam_S *) header_P->getParams();
        // Insure there is enough space for the the first log parameter
        if (header_P->getPageLength() >= (4+param_P->getLength()))
            paramBytes += 4 + param_P->getLength();
        else
            param_P = (logParam_S *)NULL;
    }

    return (data_P());

}
//dptSCSIlog_C::reset() - end


//Function - dptSCSIlog_C::next() - start
//===========================================================================
//
//Description:
//
//    This function points the param_P pointer to the next log parameter.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uCHAR * dptSCSIlog_C::next()
{

    if (param_P != (logParam_S *)NULL) {
        // Calculate the byte offset of the current parameter
        calcCurOffset();
        // Increment to the next parameter
        param_P = (logParam_S *) (param_P->getData()+param_P->getLength());
        if ((header_P != (logHeader_S *)NULL) &&
            (header_P->getPageLength() >= (paramBytes+4+param_P->getLength())))
        {
            // Increment the log counter to the next paramter
            paramBytes += 4 + param_P->getLength();
        }
        else
            param_P = (logParam_S *)NULL;
    }

    return (data_P());

}
//dptSCSIlog_C::next() - end


//Function - dptSCSIlog_C::calcSenseInfo() - start
//===========================================================================
//
//Description:
//
//    This function sequences through the log parameters initializing
//all data.  If reverseEm is set, the log parameters are reversed
//before they are processed.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void    dptSCSIlog_C::calcSenseInfo(uSHORT reverseEm)
{

    // Reset to the first parameter
    reset();

    // Initialize log page specific variables
    calcInit();

    // Loop through all valid parameters
    while (param_P != (logParam_S *)NULL) {
        if (reverseEm) {
            // Reverse the parameter code bytes
            param_P->scsiSwap();
            // Reverse the parameter data bytes
            reverseParam();
        }
        // Calculate log page specific info
        calcInfo();
        // Get the next parameter
        next();
    }

    // Set the number of valid log bytes in this buffer
    validBytes = 4 + paramBytes;

    // Reset to the first parameter
    reset();

}
//dptSCSIlog_C::calcSenseInfo() - end


//Function - dptSCSIlog_C::findParam() - start
//===========================================================================
//
//Description:
//
//    This function finds either the first or the next parameter
//with the specified parameter code.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uCHAR * dptSCSIlog_C::findParam(uSHORT inCode,uSHORT fromStart)
{

    uSHORT       found = 0;

    // Reset the log pointers
    if (fromStart)
        reset();
    else
        next();
    while ( (param_P != (logParam_S *)NULL) && !found ) {
        // Check for the specified parameter code
        if (param_P->getCode()==inCode)
            found = 1;
        else
            // Get the next log parameter
            next();
    }

    return (data_P());

}
//dptSCSIlog_C::findParam() - end


//Function - dptSCSIlog_C::data_P() - start
//===========================================================================
//
//Description:
//
//    This function returns a pointer to the current parameters data
//buffer.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uCHAR * dptSCSIlog_C::data_P()
{

    if (param_P != (logParam_S *)NULL)
        return(param_P->getData());
    else
        return ((uCHAR *)NULL);

}
//dptSCSIlog_C::data_P() - end


//Function - dptSCSIlog_C::flags() - start
//===========================================================================
//
//Description:
//
//    This function returns the current parameter flag byte.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uCHAR   dptSCSIlog_C::flags()
{

    uCHAR        retVal = 0;

    if (param_P != (logParam_S *)NULL)
        retVal = param_P->getFlags();

    return (retVal);

}
//dptSCSIlog_C::flags() - end


//Function - dptSCSIlog_C::length() - start
//===========================================================================
//
//Description:
//
//    This function returns the current parameter data length.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uCHAR   dptSCSIlog_C::length()
{

    uCHAR        retVal = 0;

    if (param_P != (logParam_S *)NULL)
        retVal = param_P->getLength();

    return (retVal);

}
//dptSCSIlog_C::length() - end


//Function - dptSCSIlog_C::code() - start
//===========================================================================
//
//Description:
//
//    This function returns the current parameter code.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT  dptSCSIlog_C::code()
{

    uSHORT       retVal = 0xffff;

    if (param_P != (logParam_S *)NULL)
        retVal = param_P->getCode();

    return (retVal);

}
//dptSCSIlog_C::code() - end


//Function - dptSCSIlog_C::page() - start
//===========================================================================
//
//Description:
//
//    This function returns the log page code associated with this buffer.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uCHAR   dptSCSIlog_C::page()
{

    uCHAR        retVal = 0xff;

    if (header_P != (logHeader_S *)NULL)
        retVal = header_P->getPageCode() & 0x3f;

    return (retVal);

}
//dptSCSIlog_C::page() - end


//Function - dptSCSIlog_C::totalSize() - start
//===========================================================================
//
//Description:
//
//    This function returns the total # of bytes in this log buffer.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT  dptSCSIlog_C::totalSize()
{

    uSHORT       retVal = 0;

    if (header_P != (logHeader_S *)NULL)
        retVal = header_P->getPageLength() + 4;

    return (retVal);

}
//dptSCSIlog_C::totalSize() - end


//Function - dptSCSIlog_C::validSize() - start
//===========================================================================
//
//Description:
//
//    This function returns the number of valid parameter bytes + the
//log header size.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT  dptSCSIlog_C::validSize()
{

    uSHORT       retVal = 0;

    if (header_P != (logHeader_S *)NULL)
        retVal = validBytes;

    return (retVal);

}
//dptSCSIlog_C::validSize() - end

//Function - dptSCSIlog_C::size() - start
//===========================================================================
//
//Description:
//
//    This function returns the # of bytes for the current parameter
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uCHAR dptSCSIlog_C::size()
{
    uCHAR rtnVal = 0;

    if (param_P)
        rtnVal = param_P->getLength();

    return(rtnVal);
}
//dptSCSIlog_C::size() - end

//Function - dptSCSIlog_C::isValidParam() - start
//===========================================================================
//
//Description:
//
//    This function determines if the current parameted is valid.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uSHORT  dptSCSIlog_C::isValidParam()
{

    uSHORT       retVal = 0;

    if (param_P != (logParam_S *)NULL)
        retVal = 1;

    return (retVal);

}
//dptSCSIlog_C::isValidParam() - end

/*

//Function - dptSCSIlog_C::initSelect() - start
//===========================================================================
//
//Description:
//
//    This function initializes the specified buffer as a new log select
//buffer.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uCHAR * dptSCSIlog_C::initSelect(void *buff_P,uCHAR pageCode)
{

    if (buff_P == (void *)NULL)
        // Dis-associate the log data from any buffer
        clear();
    else {
        // Cast the data buffer as a log page header
        header_P = (logHeader_S *) buff_P;
        // Set the log page code
        header_P->setPageCode(pageCode & 0x3f);
        header_P->setReserved(0);
        header_P->setPageLength(0);
        param_P = (logParam_S *) header_P->getParams();
        validBytes = paramBytes = 0;
    }

    return (data_P());

}
//dptSCSIlog_C::initSelect() - end


//Function - dptSCSIlog_C::setParam() - start
//===========================================================================
//
//Description:
//
//    This function sets the specified log paramter.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void    dptSCSIlog_C::setParam(uSHORT code,uCHAR dataLen,uCHAR inFlags)
{

    if (param_P != (logParam_S *)NULL) {
        // Set the log parameter code
        param_P->setCode(code);
        // Set the parameter flags
        param_P->setFlags(inFlags);
        // Set the data length
        param_P->setLength(dataLen);
        // Set the parameter byte count
        paramBytes += 4 + dataLen;
        // Set the log header page length
        header_P->setPageLength(paramBytes);
    }

}
//dptSCSIlog_C::setParam() - end


//Function - dptSCSIlog_C::nextNew() - start
//===========================================================================
//
//Description:
//
//    This function increments the parameter pointer to the next
//new parameter to be set.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

uCHAR * dptSCSIlog_C::nextNew()
{

    if (param_P != (logParam_S *)NULL)
        // Increment to the next parameter
        param_P = (logParam_S *) (param_P->getData()+param_P->getLength());

    return (data_P());

}
//dptSCSIlog_C::nextNew() - end


//Function - dptSCSIlog_C::selReverse() - start
//===========================================================================
//
//Description:
//
//    This function reverses all the log select data for output.
//
//Parameters:
//
//Return Value:
//
//Global Variables Affected:
//
//Remarks: (Side effects, Assumptions, Warnings...)
//
//
//---------------------------------------------------------------------------

void    dptSCSIlog_C::selReverse()
{

    // Reverse all the parameter codes
    reverseAllParams();

    // Reverse the log header page length
    SwapLog(header_P);

}
//dptSCSIlog_C::selReverse() - end

*/
