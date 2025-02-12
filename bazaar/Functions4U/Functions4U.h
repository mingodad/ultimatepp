#ifndef _Functions4U_Functions4U_h
#define _Functions4U_Functions4U_h

#include <float.h>
#include <Draw/Draw.h>
#ifdef flagGUI
//#include <Web/Web.h>
#include "GatherTpp.h"
#endif

#include <Functions4U/SvgColors.h>
#include "StaticPlugin.h"
#include "LocalProcess2.h"
#include <random>

namespace Upp {


enum EXT_FILE_FLAGS {NO_FLAG = 0, 
					 USE_TRASH_BIN = 1,
					 BROWSE_LINKS = 2,
					 DELETE_READ_ONLY = 4
};

String GetDesktopManagerNew();

bool LaunchFile(const char *file, const char *params = nullptr, const char *directory = ".");

bool FileCat(const char *file, const char *appendFile);

int FileCompare(const char *path1, const char *path2);

int64 FindStringInFile(const char *file, const String text, int64 pos0 = 0);

bool FileStrAppend(const char *file, const char *str);
bool AppendFile(const char *filename, const char *str);

template<typename T>
String AppendFileNameX(T t) {
    return t;
}

template<typename T, typename... Args>
String AppendFileNameX(T t, Args... args) {
    return AppendFileName(t, AppendFileNameX(args...));
}
	
inline String Trim(const String& s)   {return TrimBoth(s);}
//inline WString Trim(const WString& s) {return TrimLeft(TrimRight(s));}

String FitFileName(String fileName, int len);

Vector<String> Tokenize(const String &str, const String &token, int pos = 0);
void Tokenize(const String &str, const String &token, Vector<String> &ret, int pos = 0);
String Tokenize2(const String &str, const String &token, int &pos);
String Tokenize2(const String &str, const String &token);
//String Tokenize(const String &str, const String &token, int &pos);
//String Tokenize(const String &str, const String &token);
	
/////////
bool DirectoryExistsX(const char *path, EXT_FILE_FLAGS flags = NO_FLAG); 
void DirectoryCopyX(const char *dir, const char *newPlace, bool replaceOnlyNew, String filesToExclude, String &erroList);
bool DirectoryMove(const char *dir, const char *newPlace);
bool DeleteDeepWildcardsX(const char *path, bool filefolder, EXT_FILE_FLAGS flags = NO_FLAG, bool deep = true);
bool DeleteDeepWildcardsX(const char *pathwc, const char *namewc, bool filefolder, EXT_FILE_FLAGS flags = NO_FLAG, bool deep = true);
bool DeleteFolderDeepWildcardsX(const char *path, EXT_FILE_FLAGS flags = NO_FLAG);
bool DeleteFolderDeepWildcardsX(const char *path, const char *name, EXT_FILE_FLAGS flags = NO_FLAG);
bool DeleteFileDeepWildcardsX(const char *path, EXT_FILE_FLAGS flags = NO_FLAG);
bool DeleteFileWildcardsX(const char *path, EXT_FILE_FLAGS flags = NO_FLAG);
bool DeleteFolderDeepX(const char *path, EXT_FILE_FLAGS flags = NO_FLAG);
bool RenameDeepWildcardsX(const char *path, const char *namewc, const char *newname, bool forfile, bool forfolder, EXT_FILE_FLAGS flags = NO_FLAG);
bool FolderIsEmpty(const char *path);

bool FileMoveX(const char *oldpath, const char *newpath, EXT_FILE_FLAGS flags = NO_FLAG);
bool FileDeleteX(const char *path, EXT_FILE_FLAGS flags = NO_FLAG);

bool IsRootFolder(const char *folderName);
String GetUpperFolder(const String &folderName);
String GetNextFolder(const String &folder, const String &lastFolder);
String FileRealName(const char *fileName);
bool IsFile(const char *fileName);
bool IsFolder(const char *fileName);
bool GetRelativePath(String from, String path, String& ret, bool normalize = true);
	
bool IsSymLink(const char *path);

bool SetReadOnly(const char *path, bool readOnly);
bool SetReadOnly(const char *path, bool usr, bool grp, bool oth);
bool IsReadOnly(const char *path, bool &usr, bool &grp, bool &oth);

String LoadFile_Safe(const String fileName);
String LoadFile(const char *fileName, off_t from, size_t len = 0);

int64 GetLength(const char *fileDirName); 
int64 GetDirectoryLength(const char *directoryName);

///////////////////////////////
Vector<String> SearchFile(String dir, const Vector<String> &condFiles, const Vector<String> &condFolders, 
								 const Vector<String> &extFiles,  const Vector<String> &extFolders, 
								 const String text, Vector<String> &errorList);
Vector<String> SearchFile(String dir, String condFile, String text, Vector<String> &errorList);//, int flags = 0);
Vector<String> SearchFile(String dir, String condFile = "*.*", String text = "");//, int flags = 0);
///////////////////////////////

bool FileToTrashBin(const char *path);
int64 TrashBinGetCount();
bool TrashBinClear();

String GetPersonalFolder();
String GetRootFolder();
String GetTempFolder();
String GetOsFolder();
String GetSystemFolder();
#ifdef PLATFORM_WIN32
String GetCommonAppDataFolder();
#endif
bool SetEnv(const char *id, const char *val);

struct FileData : Moveable<FileData> {
	bool isFolder;
	String fileName;
	String relFilename;
	int64 length;
	struct Upp::Time t;
	int64 id;
	
	String ToString() const { return Format("%s %0n", fileName, length); }

	FileData(bool _isFolder, String _fileName, String _relFilename, int64 _length, 
		struct Upp::Time _t, int64 _id) : isFolder(_isFolder), fileName(_fileName), 
		relFilename(_relFilename), length(_length), t(_t), id(_id) {}
	FileData() {}
};

struct FileDiffData {
	char action;	// 'n': New, 'u': Update, 'd': Delete, 'p': Problem
	bool isFolder;
	String relPath;
	String fileName;
	uint64 idMaster, idSecondary;
	struct Upp::Time tMaster, tSecondary;
	uint64 lengthMaster, lengthSecondary;
};

class ErrorHandling {
public:
	void SetLastError(String _lastError)	{lastError = _lastError;};
	String GetLastError()					{return lastError;};
	
private:
	String lastError;
};

class FileDiffArray;

class FileDataArray : public ErrorHandling {
public:
	FileDataArray(bool use = false, int fileFlags = 0);
	bool Init(String folder, FileDataArray &orig, FileDiffArray &diff);
	void Clear();
	bool Search(String dir, String condFile, bool recurse = false, String text = "");
	FileData& operator[](long i)	{return fileList[i];}
	long GetFileCount()				{return fileCount;};
	long GetFolderCount()			{return folderCount;};
	long GetCount() 				{return fileCount + folderCount;};
	int64 GetSize()					{return fileSize;};
	inline bool UseId() 			{return useId;};
	void SortByName(bool ascending = true);
	void SortByDate(bool ascending = true);
	void SortBySize(bool ascending = true);
	Vector<String> &GetLastError()	{return errorList;};
	int Find(String &relFileName, String &fileName, bool isFolder);
	int Find(FileDataArray &data, int id);
	String FullFileName(int i)		{return AppendFileName(basePath, fileList[i].fileName);};
	bool SaveFile(const char *fileName);
	bool AppendFile(const char *fileName);
	bool LoadFile(const char *fileName);

private:
	void Search_Each(String dir, String condFile, bool recurse, String text);
	int64 GetFileId(String fileName);
	String GetRelativePath(const String &fullPath);
	String GetFileText();
	
	Upp::Array<FileData> fileList;
	Vector<String> errorList;
	String basePath;
	long fileCount, folderCount;
	int64 fileSize;
	bool useId;
	int fileFlags;
};

class FileDiffArray : public ErrorHandling {
public:
	FileDiffArray();
	void Clear();
	FileDiffData& operator[](long i)	{return diffList[i];}
	bool Compare(FileDataArray &master, FileDataArray &secondary, const String folderFrom, 
		Vector<String> &excepFolders, Vector<String> &excepFiles, int sensSecs = 0);
	bool Apply(String toFolder, String fromFolder, EXT_FILE_FLAGS flags = NO_FLAG);
	long GetCount()				{return diffList.size();};
	bool SaveFile(const char *fileName);
	bool LoadFile(const char *fileName);
	String ToString();
	
private:
	Upp::Array<FileDiffData> diffList;
};

String Replace(String str, String find, String replace); 
String Replace(String str, char find, char replace);

int ReverseFind(const String& s, const String& toFind, int from = 0);

String FormatLong(long a); 
	
Upp::Time StrToTime(const char *s);
Date StrToDate(const char *s);

String BytesToString(uint64 bytes, bool units = true);

String SecondsToString(double seconds, int dec = 2, bool units = false, bool space = false, 
						bool tp = false, bool longUnits = false, bool forceSec = false);
String HMSToString(int hour, int min, double seconds, int dec = 2, bool units = false, bool space = false, 
						bool tp = false, bool longUnits = false, bool forceSec = false); 																
double StringToSeconds(String str);		
void StringToHMS(String durat, int &hour, int &min, double &seconds); 

String SeasonName(int iseason);
int GetSeason(Date &date);
	
String FormatDoubleAdjust(double d, double range);

String RemoveAccents(String str);
String RemoveAccent(wchar c);
bool IsPunctuation(wchar c);
String RemovePunctuation(String str);

template<class T>	
inline T ToRad(T angle)	{return angle*M_PI/180.;}
template<class T>
inline T ToDeg(T angle)	{return angle*180./M_PI;}
template<class T>
inline T atan2_360(T y, T x) {
	T ret = ToDeg(atan2<T>(y, x));
	return ret > 90 ? 450 - ret : 90 - ret; 
}

inline bool Odd(int val)	  		{return val%2;}
inline bool Even(int val) 	  		{return !Odd(val);}
inline int RoundEven(int val) 		{return Even(val) ? val : val+1;}
template<class T>
inline int Sign(T a) 				{return (a > 0) - (a < 0);}
template<class T>
inline T Neg(T a) 					{return a > 0 ? -a : a;}
template<class T>
inline T Average(T a, T b) 			{return T(a+b)/2;}
template<class T>
inline T Avg(T a, T b) 				{return Average(a, b);}
template<class T>
inline T Average(T a, T b, T c)		{return T(a+b+c)/3;}
template<class T>
inline T Avg(T a, T b, T c) 		{return Average(a, b, c);}
template<class T>
inline T Average(T a, T b, T c, T d){return T(a+b+c+d)/4;}
template<class T>
inline T Avg(T a, T b, T c, T d)	{return Average(a, b, c, d);}
template<class T>
inline T pow2(T a) {return a*a;}
template<class T>
inline T pow3(T a) {return a*a*a;}
template<class T>
inline T pow4(T a) {return pow2(pow2(a));}
template<class T>
inline T fround(T x, int numdec) {
	int64 mult = 10*numdec;
	return T(int64(x*mult + .5))/mult;	
}
template<typename T>
T fact(T val) {
	if (val <= 0)
		throw std::invalid_argument("fact() accepts only nonzero positive numbers");
	T ret = 1;
	while (val > 1)
		ret *= val--;
	return ret; 
}
template <class T> 
inline bool Between(const T& val, const T& min, const T& max) { 
	return val >= min && val <= max;
}
template <class T> 
inline T BetweenVal(const T& val, const T& _min, const T& _max) { 
	return max(_min, min(_max, val));
}
template <class T> 
inline bool IsNAN(T val) {return std::isnan(val);}

template <class T> 
inline T FixFloat(T val) {
	if(std::isnan(val) || std::isinf(val) || val == HUGE_VAL || val == -HUGE_VAL)
		return Null;
	return val;
}

template <class T> 
T AngleAdd360(T ang, T val) {
	ang += val;
	while (ang >= 360)
		ang -= 360;
	while (ang < 0)
		ang += 360;
	return ang;
}

template <class T> 
inline const T Norm(const T& dx, const T& dy)  { 
	return static_cast<T>(sqrt(dx*dx + dy*dy)); }
	
template <class T> 
inline const T Distance(const T& x1, const T& y1, const T& x2, const T& y2)  { 
	return Norm(x1-x2, y1-y2); }

template <class T> 
inline const T Distance(const Point_<T>& p1, const Point_<T>& p2)  { 
	return Distance<T>(p1.x, p1.y, p2.x, p2.y); }

template <class T> 
inline const T Distance(const T& x1, const T& y1, const T& z1, const T& x2, const T& y2, const T& z2)  { 
	return static_cast<T>(sqrt(pow2(x1-x2) + pow2(y1-y2) + pow2(z1-z2))); }
	
template <class T> 
inline const T Angle(const T& x1, const T& y1, const T& x2, const T& y2)  { 
	return static_cast<T>(atan2(y2-y1, x2-x1));
}

template <class T> 
inline const T Angle(const Point_<T>& p1, const Point_<T>& p2)  { 
	return Angle<T>(p1.x, p1.y, p2.x, p2.y);
}


Vector<Vector <Value> > ReadCSV(const String strFile, char separator = ',', bool bycols = true, bool removeRepeated = true, char decimalSign = '.', bool onlyStrings = false, int fromRow = 0);
Vector<Vector <Value> > ReadCSVFile(const String fileName, char separator = ',', bool bycols = true, bool removeRepeated = true, char decimalSign = '.', bool onlyStrings = false, int fromRow = 0);
bool ReadCSVFileByLine(const String fileName, Gate<int, Vector<Value>&, String &> WhenRow, char separator = ',', char decimalSign = '.', bool onlyStrings = false, int fromRow = 0);
String WriteCSV(Vector<Vector <Value> > &data, char separator = ',', bool bycols = true, char decimalSign = '.');
bool WriteCSVFile(const String fileName, Vector<Vector <Value> > &data, char separator = ',', bool bycols = true, char decimalSign = '.');

	
// A String based class to parse into
class StringParse : public String {
public:
	void GoInit()	{pos = 0; lastSeparator='\0';};
	StringParse():String("") {GoInit();};
	StringParse(String _s): String(_s) {GoInit();};
	bool GoBefore(const String text) {
		if (pos >= GetLength()) {
			pos = GetLength()-1;
			return false;
		}
		int newpos = String::Find(text, pos);
		if (newpos < 0)
			return false;	// If it does not find it, it does not move
		pos = newpos;
		return true;
	};	
	bool GoAfter(const String text) {
		if(!GoBefore(text))
			return false;
		pos += int(strlen(text));
		return true;
	};
	bool GoAfter(const String text, const String text2) {
		if(!GoAfter(text))
			return false;
		if(!GoAfter(text2))
			return false;
		return true;
	};
	bool GoAfter(const String text, const String text2, const String text3) {
		if(!GoAfter(text))
			return false;
		if(!GoAfter(text2))
			return false;
		if(!GoAfter(text3))
			return false;
		return true;
	};
	bool GoAfter_Init(const String text) {GoInit();	return GoAfter(text);};
	bool GoAfter_Init(const String text, const String text2) {GoInit();	return GoAfter(text, text2);};
	bool GoAfter_Init(const String text, const String text2, const String text3) {GoInit();	return GoAfter(text, text2, text3);};		
	
	void GoBeginLine() {
		for (; pos >= 0; --pos) {
			if ((ToString()[pos-1] == '\r') || (ToString()[pos-1] == '\n'))
				return;
		} 
	}
	bool IsBeginLine() {
		if (pos == 0)
			return true;
		if ((ToString()[pos-1] == '\r') || (ToString()[pos-1] == '\n'))
			return true;
		return false;
	}
	bool IsSpaceRN(int c) {
		if (IsSpace(c))
			return true;
		if ((c == '\r') || (c == '\n'))
		     return true;
		return false;
	}
	// Gets text between "" or just a word until an space
	// It considers special characters with \ if between ""
	// If not between "" it gets the word when it finds one of the separator characters
	String GetText(String separators = "") {
		String ret = "";
		if (pos > GetCount() || pos == -1)
			return ret;
		int newpos = pos;
		
		while ((IsSpaceRN(ToString()[newpos]) && (ToString()[newpos] != '\"') && 
			   (ToString()[newpos] != '\0')))
			newpos++;
		if (ToString()[newpos] == '\0') {
			pos = newpos;
			return "";
		}
	
		if (ToString()[newpos] == '\"') {	// Between ""
			newpos++;
			while (ToString()[newpos] != '\"' && ToString()[newpos] != '\0') {
				if (ToString()[newpos] == '\\') {
					newpos++;
					if (ToString()[newpos] == '\0')
						return "";
				} 
				ret.Cat(ToString()[newpos]);
				newpos++;
			}
			lastSeparator = '"';
		} else if (separators == "") {		// Simple word
			while (!IsSpaceRN(ToString()[newpos]) && ToString()[newpos] != '\0') {
				if (ToString()[newpos] == '\"') {
					newpos--;	// This " belongs to the next
					break;
				}
				ret.Cat(ToString()[newpos]);
				newpos++;
			}
			lastSeparator = ToString()[newpos];
		} else {							// Simple word, special separator
			while (ToString()[newpos] != '\0') {// Only consider included spaces (!IsSpaceRN(ToString()[newpos]) && ToString()[newpos] != '\0') {
				if (ToString()[newpos] == '\"') {
					newpos--;	// This " belongs to the next
					break;
				}				
				if (separators.Find(ToString()[newpos]) >= 0) {
					lastSeparator = ToString()[newpos];
					break;
				}
				ret.Cat(ToString()[newpos]);
				newpos++;
			} 
			lastSeparator = ToString()[newpos];
		}
		pos = ++newpos;		// After the separator: ", space or separator
		return ret;
	}
	String GetLine() {
		String ret;
		if (pos > GetCount() || pos == -1)
			return String();
		while (ToString()[pos] != '\0') {
			if (ToString()[pos] == '\n') {
				pos++;
				return ret;
			}
			if (ToString()[pos] == '\r' && ToString()[pos+1] == '\n') {
				pos += 2;
				return ret;
			}
			ret.Cat(ToString()[pos]);
			pos++;
		}
		return ret;
	}
	double GetDouble(String separators = "")  	{return FixFloat(atof(GetText(separators)));};
	int GetInt(String separators = "")			{return static_cast<int>(FixFloat(atof(GetText(separators))));};
	long GetLong(String separators = "")		{return static_cast<long>(FixFloat(atof(GetText(separators))));};
	uint64 GetUInt64(String separators = "")	{return static_cast<uint64>(FixFloat(atof(GetText(separators))));};
	
	String Right() 			{return String::Mid(pos+1);}
	int GetLastSeparator() 	{return lastSeparator;}
	void MoveRel(int val) {
		pos += val;
		if (pos < 0)
			pos = 0;
		else if (pos >= GetCount())
			pos = GetCount() - 1;
	}
	int GetPos() {return pos;};
	bool SetPos(int i) 
	{
		if (i < 0 || i >= GetCount())
			return false;
		else {
			pos = i;
			return true;
		}
	}
	bool Eof()
	{
		return pos >= GetCount();
	}
	unsigned Count(String _s)
	{
		int from = 0;
		unsigned count = 0;
		
		while ((from = ToString().Find(_s, from)) >= 0) {
			count++;
			from++;
		}
		return count;
	}
private:
	int pos;
	int lastSeparator;
};

#if defined(PLATFORM_WIN32) 
Value GetVARIANT(VARIANT &result);
String WideToString(LPCWSTR wcs, int len = -1);
#endif
 

String GetExtExecutable(const String ext);

Vector<String> GetDriveList();

#define DLLFunction(dll, type, function, args) auto function = (type(*)args)dll.GetFunction(#function); if (!function) throw Exc(Format("%s not found", #function))

class Dl {
public:
	Dl();
	virtual ~Dl();
	bool Load(const String &fileDll);
	void *GetFunction(const String &functionName);
	
private:
#if defined(PLATFORM_WIN32) 
	HINSTANCE hinstLib;	
#else
	void *hinstLib;
#endif
};

typedef Dl Dll;

//bool RunFromMemory(const String &progBuffer, const String &name);


String BsGetLastError();
bool BSPatch(String oldfile, String newfile, String patchfile);
bool BSDiff(String oldfile, String newfile, String patchfile);


template <class T>
Rect_<T> FitInFrame(const Size_<T> &frame, const Size_<T> &object)
{
	double frameAspect  = frame.cx/static_cast<double>(frame.cy); 
	double objectAspect = object.cx/static_cast<double>(object.cy);	
	
	if (frameAspect > objectAspect) {
		double x = (frame.cx - objectAspect*frame.cy)/2.;
		return Rect_<T>(static_cast<T>(x), 0, static_cast<T>(x + objectAspect*frame.cy), frame.cy);
	} else {
		double y = (frame.cy - frame.cx/objectAspect)/2.;
		return Rect_<T>(0, static_cast<T>(y), frame.cx, static_cast<T>(y + frame.cx/objectAspect));
	}
}

Color RandomColor();

Image GetRect(const Image& orig, const Rect &r);

double tmGetTimeX();

int SysX(const char *cmd, String& out, String& err, double timeOut = Null, 
			Gate3<double, String&, String&> progress = false, bool convertcharset = true);
			
	
class _NRFuse {
public:
	_NRFuse(bool *_inside) {inside = _inside; failed = true;}
	virtual ~_NRFuse() 			   {if (!failed) *inside = false;}
	bool failed;
private:
	bool *inside;
};

#define NON_REENTRANT_V	 	static bool _insideNR; _NRFuse _fuseNR(&_insideNR); \
							if(!_insideNR) {									\
								_insideNR = true; 								\
								_fuseNR.failed = false;							\
							} else 												\
								return
#define NON_REENTRANT(v) 	static bool _insideNR; _NRFuse _fuseNR(&_insideNR); \
							if(!_insideNR) {									\
								_insideNR = true; 								\
								_fuseNR.failed = false;							\
							} else 												\
								return v

template <class T>
struct TempAssign {
	TempAssign(T &_val, T set) {
		old = _val;
		_val = set;
		val = &_val;
	}
	virtual ~TempAssign() {
		*val = old;
	}
	
	T *val, old;
};

/*						Replaced with std::atomic
template <class T>
class ThreadSafe {
...
};*/

template <class Range>
void LinSpaced(Range &v, int n, typename Range::value_type min, typename Range::value_type max) {
	ASSERT(n > 0);
	v.SetCount(n);
	if (n == 1)
		v[0] = min;
	else {
		typename Range::value_type d = (max - min)/(n - 1);
		for (int i = 0; i < n; ++i)
			v[i] = min + d*i;
	}
}

template <class C>
static void ShuffleAscending(C &data, std::default_random_engine &generator) {
	for (int i = 0; i < data.size() - 2; i++) {
	  	std::uniform_int_distribution<int> distribution(i, data.size() - 1);
        Swap(data[i], data[distribution(generator)]);
    }
}

template <class C>
static void ShuffleDescending(C &data, std::default_random_engine &generator) {
	for (int i = data.size() - 1; i > 0; i--) {
	  	std::uniform_int_distribution<int> distribution(0, i);
        Swap(data[i], data[distribution(generator)]);
    }
}

template <class C>
void Shuffle(C &data, int randomSeed = Null) {
	if (IsNull(randomSeed))	{
		std::random_device rd;
		randomSeed = rd();
	}
	std::default_random_engine re(randomSeed);
	
	std::mt19937 generator(randomSeed);
  
	ShuffleAscending(data, re);
	ShuffleDescending(data, re);	
}

template <class T>
bool EqualRatio(const T& a, const T& b, const T& ratio, const T& zero = 0) {
	if (abs(a) <= zero) {
		if (abs(b) <= zero)
			return true;
		else {
			if(abs((zero - b)/b) <= ratio) 
				return true;
			else
				return false;
		}
	} else if (abs(b) <= zero) {
		if(abs((a - zero)/a) <= ratio) 
			return true;
		else
			return false;
	}
	if(abs((a - b)/b) <= ratio) 
		return true;
	return false;
}

template <class T>
bool EqualDecimals(const T& a, const T& b, int numdecimals) {
	return FormatDouble(a, numdecimals) == FormatDouble(b, numdecimals);
}

template <class Range>
int Find(const Range& r, const typename Range::value_type& value, int from = 0) {
	for(int i = from; i < r.size(); i++)
		if(r[i] == value) 
			return i;
	return -1;
}

template <class Range>
int FindAdd(Range& r, const typename Range::value_type& value, int from = 0) {
	int id = Find(r, value, from);
	if (id >= 0)
		return id; 
	r.Add(value);
	return r.size()-1;
}

template <class Range>
int FindRatio(const Range& r, const typename Range::value_type& value, const typename Range::value_type& ratio, int from = 0) {
	for(int i = from; i < r.size(); i++) {
		if (EqualRatio(r[i], value, ratio))
			return i;
	}
	return -1;
}

template <class Range>
int FindAddRatio(Range& r, const typename Range::value_type& value, const typename Range::value_type& ratio, int from = 0) {
	int id = FindRatio(r, value, ratio, from);
	if (id >= 0)
		return id; 
	r.Add(value);
	return r.size()-1;
}

template <class Range>
int FindDelta(const Range& r, const typename Range::value_type& value, const typename Range::value_type& delta, int from = 0) {
	for(int i = from; i < r.size(); i++) 
		if(abs(r[i] - value) <= delta) 
			return i;
	return -1;
}

template <class Range>
int FindAddDelta(Range& r, const typename Range::value_type& value, const typename Range::value_type& delta, int from = 0) {
	int id = FindDelta(r, value, delta, from);
	if (id >= 0)
		return id; 
	r.Add(value);
	return r.size()-1;
}

template <class Range>
int FindRoundDecimals(const Range& r, const typename Range::value_type& value, int numDecimals, int from = 0) {
	String svalue = FormatDouble(value, numDecimals);
	for(int i = from; i < r.size(); i++) 
		if(FormatDouble(r[i], numDecimals) == svalue) 
			return i;
	return -1;
}

template <class Range>
int FindClosest(const Range& r, const typename Range::value_type& value, int from = 0) {
	int minId = -1;
	typename Range::value_type minDiff = std::numeric_limits<typename Range::value_type>::max();
	for(int i = from; i < r.size(); i++) {
		typename Range::value_type diff = abs(value - r[i]);
		if (diff < minDiff) {
			minDiff = diff;	
			minId = i;		
		}
	}
	return minId;
}

template <class Range>
bool Compare(const Range& a, const Range& b) {
	if (a.size() != b.size())
		return false;
	for(int i = 0; i < a.size(); i++) {
		if(a[i] != b[i]) 
			return false;
	}
	return true;
}
	
template <class Range1, class Range2>
bool CompareRatio(const Range1& a, const Range2& b, const typename Range1::value_type& ratio) {
	if (a.size() != b.size())
		return false;
	for(int i = 0; i < a.size(); i++) 
		if (!EqualRatio(a[i], b[i], ratio)) 
			return false;
	return true;
}

template <class Range1, class Range2>
bool CompareDecimals(const Range1& a, const Range2& b, int numDecimals) {
	if (a.size() != b.size())
		return false;
	for(int i = 0; i < a.size(); i++) 
		if (!EqualDecimals(a[i], b[i], numDecimals)) 
			return false;
	return true;
}

template <class Range>
String ToString(const Range& a) {
	String ret;
	for(int i = 0; i < a.size(); i++) {
		if (i > 0)
			ret << ";";
		ret << a[i]; 
	}
	return ret;
}

class RealTimeStop {  
typedef RealTimeStop CLASSNAME;
public:
	RealTimeStop() {
#ifdef CTRLLIB_H	
		callbackOn = false;
		lastTick = -1;
#endif 
		Start();
	}
	void Reset() {
		timeElapsed = lastTimeElapsed = 0;
#ifdef CTRLLIB_H
		if (!callbackOn) {
			timeCallback.Set(-5*1000, THISBACK(Tick));
			callbackOn = true;
		}
#endif
		isPaused = true;
		Continue();
	}
	void Start() {Reset();}
	void Pause(bool pause) {
		if (pause)
			Pause();
		else
			Continue();
	}
	void Pause() {
		if (!isPaused) { 		
			timeElapsed += (tmGetTimeX() - time0);
			isPaused = true;
		}
	}
	void Continue() {
		if (isPaused) {
			time0 = tmGetTimeX();
			isPaused = false;
		}
	}
	double Seconds() {
		if (isPaused)
			return timeElapsed;
		else
			return timeElapsed + (tmGetTimeX() - time0);
	}
	double Elapsed() {
		double t = Seconds();
		double elapsed = t - lastTimeElapsed;
		lastTimeElapsed = t;
		return elapsed;
	}
	void SetBack(double secs) {
		timeElapsed -= secs;
	}
	bool IsPaused()		{return isPaused;}
		
private:
	double timeElapsed;				// Time elapsed
	double time0;					// Time of last Continue()
	double lastTimeElapsed;
	bool isPaused;
#ifdef CTRLLIB_H
	bool callbackOn;
	double lastTick;
	TimeCallback timeCallback;
	
	void Tick() {
		double tActual = tmGetTimeX();
		if (!isPaused && lastTick > -1) {
			double deltaLastTick = tActual - lastTick;
			if (deltaLastTick > 5*10) 	// Some external issue has stopped normal running
				SetBack(deltaLastTick);	// Timeout timer is fixed accordingly
		}
		lastTick = tActual;
	}
#endif
};

class LocalProcessX
#ifdef CTRLLIB_H	
 : public Ctrl 
 #endif
 {
typedef LocalProcessX CLASSNAME;
public:
	LocalProcessX() {}
	virtual ~LocalProcessX() 		{Stop();}
	enum ProcessStatus {RUNNING = 1, STOP_OK = 0, STOP_TIMEOUT = -1, STOP_USER = -2, STOP_NORESPONSE = -3};
	bool Start(const char *cmd, const char *envptr = 0, const char *dir = 0, double _refreshTime = -1, 
		double _maxTimeWithoutOutput = -1, double _maxRunTime = -1, bool convertcharset = true) {
		status = STOP_OK;
		p.ConvertCharset(convertcharset);
		timeElapsed.Start();
		timeWithoutOutput.Start();
		if(!p.Start(cmd, envptr, dir))
			return false;
		status = RUNNING;
		maxTimeWithoutOutput = _maxTimeWithoutOutput;
		maxRunTime = _maxRunTime;
		refreshTime = _refreshTime;
	
#ifdef CTRLLIB_H
		if (refreshTime > 0) {
			if (!callbackOn) {
				timeCallback.Set(-int(refreshTime*1000), THISBACK(Perform));
				callbackOn = true;
			}
		}
#endif
		return true;
	}
	void Perform() {
		if (status <= 0)
			return;
		String out;
		p.Read(out);
		if(p.IsRunning()) {
#ifdef PLATFORM_WIN32			
			if (!p.IsPaused()) {
#endif
				if (maxTimeWithoutOutput > 0 && timeWithoutOutput.Seconds() > maxTimeWithoutOutput) 
					status = STOP_NORESPONSE;
				else if (maxRunTime > 0 && timeElapsed.Seconds() > maxRunTime) 
					status = STOP_TIMEOUT;
#ifdef PLATFORM_WIN32				
			}
#endif
		} else 
			status = STOP_OK;
		
		bool resetTimeout = false;
		if (!out.IsEmpty())
			resetTimeout = true;
		
		if (!WhenTimer(timeElapsed.Seconds(), out, status <= 0, resetTimeout))
			status = STOP_USER;
		
		if (resetTimeout)
			timeWithoutOutput.Reset();
		
		if (status < 0)
			p.Kill();

#ifdef CTRLLIB_H		
		if (callbackOn) {
			timeCallback.Kill();
			callbackOn = false;
		}
#endif
	}
	void Stop(ProcessStatus _status = STOP_USER) {
		if (!IsRunning())
			return;
		status = _status;
		p.Kill();		
#ifdef CTRLLIB_H		
		if (callbackOn) {
			timeCallback.Kill();
			callbackOn = false;
		}
#endif		
	}
#ifdef PLATFORM_WIN32
	void Pause() {
		p.Pause();
		if (p.IsRunning()) {
			timeElapsed.Pause(p.IsPaused());
			timeWithoutOutput.Pause(p.IsPaused());
		}
	}
	bool IsPaused()			{return p.IsPaused();}
	double Seconds()		{return timeElapsed.Seconds();}
#endif
	void Write(String str) 	{p.Write(str);}
	int GetStatus()  		{return status;}
	bool IsRunning() 		{return status > 0;}
	Gate4<double, String&, bool, bool&> WhenTimer;
	#ifdef PLATFORM_WIN32
	DWORD GetPid()	{return p.GetPid();}
	#endif
	
private:
	LocalProcess2 p;
	RealTimeStop timeElapsed, timeWithoutOutput;
	ProcessStatus status = STOP_OK;
	double maxTimeWithoutOutput, maxRunTime;
	double refreshTime;
#ifdef CTRLLIB_H	
	bool callbackOn = false;
	TimeCallback timeCallback;
#endif
};

int LevenshteinDistance(const char *s, const char *t);
int DamerauLevenshteinDistance(const char *s, const char *t, int alphabetLength = 256);
int SentenceSimilitude(const char *s, const char *t);

#define S(y)	Upp::String(y)

	
template<class T>
void Jsonize(JsonIO& io, std::complex<T>& var) {
	T re, im;
	if (io.IsStoring()) {
		re = var.real();
		im = var.imag();
	}
	io("re", re)("im", im);
	if (io.IsLoading()) {
		var.real(re);
		var.imag(im);
	}
}

size_t GetNumLines(Stream &stream);

class FileInLine : public FileIn {
public:
	FileInLine(String _fileName) : FileIn(_fileName), line(0), fileName(_fileName) {};
	String GetLine() {
		line++;	
		return FileIn::GetLine();
	}
	void GetLine(int num) {
		for (int i = 0; i < num; ++i)
			GetLine();
	}
	int GetLineNumber()	const 	{return line;}
	String Str() const 			{return Format(t_("[File: '%s', line: %d]:"), fileName, line);}
	
	struct Pos {
		Pos() : byt(0), line(0) {}
		int64 byt;
		int line;
	};
	
	Pos GetPos() {
		Pos ret;
		ret.byt = FileIn::GetPos();
		ret.line = line;
		return ret;
	}
	
	void SeekPos(Pos &pos) {
		FileIn::Seek(pos.byt);
		line = pos.line;
	}
	
private:
	int line;
	String fileName;
};

class FileInBinary : public FileIn {
public:
	FileInBinary()                          	{}
	FileInBinary(const char *fn) : FileIn(fn)	{}
	
	void ReadB(void *data, size_t sz) {
		int64 len = Get64(data, sz);
		if (len != sz)
			throw Exc(Format(t_("Data not loaded in FileInBinary::Read(%ld)"), int64(sz)));
	}
	
	template <class T>
	T ReadB() {
		T data;
		ReadB(&data, sizeof(T));
		return data;
	}
	template <class T, size_t len>
	T ReadB() {
		T data;
		ReadB(&data, min(len, sizeof(T)));
		return data;
	}
};

class FileOutBinary : public FileOut {
public:
	FileOutBinary(const char *fn) : FileOut(fn)	{}
	FileOutBinary()                          		{}
	
	template <class T>
	void Write(T data) {
		Put64(&data, sizeof(T));
	}
};

class FieldSplit {
public:
	const int FIRST = 0;
	const int LAST = Null;
	
	FieldSplit(FileInLine &_in) {in = &_in;}
	
	FieldSplit& Load(String _line) {
		line = _line;
		fields = Split(line, IsSeparator, true);
		return *this;
	}
	String GetText() const {
		return line;
	}
	String GetText(int i) const {
		if (fields.IsEmpty())
			throw Exc(in->Str() + t_("No data available"));
		if (IsNull(i))
			i = fields.GetCount()-1;
		CheckId(i);
		return fields[i];
	}
	int GetInt(int i) const {
		if (fields.IsEmpty())
			throw Exc(in->Str() + t_("No data available"));
		if (IsNull(i))
			i = fields.GetCount()-1;
		CheckId(i);
		int res = ScanInt(fields[i]);
		if (IsNull(res))
			throw Exc(in->Str() + Format(t_("Bad %s '%s' in field #%d, line\n'%s'"), "integer", fields[i], i+1, line));
		return res; 
	}
	double GetDouble(int i) const {
		if (fields.IsEmpty())
			throw Exc(in->Str() + t_("No data available"));
		if (IsNull(i))
			i = fields.GetCount()-1;
		CheckId(i);
		double res = ScanDouble(fields[i]);
		if (IsNull(res))
			throw Exc(in->Str() + Format(t_("Bad %s '%s' in field #%d, line\n'%s'"), "double", fields[i], i+1, line));
		return res;
	}
	
	int size() const 		{return fields.GetCount();}
	int GetCount() const 	{return size();}
	bool IsEmpty() const 	{return size() == 0;}
	
	int (*IsSeparator)(int) = defaultIsSeparator;
		
protected:
	String line;
	Upp::Vector<String> fields;
	FileInLine *in;
	
	void CheckId(int i) const {
		if (i >= fields.GetCount() || i < 0)
			throw Exc(in->Str() + Format(t_("Field #%d not found in line\n'%s'"), i+1, line));
	}
	static int defaultIsSeparator(int c) {
		if (c == '\t' || c == ' ')
			return true;
		return false;
	}
};

enum CONSOLE_COLOR {
#ifdef PLATFORM_WIN32
    BLACK       = 0,
    BLUE        = FOREGROUND_BLUE,
    GREEN       = FOREGROUND_GREEN,
    CYAN        = FOREGROUND_GREEN | FOREGROUND_BLUE,
    RED         = FOREGROUND_RED,
    MAGENTA     = FOREGROUND_RED | FOREGROUND_BLUE,
    YELLOW      = FOREGROUND_RED | FOREGROUND_GREEN,
    GRAY        = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    LTBLUE      = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
    LTGREEN     = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
    LTCYAN      = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
    LTRED       = FOREGROUND_INTENSITY | FOREGROUND_RED,
    LTMAGENTA	= FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
    LTYELLOW    = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
    WHITE      	= FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    RESET, 
    PREVIOUS
#else
	BLACK     	= 30,
	RED     	= 31,
	GREEN   	= 32,
	YELLOW  	= 33,
	BLUE    	= 34,
	MAGENTA 	= 35,
	CYAN    	= 36,
	GRAY    	= 37,
	LTRED     	= 131,
	LTGREEN   	= 132,
	LTYELLOW  	= 133,
	LTBLUE    	= 134,
	LTMAGENTA 	= 135,
	LTCYAN    	= 136,
	WHITE    	= 137,
	RESET   	= 0,
	PREVIOUS	= 1000
#endif
};

bool SetConsoleColor(CONSOLE_COLOR color);
void ConsoleOutputDisable(bool disable);

}

#endif
