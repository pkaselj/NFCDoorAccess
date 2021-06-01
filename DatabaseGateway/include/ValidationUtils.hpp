/*
*	 Copyright (C) Petar Kaselj 2021
*
*	 This file is part of NFCDoorAccess.
*
*	 NFCDoorAccess is written by Petar Kaselj as an employee of
*	 Emovis tehnologije d.o.o. which allowed its release under
*	 this license.
*
*    NFCDoorAccess is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NFCDoorAccess is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NFCDoorAccess.  If not, see <https://www.gnu.org/licenses/>.
*
*/

#ifndef VALIDATION_UTILS_HPP
#define VALIDATION_UTILS_HPP

#include"DataMailbox.hpp"
#include<regex>

Clearance destringifyClearance(const std::string& sClearance);
bool isValidClearance(Clearance clearance);
bool isValidCardUUID(const CardUUID& uuid);
bool isValidKeypadPassword(const KeyPass& password);
bool isValidPlainData(const std::string& data);
bool isValidSignedNumber(const std::string& data);
bool isParameterDataValid(const InputParameter& param);

// inclusive
/// Returns true if `pRequest` has between (including) min and max InputParameters. Set max to -1 (or omit it) for no upper bound.
bool hasParameterCount(CommandMessage* pRequest, int min, int max = -1)
{
	if (pRequest == nullptr)
	{
		return pRequest;
	}

	if (min < 0)
	{
		return false;
	}

	int parameterCount = pRequest->getParameterCount();

	if (max <= min)
	{
		return  parameterCount == min;
	}

	return parameterCount >= min && parameterCount <= max;
}

/// Returns true if `param` is one of the `allowedTypes`
bool isParameterType(const InputParameter& param, const std::initializer_list<InputParameter::enuType>& allowedTypes)
{
	for (const auto& type : allowedTypes)
	{
		if (param.getType() == type)
		{
			return true;
		}
	}

	return false;
}

/// Takes Clearance in string form and returns it as `Clearance` type (`Clearance` type is alias for signed char if not changed)
Clearance destringifyClearance(const std::string& sClearance)
{
	if (isValidSignedNumber(sClearance) == false)
	{
		// TODO default clearance?
		return FAIL_SAFE_CLEARANCE;
	}

	Clearance clearance = (Clearance)std::stoi(sClearance);
	return clearance;
}

/// Clearance is valid if it is contained in [-1, MAX_CLEARANCE>
bool isValidClearance(Clearance clearance)
{
	return clearance >= -1 && clearance < MAX_CLEARANCE;
}

/// Card UUID is valid if it is in form of "XX-XX-XX-XX" where X is hex numeral
bool isValidCardUUID(const CardUUID& uuid)
{
	std::regex mask("([a-zA-Z0-9]{2}-){3}[a-zA-Z0-9]{2}");

	bool valid = std::regex_match(uuid, mask);

	return valid;
}

/// Password is valid if it consists of 4 to 10 [inclusive] numerals (0-9)
bool isValidKeypadPassword(const KeyPass& password)
{
	std::regex mask("[0-9]{4,10}");

	bool valid = std::regex_match(password, mask);

	return valid;
}

/// Plain data is valid if it contains between 0 and 64 [inclusive] letters, dots, spaces, minus signs and/or plus signs
bool isValidPlainData(const std::string& data)
{
	// TODO Max length, TODO add anchors?, TODO space?
	std::regex mask("[a-zA-Z0-9 \\.\\-\\+]{0,64}");

	bool valid = std::regex_match(data, mask);

	return true;
}

/// Signed number is valid if it consists of 1 to 10 [inclusive] numerals and optional +/- sign at the beginning
bool isValidSignedNumber(const std::string& data)
{
	std::regex mask("(\\-|\\+)?[0-9]{1,10}");

	bool valid = std::regex_match(data, mask);

	return valid;
}

/// Returns true if data carried by InputParameter is valid
bool isParameterDataValid(const InputParameter& param)
{
	// Parameter with no data is an invalid parameter
	if (param.getData() == "")
	{
		return false;
	}

	switch (param.getType())
	{
		// same format as keypad PIN (TODO? or not)
	case InputParameter::enuType::KeypadCommand:
	case InputParameter::enuType::KeypadPIN:
		return isValidKeypadPassword(param.getData());

	case InputParameter::enuType::RFIDCard:
		return isValidCardUUID(param.getData());

	case InputParameter::enuType::PlainData:
		return isValidPlainData(param.getData());
	}

	return false;
}

/// Returns true if parameter `param` is one of `allowedTypes` AND if `param` data is valid
bool isParameterValid(const InputParameter& param, const std::initializer_list<InputParameter::enuType>& allowedTypes)
{
	bool parameterTypeCondition = isParameterType(param, allowedTypes);
	if (parameterTypeCondition == false) return false;

	bool parameterValidCondition = isParameterDataValid(param);
	if (parameterValidCondition == false) return false;

	return true;
}

#endif