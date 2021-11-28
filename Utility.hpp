#ifndef UTILITY_HPP
#define UTILITY_HPP

#pragma once

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <stack>
#include <deque>
#include <random>

// trim from start (in place)
static inline void ltrim(std::string &s) 
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
		std::not1(std::ptr_fun<int, int>(std::isspace))));
}

// trim from end (in place)
static inline void rtrim(std::string &s) 
{
	s.erase(std::find_if(s.rbegin(), s.rend(),
		std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) 
{
	ltrim(s);
	rtrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) 
{
	ltrim(s);
	return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) 
{
	rtrim(s);
	return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) 
{
	trim(s);
	return s;
}

static inline size_t randomGenerator(size_t min, size_t max)
{
	std::mt19937 rng;
	rng.seed(std::random_device()());
	//rng.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);

	return dist(rng);
}

static inline std::vector<std::string> split(const std::string& s, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter))
	{
		tokens.emplace_back(token);
	}
	return tokens;
}

static inline void skipComments(std::string& str)
{
	auto pos = str.find("#");
	if (pos != std::string::npos)
	{
		str.erase(pos);
	}
}

static inline std::string GetConfigSettingsValue(std::string line, std::string &variable)
{
	std::string valuestr = "";
	std::vector<std::string> splittedLine = split(line, '=');
	variable = "";
	if (splittedLine.size() > 1)
	{
		variable = splittedLine[0];
		trim(variable);

		valuestr = splittedLine[1];
		trim(valuestr);
	}
	else
	{
		variable = line;
		trim(variable);
		valuestr = "0";
	}

	return valuestr;
}

static inline std::string GetConfigSettingsValue3(std::string line, std::string &variable, std::string &variable2)
{
	std::string valuestr = "";
	std::vector<std::string> splittedLine = split(line, '=');
	variable = "";
	if (splittedLine.size() > 1)
	{
		variable = splittedLine[0];
		trim(variable);

		valuestr = splittedLine[1];
		trim(valuestr);

		if (splittedLine.size() > 2)
		{
			variable2 = splittedLine[2];
			trim(variable2);
		}
	}
	else
	{
		variable = line;
		trim(variable);
		valuestr = "0";
	}

	return valuestr;
}

static inline bool Contains(std::string str, std::string ministr)
{
	if (str.find(ministr) != std::string::npos) {
		return true;
	}
	else
		return false;
}

static inline UInt32 getHex(std::string hexstr)
{
	return (UInt32)strtoul(hexstr.c_str(), 0, 16);
}

template <typename I> static inline std::string num2hex(I w, size_t hex_len = sizeof(I) << 1) {
	static const char* digits = "0123456789ABCDEF";
	std::string rc(hex_len, '0');
	for (size_t i = 0, j = (hex_len - 1) * 4; i<hex_len; ++i, j -= 4)
		rc[i] = digits[(w >> j) & 0x0f];
	return rc;
}

static inline void replaceAll(std::string& str, const std::string& from, const std::string& to) {
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

static inline std::string removeWhiteSpaces(std::string s)
{
	std::string s2;
	s2.reserve(s.size());
	std::remove_copy_if(begin(s), end(s), std::back_inserter(s2), [l = std::locale{}](auto ch) { return std::isspace(ch, l); });
	return s2;
}

static inline bool has_suffix(const std::string &str, const std::string &suffix)
{
	return str.size() >= suffix.size() &&
		str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

static inline void ConvertToCamelCase(char * &str)
{
	bool active = true;

	for (int i = 0; str[i] != '\0'; i++) {
		if (std::isalpha(str[i])) {
			if (active) {
				str[i] = std::toupper(str[i]);
				active = false;
			}
			else {
				str[i] = std::tolower(str[i]);
			}
		}
		else if (str[i] == ' ') {
			active = true;
		}
	}
}

static inline uint64_t GetButtonMaskFromId(int id)
{
	return 1ull << id;
}

// get mod index from a normal form ID 32 bit unsigned
static inline UInt32 GetModIndex(UInt32 formId)
{
	return formId >> 24;
}

// get base formID (without mod index)
static inline UInt32 GetBaseFormID(UInt32 formId)
{
	return formId & 0x00FFFFFF;
}

// check if mod index is valid (mod index is the upper 8 bits of form ID)
static inline bool IsValidModIndex(UInt32 modIndex)
{
	return modIndex > 0 && modIndex != 0xFF;
}

static inline NiPoint3 GetPointFromRatio(NiPoint3 low, NiPoint3 high, float ratio)
{
	return low + ((high - low)*ratio);
}

static inline float distance2D(NiPoint3 po1, NiPoint3 po2)
{
	float x = po1.x - po2.x;
	float y = po1.y - po2.y;
	return sqrtf(x*x + y*y);
}

static inline float distanceNoSqrt(NiPoint3 po1, NiPoint3 po2)
{
	/*LARGE_INTEGER startingTime, endingTime, elapsedMicroseconds;
	LARGE_INTEGER frequency;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&startingTime);
	_MESSAGE("distanceNoSqrt Start");*/

	float x = po1.x - po2.x;
	float y = po1.y - po2.y;
	float z = po1.z - po2.z;
	float result = x*x + y*y + z*z;

	/*QueryPerformanceCounter(&endingTime);
	elapsedMicroseconds.QuadPart = endingTime.QuadPart - startingTime.QuadPart;
	elapsedMicroseconds.QuadPart *= 1000000000LL;
	elapsedMicroseconds.QuadPart /= frequency.QuadPart;
	_MESSAGE("distanceNoSqrt Update Time = %lld ns\n", elapsedMicroseconds.QuadPart);*/
	return result;
}

static inline bool CompareNiPoints(NiPoint3 collisionVector, NiPoint3 emptyPoint)
{
	return collisionVector.x == emptyPoint.x && collisionVector.y == emptyPoint.y && collisionVector.z == emptyPoint.z;
}

// Calculates a dot product
static inline float dot(NiPoint3 a, NiPoint3 b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

// Calculates a cross product
static inline NiPoint3 cross(NiPoint3 a, NiPoint3 b)
{
	return NiPoint3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

static inline void GetAttitudeAndHeadingFromTwoPoints(NiPoint3 source, NiPoint3 target, float &attitude, float &heading)
{
	NiPoint3 vector = target - source;

	const float sqr = vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
	if (sqr != 0)
	{
		vector = vector * (1.0f / sqrtf(sqr));

		attitude = (float)atan2(vector.z, sqrtf(vector.y*vector.y + vector.x*vector.x)) * -1;

		heading = (float)atan2(vector.x, vector.y);
	}
	else
	{
		attitude = 0;
		heading = 0;
	}
}

//returns true if clockwise, false if counter clockwise
static inline bool AreAnglesTurningClockwise(float angle1, float angle2, float angle3)
{
	if (angle1 < 0)
		angle1 = 360.0f - abs(angle1);

	if (angle2 < 0)
		angle2 = 360.0f - abs(angle2);

	if (angle3 < 0)
		angle3 = 360.0f - abs(angle3);

	//written by my sister (someone else's sister now muahahaha)
	if (angle2 > angle1 && angle1 > 180.0f) 
	{
		return true;
	}
	else if (angle2 > angle1 && angle1 < 180.0f) 
	{
		if ((angle1 + angle2) < 180.0f) 
		{
			return true;
		}
		else 
		{
			if (angle3 < angle2) 
			{
				return false;
			}
			else 
			{
				return true;
			}
		}
	}
	else 
	{
		return false;
	}
}


//returns true if clockwise, false if counter clockwise
static inline bool AreAnglesTurningClockwiseOLD(float angle1, float angle2, float angle3)
{
	if (angle1 < 0)
		angle1 = 360.0f - abs(angle1);

	if (angle2 < 0)
		angle2 = 360.0f - abs(angle2);

	if (angle3 < 0)
		angle3 = 360.0f - abs(angle3);

	//written by me, hate this code (someone else hahaha)
	if(angle2 - angle1 < 0 && angle3 - angle2 < 0)
	{
		//CCW
		return false;
	}
	else if(angle1 - angle2 < 0 && angle2 - angle3 < 0)
	{
		//CW
		return true;
	}
	else
	{
		if(angle1 - angle2 > 0 && angle2 - angle3 < -180.0f)
		{
			if (angle1 - angle2 > 0 && angle2 - (angle3 - 360.0f) > 0)
			{
				//CCW
				return false;
			}
			else
				return true;
		}
		else if(angle1 - angle2 < -180.0f && angle2 - angle3 > 0) 
		{
			if ((angle1 +360.0f) - angle2 > 0 && angle2 - angle3 > 0)
			{
				//CCW
				return false;
			}
			else
				return true;
		}
		else if(angle1 - angle2 > 180.0f && angle2 - angle3 < 0)
		{
			if ((angle1 - 360.0f) - angle2 < 0 && angle2 - angle3 < 0)
			{
				//CW
				return true;
			}
			else
				return false;			
		}
		else if(angle1 - angle2 < 0 && angle2 - angle3 > 180.0f)
		{
			if (angle1 - angle2 < 0 && angle2 - (angle3 + 360.0f) < 0)
			{
				//CW
				return true;
			}
			else
				return false;
		}
		else
			return true;		
	}	
}

static inline float AngleDifference(float angle1, float angle2)
{	
	if((angle1 < -90 && angle2 > 90) || (angle2 < -90 && angle1 > 90))
	{
		return (180 - abs(angle1)) + (180 - abs(angle2));
	}
	else if((angle1 > -90 && 0 >= angle1 && angle2 < 90 && 0 <= angle2) || (angle2 > -90 && 0 >= angle2 && angle1 < 90 && 0 <= angle1))
	{
		return abs(angle1) + abs(angle2);
	}
	else
	{
		return abs(angle1 - angle2);
	}
}

template <typename T, int MaxLen, typename Container = std::deque<T>>
class FixedQueue : public std::queue<T, Container> {
public:
	void push(const T& value) {
		if (this->size() == MaxLen) {
			this->c.pop_front();
		}
		std::queue<T, Container>::push(value);
	}
};

// Names should be the full INI setting name followed by a colon and it's category (Prog's code)
// For example: bAlwaysShowHands:VR
static inline double vlibGetSetting(const char * name) {
	Setting * setting = GetINISetting(name);
	double value;
	if (!setting)
		return -1;
	if (setting->GetDouble(&value))
		return value;
	return -1;
}


// a = attitude, b = bank, h = heading, c = cos, s = sin
static inline void SetRotationMatrix(NiMatrix33 *mat, double sacb, double cacb, double sb)
{
	double cb = sqrt(1 - sb*sb);
	double ca = cacb / cb, sa = sacb / cb;
	mat->data[0][0] = ca;
	mat->data[0][1] = -sacb;
	mat->data[0][2] = sa * sb;
	mat->data[1][0] = sa;
	mat->data[1][1] = cacb;
	mat->data[1][2] = -ca * sb;
	mat->data[2][0] = 0.0;
	mat->data[2][1] = sb;
	mat->data[2][2] = cb;
}

static inline NiPoint3 ConvertRotation(NiMatrix33 mat)
{
	float heading;
	float attitude;
	float bank;
	mat.GetEulerAngles(&heading, &attitude, &bank);
	return NiPoint3(heading, attitude, bank);
}

// Check if a pointer is bad by following it, reading memory, and catching any exception (Prog's code)
static inline bool IsBadPtr(const void * p, size_t bytes)
{
	try 
	{
		memcmp(p, p, bytes);
	}
	catch (...) 
	{
		return true;
	}
	return false;
}

// Controls visibility visibility of an NiNode (Prog's code)
static inline void setVisible(NiAVObject * node, bool isVisible) {
	if (node) {
		if (isVisible)
			*(uint8_t*)(((uintptr_t)node) + 0x10C) &= 0xFE;
		else
			*(uint8_t*)(((uintptr_t)node) + 0x10C) |= 0x01;
	}
}

static inline bool isVisible(NiAVObject * node) {
	if (!node)
		return false;
	bool isHidden = (*(uint8_t*)(((uintptr_t)node) + 0x10C) & 0x01);
	return !isHidden;
}

// Interpolate between two rotation matrices using quaternion math (Prog's code)
static inline NiMatrix33 slerpMatrix(float interp, NiMatrix33 mat1, NiMatrix33 mat2) {
	// Convert mat1 to a quaternion
	float q1w = sqrt(max(0, 1 + mat1.data[0][0] + mat1.data[1][1] + mat1.data[2][2])) / 2;
	float q1x = sqrt(max(0, 1 + mat1.data[0][0] - mat1.data[1][1] - mat1.data[2][2])) / 2;
	float q1y = sqrt(max(0, 1 - mat1.data[0][0] + mat1.data[1][1] - mat1.data[2][2])) / 2;
	float q1z = sqrt(max(0, 1 - mat1.data[0][0] - mat1.data[1][1] + mat1.data[2][2])) / 2;
	q1x = _copysign(q1x, mat1.data[2][1] - mat1.data[1][2]);
	q1y = _copysign(q1y, mat1.data[0][2] - mat1.data[2][0]);
	q1z = _copysign(q1z, mat1.data[1][0] - mat1.data[0][1]);

	// Convert mat2 to a quaternion
	float q2w = sqrt(max(0, 1 + mat2.data[0][0] + mat2.data[1][1] + mat2.data[2][2])) / 2;
	float q2x = sqrt(max(0, 1 + mat2.data[0][0] - mat2.data[1][1] - mat2.data[2][2])) / 2;
	float q2y = sqrt(max(0, 1 - mat2.data[0][0] + mat2.data[1][1] - mat2.data[2][2])) / 2;
	float q2z = sqrt(max(0, 1 - mat2.data[0][0] - mat2.data[1][1] + mat2.data[2][2])) / 2;
	q2x = _copysign(q2x, mat2.data[2][1] - mat2.data[1][2]);
	q2y = _copysign(q2y, mat2.data[0][2] - mat2.data[2][0]);
	q2z = _copysign(q2z, mat2.data[1][0] - mat2.data[0][1]);

	// Take the dot product, inverting q2 if it is negative
	double dot = q1w*q2w + q1x*q2x + q1y*q2y + q1z*q2z;
	if (dot < 0.0f) {
		q2w = -q2w;
		q2x = -q2x;
		q2y = -q2y;
		q2z = -q2z;
		dot = -dot;
	}

	// Linearly interpolate and normalize if the dot product is too close to 1
	float q3w, q3x, q3y, q3z;
	if (dot > 0.9995) {
		q3w = q1w + interp * (q2w - q1w);
		q3x = q1x + interp * (q2x - q1x);
		q3y = q1y + interp * (q2y - q1y);
		q3z = q1z + interp * (q2z - q1z);
		float length = sqrtf(q3w*q3w + q3x + q3x + q3y*q3y + q3z*q3z);
		q3w /= length;
		q3x /= length;
		q3y /= length;
		q3z /= length;

		// Otherwise do a spherical linear interpolation normally
	}
	else {
		float theta_0 = acosf(dot);        // theta_0 = angle between input vectors
		float theta = theta_0 * interp;    // theta = angle between q1 and result
		float sin_theta = sinf(theta);     // compute this value only once
		float sin_theta_0 = sinf(theta_0); // compute this value only once
		float s0 = cosf(theta) - dot * sin_theta / sin_theta_0;  // == sin(theta_0 - theta) / sin(theta_0)
		float s1 = sin_theta / sin_theta_0;
		q3w = (s0 * q1w) + (s1 * q2w);
		q3x = (s0 * q1x) + (s1 * q2x);
		q3y = (s0 * q1y) + (s1 * q2y);
		q3z = (s0 * q1z) + (s1 * q2z);
	}

	// Convert the new quaternion back to a matrix
	NiMatrix33 result;
	result.data[0][0] = 1 - (2 * q3y*q3y) - (2 * q3z*q3z);
	result.data[0][1] = (2 * q3x*q3y) - (2 * q3z*q3w);
	result.data[0][2] = (2 * q3x*q3z) + (2 * q3y*q3w);
	result.data[1][0] = (2 * q3x*q3y) + (2 * q3z*q3w);
	result.data[1][1] = 1 - (2 * q3x*q3x) - (2 * q3z*q3z);
	result.data[1][2] = (2 * q3y*q3z) - (2 * q3x*q3w);
	result.data[2][0] = (2 * q3x*q3z) - (2 * q3y*q3w);
	result.data[2][1] = (2 * q3y*q3z) + (2 * q3x*q3w);
	result.data[2][2] = 1 - (2 * q3x*q3x) - (2 * q3y*q3y);
	return result;
}

static inline float distance(NiPoint3 po1, NiPoint3 po2)
{
	float x = po1.x - po2.x;
	float y = po1.y - po2.y;
	float z = po1.z - po2.z;
	return std::sqrtf(x*x + y*y + z*z);
}

static inline float magnitude(NiPoint3 p)
{
	return sqrtf(p.x*p.x + p.y*p.y + p.z*p.z);
}

static inline float magnitudePwr2(NiPoint3 p)
{
	return p.x*p.x + p.y*p.y + p.z*p.z;
}

static inline NiPoint3 crossProduct(NiPoint3 A, NiPoint3 B)
{
	return NiPoint3(A.y*B.z - A.z*B.y, A.z*B.x - A.x*B.z, A.x*B.y - A.y*B.x);
}

static inline float GetPercentageValue(float number1, float number2, float division)
{
	if (division == 1.0f)
		return number2;
	else if (division == 0)
		return number1;
	else
	{
		return number1 + ((number2 - number1)*(division));
	}
}

static inline float CalculateCollisionAmount(const NiPoint3 &a, const NiPoint3 &b, float Wradius, float Bradius)
{
	float distPwr2 = distanceNoSqrt(a, b);

	float totalRadius = Wradius + Bradius;
	if (distPwr2 < totalRadius*totalRadius)
	{
		return totalRadius - sqrtf(distPwr2);
	}
	else
		return 0;
}

static inline bool invert(NiMatrix33 matIn, NiMatrix33 & matOut)
{
	float inv[9];
	inv[0] = matIn.data[1][1] * matIn.data[2][2] - matIn.data[2][1] * matIn.data[1][2]; // = (A(1,1)*A(2,2)-A(2,1)*A(1,2))*invdet;
	inv[1] = matIn.data[1][2] * matIn.data[2][0] - matIn.data[1][0] * matIn.data[2][2]; // = (A(1,2)*A(2,0)-A(1,0)*A(2,2))*invdet;
	inv[2] = matIn.data[1][0] * matIn.data[2][1] - matIn.data[2][0] * matIn.data[1][1]; // = (A(1,0)*A(2,1)-A(2,0)*A(1,1))*invdet;
	inv[3] = matIn.data[0][2] * matIn.data[2][1] - matIn.data[0][1] * matIn.data[2][2]; // = (A(0,2)*A(2,1)-A(0,1)*A(2,2))*invdet;
	inv[4] = matIn.data[0][0] * matIn.data[2][2] - matIn.data[0][2] * matIn.data[2][0]; // = (A(0,0)*A(2,2)-A(0,2)*A(2,0))*invdet;
	inv[5] = matIn.data[2][0] * matIn.data[0][1] - matIn.data[0][0] * matIn.data[2][1]; // = (A(2,0)*A(0,1)-A(0,0)*A(2,1))*invdet;
	inv[6] = matIn.data[0][1] * matIn.data[1][2] - matIn.data[0][2] * matIn.data[1][1]; // = (A(0,1)*A(1,2)-A(0,2)*A(1,1))*invdet;
	inv[7] = matIn.data[1][0] * matIn.data[0][2] - matIn.data[0][0] * matIn.data[1][2]; // = (A(1,0)*A(0,2)-A(0,0)*A(1,2))*invdet;
	inv[8] = matIn.data[0][0] * matIn.data[1][1] - matIn.data[1][0] * matIn.data[0][1]; // = (A(0,0)*A(1,1)-A(1,0)*A(0,1))*invdet;
	double determinant =
		+matIn.data[0][0] * (matIn.data[1][1] * matIn.data[2][2] - matIn.data[2][1] * matIn.data[1][2])  //+A(0,0)*(A(1,1)*A(2,2)-A(2,1)*A(1,2))
		- matIn.data[0][1] * (matIn.data[1][0] * matIn.data[2][2] - matIn.data[1][2] * matIn.data[2][0])  //-A(0,1)*(A(1,0)*A(2,2)-A(1,2)*A(2,0))
		+ matIn.data[0][2] * (matIn.data[1][0] * matIn.data[2][1] - matIn.data[1][1] * matIn.data[2][0]); //+A(0,2)*(A(1,0)*A(2,1)-A(1,1)*A(2,0));

																										  // Can't get the inverse if determinant = 0 (divide by zero)
	if (determinant >= -0.001 || determinant <= 0.001)
		return false;

	// Invert and return the matrix
	for (int i = 0; i<9; i++)
		matOut.data[i / 3][i % 3] = inv[i] / determinant;
	return true;
}

static inline float determinant(NiPoint3 a, NiPoint3 b, NiPoint3 c)
{
	float det = 0;

	det = det + ((a.x * b.y * c.z) - (b.z * c.y));
	det = det + ((a.y * b.z * c.x) - (b.x * c.z));
	det = det + ((a.z * b.x * c.y) - (b.y * c.x));

	return det;
}

// Dot product of 2 vectors 
static inline float Dot(NiPoint3 A, NiPoint3 B)
{
	float x1, y1, z1;
	x1 = A.x * B.x;
	y1 = A.y * B.y;
	z1 = A.z * B.z;
	return (x1 + y1 + z1);
}

static inline float clamp(float val, float min, float max) {
	if (val < min) return min;
	else if (val > max) return max;
	return val;
}

static inline NiPoint3 normalize(const NiPoint3 &v)
{
	const float length_of_v = sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
	return NiPoint3(v.x / length_of_v, v.y / length_of_v, v.z / length_of_v);
}

// Gets a rotation matrix to transform one vector to another
static inline NiMatrix33 getRotation(NiPoint3 a, NiPoint3 b) {
	// Normalize the inputs
	float l1, l2;
	l1 = sqrtf(a.x*a.x + a.y*a.y + a.z*a.z);
	l2 = sqrtf(b.x*b.x + b.y*b.y + b.z*b.z);
	a /= l1;
	b /= l2;

	// Get the dot product and return an identity matrix if there's not much of an angle
	NiMatrix33 mat; // mat[row][column]
	float dotP = a.x*b.x + a.y*b.y + a.z*b.z;
	if (dotP >= 0.99999) {
		mat.Identity();
		return mat;
	}

	// Get the normalized cross product
	NiPoint3 crossP = NiPoint3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
	float cpLen = sqrtf(crossP.x*crossP.x + crossP.y*crossP.y + crossP.z*crossP.z);
	crossP /= cpLen;

	// Get the angles
	float phi = acosf(dotP);
	float rcos = cos(phi);
	float rsin = sin(phi);

	// Build the matrix
	mat.data[0][0] = rcos + crossP.x * crossP.x * (1.0 - rcos);
	mat.data[0][1] = -crossP.z * rsin + crossP.x * crossP.y * (1.0 - rcos);
	mat.data[0][2] = crossP.y * rsin + crossP.x * crossP.z * (1.0 - rcos);
	mat.data[1][0] = crossP.z * rsin + crossP.y * crossP.x * (1.0 - rcos);
	mat.data[1][1] = rcos + crossP.y * crossP.y * (1.0 - rcos);
	mat.data[1][2] = -crossP.x * rsin + crossP.y * crossP.z * (1.0 - rcos);
	mat.data[2][0] = -crossP.y * rsin + crossP.z * crossP.x * (1.0 - rcos);
	mat.data[2][1] = crossP.x * rsin + crossP.z * crossP.y * (1.0 - rcos);
	mat.data[2][2] = rcos + crossP.z * crossP.z * (1.0 - rcos);
	return mat;
}

static inline NiPoint3 NearestPointOnLine(NiPoint3 start, NiPoint3 end, NiPoint3 pnt)
{
	NiPoint3 line = (end - start);
	float len = magnitude(line);
	line = normalize(line);

	NiPoint3 v = pnt - start;
	float d = Dot(v, line);
	d = clamp(d, 0.0f, len);

	return start + line * d;
}

static inline NiPoint3 rotate(const NiPoint3& v, const NiPoint3& axis, float theta)
{
	const float cos_theta = cosf(theta);

	return (v * cos_theta) + (crossProduct(axis, v) * sinf(theta)) + (axis * Dot(axis, v)) * (1 - cos_theta);
}

static inline float angleBetweenVectors(const NiPoint3& v1, const NiPoint3& v2)
{
	return std::acos(dot(v1, v2) / (magnitude(v1) * magnitude(v2))) * 57.295776f;
}

static inline bool HighArch(const std::deque<NiPoint3> points)
{
	if (points.size() > 3)
	{
		int aboveCount = 0;
		int belowCount = 0;
		for (int i = 1; i < points.size() - 2; i++)
		{
			NiPoint3 nearest = NearestPointOnLine(points[0], points[points.size() - 1], points[i]);

			if (nearest.z > points[i].z)
				belowCount++;
			else if (nearest.z < points[i].z)
				aboveCount++;
		}
		//_MESSAGE("--->Above:%d Below:%d", aboveCount, belowCount);
		return aboveCount > belowCount;
	}
	else
		return true;
}

static inline NiPoint3 InterpolateBetweenVectors(const NiPoint3& from, const NiPoint3& to, float percentage)
{
	return normalize((normalize(to) * percentage) + (normalize(from) * (100.0f - percentage))) * magnitude(to);
}

#endif