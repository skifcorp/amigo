/*
 * This file is part of the OpenSCADA project
 * Copyright (C) 2006-2010 TH4 SYSTEMS GmbH (http://th4-systems.com)
 *
 * OpenSCADA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * only, as published by the Free Software Foundation.
 *
 * OpenSCADA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License version 3 for more details
 * (a copy is included in the LICENSE file that accompanied this code).
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with OpenSCADA. If not, see
 * <http://opensource.org/licenses/lgpl-3.0.html> for a copy of the LGPLv3 License.
 */

package org.openscada.opc.dcom.da.impl;

import java.util.LinkedList;
import java.util.List;

import org.jinterop.dcom.common.JIException;
import org.jinterop.dcom.core.JIArray;
import org.jinterop.dcom.core.JIFlags;
import org.jinterop.dcom.core.JILocalCoClass;
import org.jinterop.dcom.core.JILocalInterfaceDefinition;
import org.jinterop.dcom.core.JILocalMethodDescriptor;
import org.jinterop.dcom.core.JILocalParamsDescriptor;
import org.jinterop.dcom.core.JIStruct;
import org.jinterop.dcom.core.JIVariant;
import org.openscada.opc.dcom.common.FILETIME;
import org.openscada.opc.dcom.common.KeyedResult;
import org.openscada.opc.dcom.common.KeyedResultSet;
import org.openscada.opc.dcom.common.Result;
import org.openscada.opc.dcom.common.ResultSet;
import org.openscada.opc.dcom.common.impl.EventHandlerImpl;
import org.openscada.opc.dcom.da.Constants;
import org.openscada.opc.dcom.da.IOPCDataCallback;
import org.openscada.opc.dcom.da.ValueData;

public class OPCDataCallback extends EventHandlerImpl
{
    private IOPCDataCallback _callback = null;

    private JILocalCoClass _coClass = null;

    public OPCDataCallback ()
    {
        super ();
    }

    public Object[] OnDataChange ( final int transactionId, final int serverGroupHandle, final int masterQuality, final int masterErrorCode, final int count, final JIArray clientHandles, final JIArray values, final JIArray qualities, final JIArray timestamps, final JIArray errors )
    {
        IOPCDataCallback callback = this._callback;
        if ( callback == null )
        {
            return new Object[] { org.openscada.opc.dcom.common.Constants.S_OK };
        }

        // get arrays for more readable code later ;-)
        Integer[] errorCodes = (Integer[])errors.getArrayInstance ();
        Integer[] itemHandles = (Integer[])clientHandles.getArrayInstance ();
        Short[] qualitiesArray = (Short[])qualities.getArrayInstance ();
        JIVariant[] valuesArray = (JIVariant[])values.getArrayInstance ();
        JIStruct[] timestampArray = (JIStruct[])timestamps.getArrayInstance ();

        // create result data
        KeyedResultSet<Integer, ValueData> result = new KeyedResultSet<Integer, ValueData> ();
        for ( int i = 0; i < count; i++ )
        {
            ValueData vd = new ValueData ();
            vd.setQuality ( qualitiesArray[i] );
            vd.setTimestamp ( FILETIME.fromStruct ( timestampArray[i] ).asCalendar () );
            vd.setValue ( valuesArray[i] );
            result.add ( new KeyedResult<Integer, ValueData> ( itemHandles[i], vd, errorCodes[i] ) );
        }

        // fire event
        try
        {
            callback.dataChange ( transactionId, serverGroupHandle, masterQuality, masterErrorCode, result );
        }
        catch ( Throwable e )
        {
            e.printStackTrace ();
        }

        // The client must always return S_OK
        return new Object[] { org.openscada.opc.dcom.common.Constants.S_OK };
    }

    public synchronized Object[] OnReadComplete ( final int transactionId, final int serverGroupHandle, final int masterQuality, final int masterErrorCode, final int count, final JIArray clientHandles, final JIArray values, final JIArray qualities, final JIArray timestamps, final JIArray errors )
    {
        if ( this._callback == null )
        {
            return new Object[] { org.openscada.opc.dcom.common.Constants.S_OK };
        }

        // get arrays for more readable code later ;-)
        Integer[] errorCodes = (Integer[])errors.getArrayInstance ();
        Integer[] itemHandles = (Integer[])clientHandles.getArrayInstance ();
        Short[] qualitiesArray = (Short[])qualities.getArrayInstance ();
        JIVariant[] valuesArray = (JIVariant[])values.getArrayInstance ();
        JIStruct[] timestampArray = (JIStruct[])timestamps.getArrayInstance ();

        // create result data
        KeyedResultSet<Integer, ValueData> result = new KeyedResultSet<Integer, ValueData> ();
        for ( int i = 0; i < count; i++ )
        {
            ValueData vd = new ValueData ();
            vd.setQuality ( qualitiesArray[i] );
            vd.setTimestamp ( FILETIME.fromStruct ( timestampArray[i] ).asCalendar () );
            vd.setValue ( valuesArray[i] );
            result.add ( new KeyedResult<Integer, ValueData> ( itemHandles[i], vd, errorCodes[i] ) );
        }

        // fire event
        try
        {
            this._callback.readComplete ( transactionId, serverGroupHandle, masterQuality, masterErrorCode, result );
        }
        catch ( Throwable e )
        {
            e.printStackTrace ();
        }

        // The client must always return S_OK
        return new Object[] { org.openscada.opc.dcom.common.Constants.S_OK };
    }

    public synchronized Object[] OnWriteComplete ( final int transactionId, final int serverGroupHandle, final int masterErrorCode, final int count, final JIArray clientHandles, final JIArray errors )
    {
        if ( this._callback == null )
        {
            return new Object[] { org.openscada.opc.dcom.common.Constants.S_OK };
        }

        // get arrays for more readable code later ;-)
        Integer[] errorCodes = (Integer[])errors.getArrayInstance ();
        Integer[] itemHandles = (Integer[])clientHandles.getArrayInstance ();

        // create result data
        ResultSet<Integer> result = new ResultSet<Integer> ();
        for ( int i = 0; i < count; i++ )
        {
            result.add ( new Result<Integer> ( itemHandles[i], errorCodes[i] ) );
        }

        // fire event
        try
        {
            this._callback.writeComplete ( transactionId, serverGroupHandle, masterErrorCode, result );
        }
        catch ( Throwable e )
        {
            e.printStackTrace ();
        }

        // The client must always return S_OK
        return new Object[] { org.openscada.opc.dcom.common.Constants.S_OK };
    }

    public synchronized Object[] OnCancelComplete ( final int transactionId, final int serverGroupHandle )
    {
        if ( this._callback == null )
        {
            return new Object[] { org.openscada.opc.dcom.common.Constants.S_OK };
        }

        this._callback.cancelComplete ( transactionId, serverGroupHandle );

        // The client must always return S_OK
        return new Object[] { org.openscada.opc.dcom.common.Constants.S_OK };
    }

    public synchronized JILocalCoClass getCoClass () throws JIException
    {
        if ( this._coClass != null )
        {
            return this._coClass;
        }

        this._coClass = new JILocalCoClass ( new JILocalInterfaceDefinition ( Constants.IOPCDataCallback_IID, false ), this, false );

        JILocalParamsDescriptor params;
        JILocalMethodDescriptor method;

        // OnDataChange
        params = new JILocalParamsDescriptor ();
        params.addInParamAsType ( Integer.class, JIFlags.FLAG_NULL ); // trans id
        params.addInParamAsType ( Integer.class, JIFlags.FLAG_NULL ); // group handle
        params.addInParamAsType ( Integer.class, JIFlags.FLAG_NULL ); // master quality
        params.addInParamAsType ( Integer.class, JIFlags.FLAG_NULL ); // master error
        params.addInParamAsType ( Integer.class, JIFlags.FLAG_NULL ); // count
        params.addInParamAsObject ( new JIArray ( Integer.class, null, 1, true ), JIFlags.FLAG_NULL ); // item handles
        params.addInParamAsObject ( new JIArray ( JIVariant.class, null, 1, true ), JIFlags.FLAG_NULL ); // values
        params.addInParamAsObject ( new JIArray ( Short.class, null, 1, true ), JIFlags.FLAG_NULL ); // qualities
        params.addInParamAsObject ( new JIArray ( FILETIME.getStruct (), null, 1, true ), JIFlags.FLAG_NULL ); // timestamps
        params.addInParamAsObject ( new JIArray ( Integer.class, null, 1, true ), JIFlags.FLAG_NULL ); // errors

        method = new JILocalMethodDescriptor ( "OnDataChange", params );
        this._coClass.getInterfaceDefinition ().addMethodDescriptor ( method );

        // OnReadComplete
        params = new JILocalParamsDescriptor ();
        params.addInParamAsType ( Integer.class, JIFlags.FLAG_NULL );
        params.addInParamAsType ( Integer.class, JIFlags.FLAG_NULL );
        params.addInParamAsType ( Integer.class, JIFlags.FLAG_NULL );
        params.addInParamAsType ( Integer.class, JIFlags.FLAG_NULL );
        params.addInParamAsType ( Integer.class, JIFlags.FLAG_NULL );
        params.addInParamAsObject ( new JIArray ( Integer.class, null, 1, true ), JIFlags.FLAG_NULL );
        params.addInParamAsObject ( new JIArray ( JIVariant.class, null, 1, true ), JIFlags.FLAG_NULL );
        params.addInParamAsObject ( new JIArray ( Short.class, null, 1, true ), JIFlags.FLAG_NULL );
        params.addInParamAsObject ( new JIArray ( FILETIME.getStruct (), null, 1, true ), JIFlags.FLAG_NULL );
        params.addInParamAsObject ( new JIArray ( Integer.class, null, 1, true ), JIFlags.FLAG_NULL );
        method = new JILocalMethodDescriptor ( "OnReadComplete", params );
        this._coClass.getInterfaceDefinition ().addMethodDescriptor ( method );

        // OnWriteComplete
        params = new JILocalParamsDescriptor ();
        params.addInParamAsType ( Integer.class, JIFlags.FLAG_NULL );
        params.addInParamAsType ( Integer.class, JIFlags.FLAG_NULL );
        params.addInParamAsType ( Integer.class, JIFlags.FLAG_NULL );
        params.addInParamAsType ( Integer.class, JIFlags.FLAG_NULL );
        params.addInParamAsObject ( new JIArray ( Integer.class, null, 1, true ), JIFlags.FLAG_NULL );
        params.addInParamAsObject ( new JIArray ( Integer.class, null, 1, true ), JIFlags.FLAG_NULL );
        method = new JILocalMethodDescriptor ( "OnWriteComplete", params );
        this._coClass.getInterfaceDefinition ().addMethodDescriptor ( method );

        // OnCancelComplete
        params = new JILocalParamsDescriptor ();
        params.addInParamAsType ( Integer.class, JIFlags.FLAG_NULL );
        params.addInParamAsType ( Integer.class, JIFlags.FLAG_NULL );
        method = new JILocalMethodDescriptor ( "OnCancelComplete", params );
        this._coClass.getInterfaceDefinition ().addMethodDescriptor ( method );

        // Add supported event interfaces
        List<String> eventInterfaces = new LinkedList<String> ();
        eventInterfaces.add ( Constants.IOPCDataCallback_IID );
        this._coClass.setSupportedEventInterfaces ( eventInterfaces );

        return this._coClass;
    }

    public void setCallback ( final IOPCDataCallback callback )
    {
        this._callback = callback;
    }

    public IOPCDataCallback getCallback ()
    {
        return this._callback;
    }
}
