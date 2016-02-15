#include"MatasanoConverter.h"

uint8_t MatasanoConverter::numberFromHexChar(char hexCharacter) {
	//cout << hexCharacter << endl;
	if (48 <= hexCharacter && hexCharacter <= 57) { //0-9
		return (uint8_t) hexCharacter - 48;
	} else if (65 <= hexCharacter && hexCharacter <= 70) { //A-F
		return (uint8_t) hexCharacter - 55;
	} else if (97 <= hexCharacter && hexCharacter <= 102) { //a-f
		return (uint8_t) hexCharacter - 87;
	} else { //other - error!
		throw invalid_argument("numberFromHexChar: hexCharacter not valid.");
	}
}


vector<uint8_t> MatasanoConverter::base64Splitter(vector<uint8_t> eightBitNumberArray) {
	uint8_t temp;
	vector<uint8_t> base64Output;
	base64Output.clear(); //may be redundant
	
	for (int i = 0; i < eightBitNumberArray.size(); i+=3) { //three 8 bit numbers are taken at once and converted into four 6 bit numbers
		temp = eightBitNumberArray[i] >> 2;
		base64Output.push_back(temp);
		temp = (eightBitNumberArray[ i ] & 0x3) << 4 | eightBitNumberArray[i+1] >> 4;
		base64Output.push_back(temp);
		temp = (eightBitNumberArray[i+1] & 0xF) << 2 | eightBitNumberArray[i+2] >> 6;
		base64Output.push_back(temp);
		temp = eightBitNumberArray[i+2] & 0x3F;
		base64Output.push_back(temp);
	}
	
	return base64Output;
}


vector<uint8_t> MatasanoConverter::hexSplitter(vector<uint8_t> eightBitNumberArray) {
	uint8_t temp;
	vector<uint8_t> hexOutput;
	hexOutput.clear(); //may be redundant
	
	for (int i = 0; i < eightBitNumberArray.size(); i++) {
		temp = eightBitNumberArray[i] >> 4;
		hexOutput.push_back(temp);
		temp = eightBitNumberArray[i] & 0xF;
		hexOutput.push_back(temp);
	}
	
	return hexOutput;
}


char MatasanoConverter::base64Character(uint8_t base64Number) {
	if (0 <= base64Number && base64Number <= 25) { //upper case characters
		return (char) base64Number + 65;
	} else if (26 <= base64Number && base64Number <= 51) { //lower case characters
		return (char) base64Number + 71;
	} else if (52 <= base64Number && base64Number <= 61) { //numbers
		return (char) base64Number - 4;
	} else if (62 == base64Number) { //+
		return '+';
	} else if (63 == base64Number) { // /
		return '/';
	} else {
		throw invalid_argument("base64Character: invalid base64 number (not a number 0-63)");
	}
}


char MatasanoConverter::hexCharacter(uint8_t hexNumber) {
	if (0 <= hexNumber && hexNumber <= 9) {
		return (char) hexNumber + 48;
	} else if (10 <= hexNumber && hexNumber <= 15) {
		return (char) hexNumber + 87;
	} else {
		throw invalid_argument("hexCharacter: invalid hex number (not a number 0-15)");
	}
}


uint8_t MatasanoConverter::combineHex(uint8_t MSB, uint8_t LSB) {
	return (MSB << 4) | LSB;
}


void MatasanoConverter::hexStringInput(string inputString) {
	//need to loop over each character and add two characters at a time to the uint8_t array
	uint32_t max = inputString.length();
	uint8_t a = 0, b = 0;
	
	if (max % 2 != 0) {
		throw invalid_argument("hexStringInput: inputString not valid hex - uneven length.");
	}
	
	converterData.clear();
	
	for(uint32_t i = 0; i < max; i+=2) {
		a = numberFromHexChar(inputString[i]);
		b = numberFromHexChar(inputString[i + 1]);
		converterData.push_back( combineHex( a , b ) );
	}
}


void MatasanoConverter::loadString(string inputString, string inputType) {
	//this function uses an if statement and the hexStringInput function to allow for easy expansion of the MatasanoConverter class
	//e.g. for base64 input
	if (inputType.compare("hex") == 0 || inputType.compare("Hex") == 0 || inputType.compare("h") == 0) { 
		hexStringInput(inputString);
	} else {
		throw invalid_argument("inputString: unknown inputType.");
	}
}


string MatasanoConverter::getBase64() {
	vector<uint8_t> outputDataCopy = converterData; //create copy that can be safely modified if needed
	char paddingBytes; //counter to record number of bytes added - value will be 0, 1 or 2
	string base64String = "";

	if (converterData.size() % 3 == 1) {
		//add two pad bytes
		outputDataCopy.push_back(0);
		outputDataCopy.push_back(0);
		paddingBytes = 2;
	} else if (converterData.size() % 3 == 2) {
		//add one pad byte
		outputDataCopy.push_back(0);
		paddingBytes = 1;
	} else {
		//no padding required
		paddingBytes = 0;
	}
	
	outputDataCopy = base64Splitter(outputDataCopy);
	
	for (int i = 0; i < outputDataCopy.size(); i++) {
		base64String += base64Character(outputDataCopy[i]);
	}
	
	if (paddingBytes == 1) {
		base64String.replace(base64String.length() - 2, 2, "=", 0, 1);
	} else if (paddingBytes == 2) {
		base64String.replace(base64String.length() - 3, 3, "==", 0, 2);
	}
	
	outputDataCopy.clear(); //destroy the vector as the copy is no longer needed
	
	return base64String;
}

string MatasanoConverter::getHex() {
	vector<uint8_t> outputDataCopy = converterData; //create copy that can be safely modified if needed
	string hexString = "";
	
	outputDataCopy = hexSplitter(outputDataCopy);
	
	for (int i = 0; i < outputDataCopy.size(); i++) {
		hexString += hexCharacter(outputDataCopy[i]);
	}
	
	return hexString;
}

string MatasanoConverter::getStringOutput(string outputType) {
	string temp; //needed for upper case hex output
	if (outputType.compare("base64") == 0 || outputType.compare("b64") == 0 || outputType.compare("Base64") == 0 || outputType.compare("B64") == 0) {
		return getBase64();	
	} else if (outputType.compare("hex") == 0 || outputType.compare("h") == 0) { //returns lower case hex output
		return getHex();
	} else if (outputType.compare("H") == 0 || outputType.compare("Hex") == 0 || outputType.compare("HEX") == 0) { //returns upper case hex output
		temp = getHex();
		transform(temp.begin(), temp.end(), temp.begin(), ::toupper);
		return temp;
	} else {
		throw invalid_argument("outputString: unknown outputType.");
	}
	
}

