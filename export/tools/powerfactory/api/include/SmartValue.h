/* --------------------------------------------------------------
 * Copyright (c) 2017, AIT Austrian Institute of Technology GmbH.
 * All rights reserved. See file FMIPP_LICENSE for details.
 * --------------------------------------------------------------*/

#ifndef _PF_API_SMARTVALUE_H
#define _PF_API_SMARTVALUE_H

/**
 * \file SmartValue.h 
 *
 * \class SmartValue SmartValue.h 
 * Smart pointers for PF API Values.
 *  
 * \authors Roman Schwalbe, Matthias Stifter, Edmund Widl
 */


namespace pf_api {
 
class SmartValue
{

public:

	SmartValue( const Value* value ) : val( value ) {};
	SmartValue( const char* value ) : val( new Value( value ) ) {};
	SmartValue( const std::string value ) : val( new Value( value.c_str() ) ) {};
	SmartValue( const double value ) : val( new Value( value ) ) {};
	SmartValue( const int value ) : val( new Value( value ) ) {};
	SmartValue( const Value::Type value_type ) : val( new Value( value_type ) ) {};

	~SmartValue() {
		if( SmartValue::api&& val ) {
			int error = SmartValue::api->ReleaseValue( val );
			if ( 0 != error ) {
				LOG_ERROR << "PowerFactory: ERROR " << error << " while releasing value (0x" << std::hex << val << std::dec << ") ";
				
				Value::Type t = val->GetType();
				switch( t )
				{	case Value::STRING: LOG_ERROR << val->GetString(); break;
					case Value::DOUBLE: LOG_ERROR << val->GetDouble(); break;
					case Value::VECTOR: LOG_ERROR << "from type VECTOR"; break;
				}

				LOG_ERROR << std::endl;
			}
		}
	}

	inline const Value* operator->() { return val; }
	inline operator const Value*() const { return val; }

	const Value *val;
	static Api* api; /// \FIXME Works only with a single instance of PowerFactory Instance (but in fact only one can run at a time).

private:

	SmartValue() {};
	SmartValue( const SmartValue& ) {};
};


/**
 * \class SmartObject SmartValue.h 
 * Smart pointers for PF API Objects.
 *  
 * \authors Roman Schwalbe, Matthias Stifter, Edmund Widl
 */

class SmartObject
{

public:

	SmartObject( DataObject *object ) : obj( object ) {};

	~SmartObject() {
		if ( SmartValue::api&& obj ) {
			int error = SmartValue::api->ReleaseObject( obj );
			if ( 0 != error )
				LOG_ERROR << "PowerFactory: ERROR " << error << " while releasing object " << obj->GetFullName()->GetString() << " (0x" << std::hex << obj << std::dec << ") ";
		}
	}

	inline DataObject* operator->() { return obj; }
	inline DataObject* operator *() { return obj; }

private:

	DataObject *obj;

	SmartObject() {};
	SmartObject( const SmartObject& ) {};
};


} // namespace pf_api


#endif // _PF_API_SMARTVALUE_H